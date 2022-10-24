//
// Created by Kirill Zhukov on 24.10.2022.
//

#ifndef UNIT_DBWRITER_H
#define UNIT_DBWRITER_H
#include <utility>

#include "DBStatus.h"
#include "rocksdb/db.h"

namespace writer {
    class AbstractWriter {
        virtual void write(std::string *key, std::string *value);
        virtual void read(std::string *key);
        virtual void deleteKey(std::string *key);
    };

    class BatchWriter : public AbstractWriter {
    public:
        explicit BatchWriter(std::string path);
        std::string path;
        void write(std::string *key, std::string *value) override;
    };

    void BatchWriter::write(std::string *key, std::string *value) {

    }

    BatchWriter::BatchWriter(std::string path) : path(std::move(path)) {}
}
#endif //UNIT_DBWRITER_H
