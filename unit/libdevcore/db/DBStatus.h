//
// Created by Kirill Zhukov on 24.10.2022.
//

#ifndef UNIT_DBSTATUS_H
#define UNIT_DBSTATUS_H
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
}
#endif //UNIT_DBSTATUS_H
