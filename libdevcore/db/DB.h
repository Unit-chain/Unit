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
        std::string path;
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
        virtual std::shared_ptr<rocksdb::DB *> newDB() = 0;
        static std::shared_ptr<rocksdb::WriteBatch> getBatch() { return std::make_shared<rocksdb::WriteBatch>(rocksdb::WriteBatch()); }
        static inline void close(rocksdb::DB **db) { delete *db; *db = nullptr; }
        static inline void close(const std::shared_ptr<rocksdb::DB*> &db) { delete *db; *db = nullptr; }
        virtual std::string get(std::string &key) = 0;
        virtual std::string getWithIO(std::string &key) = 0;
        virtual std::string get(std::string &key, std::shared_ptr<rocksdb::DB*> &db) = 0;
        virtual std::tuple<std::vector<rocksdb::Status>, std::vector<std::string>> multiGet(std::vector<std::string> *keys) = 0;
        virtual std::variant<std::string, std::exception> seek(std::string &key) = 0;
        virtual std::variant<bool, std::exception> has(std::string &key) = 0;
        virtual std::variant<bool, std::exception> has(std::shared_ptr<rocksdb::Snapshot*> &snapshot) = 0;
        virtual std::exception put(std::string &key, std::string &value) = 0;
        virtual std::optional<std::exception> deleteKey(std::string &key) = 0;
        virtual std::optional<std::exception> commit(std::shared_ptr<rocksdb::WriteBatch> &batch) = 0;
        virtual std::variant<std::shared_ptr<rocksdb::Snapshot*>, std::exception> getSnapshot(std::shared_ptr<rocksdb::DB*> &dbPtr) = 0;
        // should be used as function for checking if error equals to other
        template<class U, class V>
        inline static bool isSameErrors(U &u, V &v) { return std::is_same_v<decltype(u), decltype(v)>; }
        // should be used as function for checking if error equals to other
        template<class U, class V>
        inline static bool isSameError(U &u) { return std::is_same_v<decltype(u), V>; }
        template<class U, class V>
        inline static bool isSameError(U &u, V &v) { return std::is_same_v<U, V>; }
        inline DB &operator=(const DB &c2) = default;
        [[nodiscard]] inline std::string getPath() const { return this->path; }
    private:
        rocksdb::ReadOptions readOptions;
    protected:
        rocksdb::Options options;
        rocksdb::WriteOptions writeOptions;
    };

    class BasicDB : public DB {
    public:
        BasicDB() = default;
        explicit BasicDB(const std::string &path) : DB(path) {}
        std::string get(std::string &key) override;
        std::shared_ptr<rocksdb::DB *> newDB() override;
        std::string get(std::string &key, std::shared_ptr<rocksdb::DB*> &db);
        std::string getWithIO(std::string &key);
        std::optional<std::exception> commit(std::shared_ptr<rocksdb::WriteBatch> &batch) override;
        std::tuple<std::vector<rocksdb::Status>, std::vector<std::string>> multiGet(std::vector<std::string> *keys) override;
        [[deprecated("not implemented")]]virtual std::variant<bool, std::exception> has(std::shared_ptr<rocksdb::Snapshot*> &snapshot) override;
        [[deprecated("not implemented")]]virtual std::exception put(std::string &key, std::string &value) override;
        [[deprecated("not implemented")]]virtual std::optional<std::exception> deleteKey(std::string &key) override;
        [[deprecated("not implemented")]]virtual std::variant<std::shared_ptr<rocksdb::Snapshot*>, std::exception> getSnapshot(std::shared_ptr<rocksdb::DB*> &dbPtr) override;
        [[deprecated("not implemented")]]std::variant<std::string, std::exception> seek(std::string &key) override;
        [[deprecated("not implemented")]]std::variant<bool, std::exception> has(std::string &key) override;
    private:
    };

    std::string BasicDB::get(std::string &key) {
        rocksdb::DB *db;
        rocksdb::Status status = rocksdb::DB::OpenForReadOnly(this->options, this->path, &db);
        if (status.code() == rocksdb::Status::Code::kCorruption) throw unit::error::DBCorruption();
        while (status.code() != rocksdb::Status::Code::kOk) {
            status = rocksdb::DB::OpenForReadOnly(this->options, this->path, &db);
            if (status.code() != rocksdb::Status::Code::kOk) std::this_thread::sleep_for(std::chrono::seconds(3));
        }
        std::string response;
        status = db->Get(rocksdb::ReadOptions(), rocksdb::Slice(key), &response);
        if (!status.ok()) {
            if (status.code() == rocksdb::Status::Code::kNotFound) {
                DB::close(&db);
                throw unit::error::DBNotFound();
            }
            while (status.code() != rocksdb::Status::Code::kOk) {
                status = db->Get(rocksdb::ReadOptions(), rocksdb::Slice(key), &response);
                std::this_thread::sleep_for(std::chrono::seconds ( 3));
            }
        }
        return {response};
    }

    std::string BasicDB::getWithIO(std::string &key) {
        rocksdb::DB *db;
        rocksdb::Status status = rocksdb::DB::OpenForReadOnly(this->options, this->path, &db);
        if (status.code() == rocksdb::Status::Code::kCorruption) throw unit::error::DBCorruption();
        if (status.code() == rocksdb::Status::Code::kIOError) throw unit::error::DBIOError();
        while (status.code() != rocksdb::Status::Code::kOk) {
            status = rocksdb::DB::OpenForReadOnly(this->options, this->path, &db);
            if (status.code() != rocksdb::Status::Code::kOk) std::this_thread::sleep_for(std::chrono::seconds(3));
        }
        std::string response;
        status = db->Get(rocksdb::ReadOptions(), rocksdb::Slice(key), &response);
        if (!status.ok()) {
            if (status.code() == rocksdb::Status::Code::kNotFound) {
                DB::close(&db);
                throw unit::error::DBNotFound();
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

    std::shared_ptr<rocksdb::DB *> BasicDB::newDB() {
        rocksdb::DB *db;
        rocksdb::Status status = rocksdb::DB::OpenForReadOnly(this->options, this->path, &db);
        if (status.code() == rocksdb::Status::Code::kCorruption) throw unit::error::DBCorruption();
        if (status.code() == rocksdb::Status::Code::kIOError) throw unit::error::DBIOError();
        while (status.code() != rocksdb::Status::Code::kOk) {
            status = rocksdb::DB::OpenForReadOnly(this->options, this->path, &db);
            if (status.code() != rocksdb::Status::Code::kOk) std::this_thread::sleep_for(std::chrono::seconds(3));
        }
        return std::make_shared<rocksdb::DB*>(db);
    }

    std::tuple<std::vector<rocksdb::Status>, std::vector<std::string>> BasicDB::multiGet(std::vector<std::string> *keys) {
        rocksdb::DB *db;
        rocksdb::Status status = rocksdb::DB::OpenForReadOnly(this->options, this->path, &db);
        if (status.code() == rocksdb::Status::Code::kCorruption) throw unit::error::DBCorruption();
        if (status.code() == rocksdb::Status::Code::kIOError) throw unit::error::DBIOError();
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

    std::variant<std::string, std::exception> BasicDB::seek(std::string &key) {
        return {};
    }

    std::variant<bool, std::exception> BasicDB::has(std::string &key) {
        return {};
    }
    std::exception BasicDB::put(std::string &key, std::string &value) {
        return {};
    }
    std::optional<std::exception> BasicDB::deleteKey(std::string &key) {
        return std::exception();
    }
    std::variant<std::shared_ptr<rocksdb::Snapshot*>, std::exception> BasicDB::getSnapshot(std::shared_ptr<rocksdb::DB*> &dbPtr) {
        return {};
    }

    std::variant<bool, std::exception> BasicDB::has(std::shared_ptr<rocksdb::Snapshot *> &snapshot) {
        return {};
    }

    std::string BasicDB::get(std::string &key, std::shared_ptr<rocksdb::DB *> &db) {
        std::string response;
        std::vector<rocksdb::ColumnFamilyHandle*> handles;
        rocksdb::Status status = (*db)->Get(rocksdb::ReadOptions(), rocksdb::Slice(key), &response);
        if (!status.ok()) {
            if (status.code() == rocksdb::Status::Code::kNotFound) {
                close(db);
                throw unit::error::DBNotFound();
            }
            while (status.code() != rocksdb::Status::Code::kOk) {
                status = (*db)->Get(rocksdb::ReadOptions(), rocksdb::Slice(key), &response);
                std::this_thread::sleep_for(std::chrono::seconds(2));
            }
        }
//        close(db);
        return {response};
    }
}
#endif //UNIT_DB_H