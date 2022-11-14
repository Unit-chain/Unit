//
// Created by Kirill Zhukov on 14.11.2022.
//

#ifndef UNIT_DBERROR_H
#define UNIT_DBERROR_H
#include "iostream"

namespace unit::error {
    class DBCorruption : public std::exception {
        std::string what() { return "DB corruption"; }
    };
    class DBBusy : public std::exception {
        std::string what() { return "DB busy"; }
    };
    class DBNotFound : public std::exception {
        std::string what() { return "DB not found"; }
    };
    class DBNotSupported : public std::exception {
        std::string what() { return "DB not supported"; }
    };
    class DBInvalidArgument : public std::exception {
        std::string what() { return "DB invalid argument"; }
    };
    class DBIOError : public std::exception {
        std::string what() { return "DB IO error"; }
    };
    class DBDefault : public std::exception {
        std::string what() { return "DB default"; }
    };
}

#endif //UNIT_DBERROR_H
