//
// Created by Kirill Zhukov on 14.11.2022.
//

#ifndef UNIT_DB_H
#define UNIT_DB_H
#include <utility>
#include <variant>
#include "iostream"
#include "thread"
#include "type_traits"
#include "rocksdb/db.h"
#include "DBError.h"


namespace unit {
    class DB {
    public:
        DB() = default;

        explicit DB(std::string path) : path(std::move(path)) {
            int cpuPiece = (int) std::thread::hardware_concurrency() / 2;
            this->options.create_if_missing = true;
            this->options.create_missing_column_families = true;
            this->options.unordered_write = true;
            this->options.bottommost_compression = rocksdb::kZSTD;
            this->options.compression = rocksdb::kLZ4Compression;
            this->options.IncreaseParallelism(cpuPiece);
            this->options.max_background_jobs = (cpuPiece / 2  <= 2) ? 2 : cpuPiece / 2;
        }

        virtual std::variant<std::shared_ptr<rocksdb::DB*>, std::exception> newDB() = 0;
        virtual std::variant<std::shared_ptr<rocksdb::WriteBatch>, std::exception> getBatch() = 0;
        static inline void close(rocksdb::DB **db) { delete *db; }
        static inline void close(const std::shared_ptr<rocksdb::DB*> &db) { delete *db; }
        virtual std::variant<std::string, std::exception> get(std::string &key) = 0;
        virtual std::variant<std::tuple<std::vector<rocksdb::Status>, std::vector<std::string>>, std::exception> multiGet(std::vector<std::string> *keys) = 0;
        virtual std::variant<std::string, std::exception> seek(std::string &key) = 0;
        virtual std::variant<bool, std::exception> has(std::string &key) = 0;
        virtual std::variant<bool, std::exception> has(std::shared_ptr<rocksdb::Snapshot*> &snapshot) = 0;
        virtual std::variant<void, std::exception> put(std::string &key, std::string &value) = 0;
        virtual std::optional<std::exception> deleteKey(std::string &key) = 0;
        virtual std::optional<std::exception> commit(std::shared_ptr<rocksdb::WriteBatch> &batch) = 0;
        virtual std::variant<std::shared_ptr<rocksdb::Snapshot*>, std::exception> getSnapshot(std::shared_ptr<rocksdb::DB*> &dbPtr) = 0;
        // should be used as function for checking if error equals to other
        template<class U, class V>
        inline bool isSameErrors(U &u, V &v) { return std::is_same_v<decltype(u), decltype(v)>; }
        // should be used as function for checking if error equals to other
        template<class U, class V>
        inline bool isSameError(U &u) { return std::is_same_v<decltype(u), V>; }
    private:
        rocksdb::ReadOptions readOptions;
    protected:
        rocksdb::Options options;
        const std::string path;
        rocksdb::WriteOptions writeOptions;
    };

    class BasicDB : private DB {
    public:
        explicit BasicDB(const std::string &path) : DB(path) {}
        std::variant<std::shared_ptr<rocksdb::WriteBatch>, std::exception> getBatch() override;
        std::variant<std::string, std::exception> get(std::string &key) override;
        std::variant<std::string, std::exception> seek(std::string &key) override;
        std::variant<bool, std::exception> has(std::string &key) override;
        std::optional<std::exception> commit(std::shared_ptr<rocksdb::WriteBatch> &batch) override;
        std::variant<std::shared_ptr<rocksdb::DB*>, std::exception> newDB() override;
        std::variant<std::tuple<std::vector<rocksdb::Status>, std::vector<std::string>>, std::exception> multiGet(std::vector<std::string> *keys) override;
    private:
    };

    std::variant<std::shared_ptr<rocksdb::WriteBatch>, std::exception> BasicDB::getBatch() {
        return std::make_shared<rocksdb::WriteBatch>(rocksdb::WriteBatch());
    }

    std::variant<std::string, std::exception> BasicDB::get(std::string &key) {
        rocksdb::DB *db;
        rocksdb::Status status = rocksdb::DB::OpenForReadOnly(this->options, this->path, &db);
        if (status.code() == rocksdb::Status::Code::kCorruption) return {unit::error::DBCorruption()};
        if (status.code() == rocksdb::Status::Code::kIOError) return {unit::error::DBIOError()};
        while (status.code() != rocksdb::Status::Code::kOk) {
            status = rocksdb::DB::OpenForReadOnly(this->options, this->path, &db);
            if (status.code() != rocksdb::Status::Code::kOk) std::this_thread::sleep_for(std::chrono::seconds(3));
        }
        std::string response;
        status = db->Get(rocksdb::ReadOptions(), rocksdb::Slice(key), &response);
        if (!status.ok()) {
            if (status.code() == rocksdb::Status::Code::kNotFound) {
                DB::close(&db);
                return {unit::error::DBNotFound()};
            }
            while (status.code() != rocksdb::Status::Code::kOk) {
                status = db->Get(rocksdb::ReadOptions(), rocksdb::Slice(key), &response);
                std::this_thread::sleep_for(std::chrono::seconds ( 3));
            }
        }
        return {response};
    }

    std::optional<std::exception> BasicDB::commit(std::shared_ptr<rocksdb::WriteBatch> &batch) {
        rocksdb::DB *db;
        rocksdb::Status status = rocksdb::DB::Open(this->options, this->path, &db);
        if (status.code() == rocksdb::Status::Code::kCorruption) return {unit::error::DBCorruption()};
        if (status.code() == rocksdb::Status::Code::kIOError) return {unit::error::DBIOError()};
        while (status.code() != rocksdb::Status::Code::kOk) {
            status = rocksdb::DB::OpenForReadOnly(this->options, this->path, &db);
            if (status.code() != rocksdb::Status::Code::kOk) std::this_thread::sleep_for(std::chrono::seconds(3));
            return {unit::error::DBIOError()};
        }
        status = db->Write(this->writeOptions, batch.get());
        if (status.code() == rocksdb::Status::Code::kCorruption) return {unit::error::DBCorruption()};
        close(&db);
        return std::nullopt;
    }

    std::variant<std::shared_ptr<rocksdb::DB *>, std::exception> BasicDB::newDB() {
        rocksdb::DB *db;
        rocksdb::Status status = rocksdb::DB::OpenForReadOnly(this->options, this->path, &db);
        if (status.code() == rocksdb::Status::Code::kCorruption) return {unit::error::DBCorruption()};
        if (status.code() == rocksdb::Status::Code::kIOError) return {unit::error::DBIOError()};
        while (status.code() != rocksdb::Status::Code::kOk) {
            status = rocksdb::DB::OpenForReadOnly(this->options, this->path, &db);
            if (status.code() != rocksdb::Status::Code::kOk) std::this_thread::sleep_for(std::chrono::seconds(3));
            return {unit::error::DBIOError()};
        }
        return std::make_shared<rocksdb::DB*>(db);
    }

    std::variant<std::tuple<std::vector<rocksdb::Status>, std::vector<std::string>>, std::exception> BasicDB::multiGet(std::vector<std::string> *keys) {
        rocksdb::DB *db;
        rocksdb::Status status = rocksdb::DB::OpenForReadOnly(this->options, this->path, &db);
        if (status.code() == rocksdb::Status::Code::kCorruption) return {unit::error::DBCorruption()};
        if (status.code() == rocksdb::Status::Code::kIOError) return {unit::error::DBIOError()};
        std::vector<rocksdb::Slice> slices{};
        slices.reserve(keys->size());
        while (status.code() != rocksdb::Status::Code::kOk) {
            status = rocksdb::DB::OpenForReadOnly(this->options, this->path, &db);
            if (status.code() != rocksdb::Status::Code::kOk) {
                if (slices.empty()) for (auto &key : (*keys)) slices.emplace_back(rocksdb::Slice(key));
                else std::this_thread::sleep_for(std::chrono::seconds(3));
                std::this_thread::sleep_for(std::chrono::seconds(3));
            }
        }
        if (slices.empty()) for (const auto& key : *keys) slices.emplace_back(rocksdb::Slice(key));
        std::vector<std::string> response;
        std::vector<rocksdb::Status> statuses = db->MultiGet(rocksdb::ReadOptions(), slices, &response);
        return {std::make_tuple(std::move(statuses), std::move(response))};
    }
}
#endif //UNIT_DB_H
