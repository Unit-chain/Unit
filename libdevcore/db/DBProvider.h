//
// Created by Kirill Zhukov on 24.10.2022.
//

#ifndef UNIT_DBPROVIDER_H
#define UNIT_DBPROVIDER_H
#include <memory>
#include <utility>
#include "thread"
#include "DBStatus.h"
#include "rocksdb/db.h"
#include "../datastructures/containers/list.h"

namespace dbProvider {
    class AbstractProvider {
        virtual operationDBStatus::DBCode singleWrite(std::string *key, std::string *value);
        template<class T> operationDBStatus::DBResponse<T> read(std::string *key);
        template<class T> operationDBStatus::DBResponse<T> seek(std::string *key);
    };

    operationDBStatus::DBCode AbstractProvider::singleWrite(std::string *key, std::string *value) {
        return operationDBStatus::cDefault;
    }

    template<class T>
    operationDBStatus::DBResponse<T> AbstractProvider::read(std::string *key) {
        return operationDBStatus::DBResponse<T>(operationDBStatus::cDefault, false);
    }

    template<class T>
    operationDBStatus::DBResponse<T> AbstractProvider::seek(std::string *key) {
        return operationDBStatus::DBResponse<T>(operationDBStatus::cDefault, false);
    }

    class BatchProvider : AbstractProvider {
    public:
        BatchProvider() = default;
        explicit BatchProvider(std::string path);
        std::string path;

        operationDBStatus::DBCode singleWrite(std::string *key, std::string *value) override;
        template<class T> operationDBStatus::DBCode multipleWrite(unit::list<T> *keys, unit::list<T> *values);
        template<class T> operationDBStatus::DBResponse<T> read(std::string *key);
        operationDBStatus::DBTupleResponse multiRead(std::vector<std::string> *keys);
        static std::shared_ptr<rocksdb::WriteBatch> getBatch();
        operationDBStatus::DBCode commitBatch(const std::shared_ptr<rocksdb::WriteBatch>& batch);
        inline BatchProvider &operator=(const BatchProvider &c2) {
            this->path = c2.path;
            this->options = c2.options;
            this->writeOptions = c2.writeOptions;
            this->readOptions = c2.readOptions;
            return *this;
        }
    private:
        friend class AbstractProvider;
    protected:
        static void closeDB(rocksdb::DB **dbPtr);
        rocksdb::Options options;
        rocksdb::WriteOptions writeOptions;
        rocksdb::ReadOptions readOptions;
    };

    class TransactionProvider : AbstractProvider {
    public:
        explicit TransactionProvider(std::string path);
        const std::string path;
    private:
        friend class AbstractProvider;
    protected:
        static void closeDB(rocksdb::DB **dbPtr);
        rocksdb::Options options;
        rocksdb::WriteOptions writeOptions;
        rocksdb::ReadOptions readOptions;
    };

    TransactionProvider::TransactionProvider(std::string path) : path(std::move(path)) {
        this->options.create_if_missing = true;
        this->options.create_missing_column_families = true;
        options.unordered_write=true;
        options.bottommost_compression = rocksdb::kZSTD;
        options.compression = rocksdb::kLZ4Compression;
    }

    std::shared_ptr<rocksdb::WriteBatch> BatchProvider::getBatch() {
        return std::make_shared<rocksdb::WriteBatch>(rocksdb::WriteBatch());
    }

    operationDBStatus::DBCode BatchProvider::singleWrite(std::string *key, std::string *value) {
        rocksdb::DB *db;
        rocksdb::Status status = rocksdb::DB::Open(this->options, this->path, &db);
        if (status.code() == rocksdb::Status::Code::kCorruption) return operationDBStatus::DBCode::cCorruption;
        while (status.code() != rocksdb::Status::Code::kOk) {
            status = rocksdb::DB::Open(this->options, this->path, &db);
            if (status.code() != rocksdb::Status::Code::kOk) std::this_thread::sleep_for(std::chrono::seconds(3));
        }
        rocksdb::WriteBatch batch = rocksdb::WriteBatch();
        batch.Put(*key, *value);
        status = db->Write(rocksdb::WriteOptions(), &batch);
        closeDB(&db);
        return operationDBStatus::cOk;
    }

    BatchProvider::BatchProvider(std::string path) : path(std::move(path)) {
        int cpuPiece = (int) std::thread::hardware_concurrency() / 2;
        this->options.create_if_missing = true;
        this->options.create_missing_column_families = true;
        this->options.unordered_write=true;
        this->options.bottommost_compression = rocksdb::kZSTD;
        this->options.compression = rocksdb::kLZ4Compression;
        this->options.IncreaseParallelism(cpuPiece);
        this->options.max_background_jobs = (cpuPiece / 2  <= 2) ? 2 : cpuPiece / 2;
    }

    void BatchProvider::closeDB(rocksdb::DB **dbPtr) {
        delete *dbPtr;
    }

    template<class T>
    operationDBStatus::DBCode BatchProvider::multipleWrite(unit::list<T> *keys, unit::list<T> *values) {
        if (keys->size() != values->size()) return operationDBStatus::DBCode::cIOError;
        rocksdb::DB *db;
        rocksdb::Status status = rocksdb::DB::Open(this->options, this->path, &db);
        if (status.code() == rocksdb::Status::Code::kCorruption) return operationDBStatus::DBCode::cCorruption;
        while (status.code() != rocksdb::Status::Code::kOk) {
            status = rocksdb::DB::Open(this->options, this->path, &db);
            if (status.code() != rocksdb::Status::Code::kOk) std::this_thread::sleep_for(std::chrono::seconds(3));
        }
        rocksdb::WriteBatch batch = rocksdb::WriteBatch();
        for(int i = 0; i < keys->size(); i++) batch.Put(keys[i], values[i]);
        status = db->Write(rocksdb::WriteOptions(), &batch);
        closeDB(&db);
        return operationDBStatus::cOk;
    }

    template<class T>
    operationDBStatus::DBResponse<T> BatchProvider::read(std::string *key) {
        rocksdb::DB *db;
        rocksdb::Status status = rocksdb::DB::OpenForReadOnly(this->options, this->path, &db);
        if (status.code() == rocksdb::Status::Code::kCorruption) return {operationDBStatus::DBCode::cCorruption, true};
        if (status.code() == rocksdb::Status::Code::kIOError) return {operationDBStatus::DBCode::cIOError, true};
        while (status.code() != rocksdb::Status::Code::kOk) {
            status = rocksdb::DB::OpenForReadOnly(this->options, this->path, &db);
            if (status.code() != rocksdb::Status::Code::kOk) std::this_thread::sleep_for(std::chrono::seconds(3));
        }
        std::string response;
        status = db->Get(rocksdb::ReadOptions(), rocksdb::Slice(*key), &response);
        if (!status.ok()) {
            if (status.code() == rocksdb::Status::Code::kNotFound) {
                closeDB(&db);
                return {operationDBStatus::DBCode::cNotFound, true};
            }
            while (status.code() != rocksdb::Status::Code::kOk) {
                status = db->Get(rocksdb::ReadOptions(), rocksdb::Slice(*key), &response);
                std::this_thread::sleep_for(std::chrono::seconds ( 3));
            }
        }
        return {false, response};
    }

    operationDBStatus::DBCode BatchProvider::commitBatch(const std::shared_ptr<rocksdb::WriteBatch>& batch) {
        rocksdb::DB *db;
        rocksdb::Status status = rocksdb::DB::Open(this->options, this->path, &db);
        if (status.code() == rocksdb::Status::Code::kCorruption) return operationDBStatus::DBCode::cCorruption;
        if (status.code() == rocksdb::Status::Code::kIOError) return operationDBStatus::DBCode::cIOError;
        while (status.code() != rocksdb::Status::Code::kOk) {
            status = rocksdb::DB::OpenForReadOnly(this->options, this->path, &db);
            if (status.code() != rocksdb::Status::Code::kOk) std::this_thread::sleep_for(std::chrono::seconds(3));
            return operationDBStatus::DBCode::cIOError;
        }
        status = db->Write(this->writeOptions, batch.get());
        if (status.code() == rocksdb::Status::Code::kCorruption) return operationDBStatus::DBCode::cCorruption;
        closeDB(&db);
        return operationDBStatus::cOk;
    }

    operationDBStatus::DBTupleResponse BatchProvider::multiRead(std::vector<std::string> *keys) {
        rocksdb::DB *db;
        rocksdb::Status status = rocksdb::DB::OpenForReadOnly(this->options, this->path, &db);
        if (status.code() == rocksdb::Status::Code::kCorruption) return {true, operationDBStatus::DBCode::cCorruption};
        if (status.code() == rocksdb::Status::Code::kIOError) return {true, operationDBStatus::DBCode::cIOError};
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
        return {statuses, response};
    }
}
#endif //UNIT_DBPROVIDER_H
