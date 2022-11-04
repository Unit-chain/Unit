//
// Created by Kirill Zhukov on 24.10.2022.
//

#ifndef UNIT_DBSTATUS_H
#define UNIT_DBSTATUS_H
#include <utility>

#include "rocksdb/db.h"

using namespace rocksdb;
using namespace std;

namespace operationDBStatus {
    enum DBCode {
        cOk = 0,
        cDBBusy = 1,
        cNotFound = 2,
        cCorruption = 3,
        cNotSupported = 4,
        cInvalidArgument = 5,
        cIOError = 6,
        cDefault = 7
    };

    template<class T>
    class [[maybe_unused]] DBResponse {
    public:
        DBResponse(DBCode dbCodeResponse, bool error, T value) : errorResponse(dbCodeResponse), error(error),
                                                                 value(new T(value)) {}
        DBResponse(bool error, T value) : error(error), value(new T(value)) {
            this->errorResponse = DBCode::cOk;
        }
        DBResponse(DBCode errorResponse, bool error) : errorResponse(errorResponse), error(error) {}
        DBCode errorResponse;
        bool error = false;
        T *value;
    };

    class [[maybe_unused]] DBTupleResponse {
    public:
        DBTupleResponse(std::vector<Status> statuses, std::vector<string> values) : statuses(std::move(statuses)) , responses(std::move(values)) {}
        DBTupleResponse(bool error, DBCode dbCode) : error(error), dbCode(dbCode) {}
        /// structures below
        std::vector<Status> statuses;
        std::vector<string> responses;
        bool error{};
        DBCode dbCode;
        /// functions
        [[nodiscard]] bool hasValue () const {
            return !error;
        }
        [[nodiscard]] std::tuple<std::vector<Status>, std::vector<string>> getValue() const {
            return std::make_tuple(statuses, responses);
        }
    };
}
#endif //UNIT_DBSTATUS_H
