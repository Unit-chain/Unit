//
// Created by Kirill Zhukov on 24.10.2022.
//

#ifndef UNIT_DBSTATUS_H
#define UNIT_DBSTATUS_H
namespace operationDBStatus {
    enum DBCode {
        cOk = 0,
        cDBBusy = 1,
        cNotFound = 2
    };

    template<class T>
    class [[maybe_unused]] DBResponse {
    public:
        DBResponse(DBCode errorResponse, bool error, T value) : errorResponse(errorResponse), error(error),
                                                                value(value) {}
        DBCode errorResponse;
        bool error = false;
        T value;
    };
}
#endif //UNIT_DBSTATUS_H
