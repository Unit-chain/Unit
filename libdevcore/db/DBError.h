//
// Created by Kirill Zhukov on 14.11.2022.
//

#ifndef UNIT_DBERROR_H
#define UNIT_DBERROR_H
#include "iostream"

namespace unit::error {
    class DBCorruption : public std::exception {
        [[nodiscard]] const char * what() const noexcept override { return "DB corruption"; }
    };
    class DBBusy : public std::exception {
        [[nodiscard]] const char * what() const noexcept override { return "DB busy"; }
    };
    class DBNotFound : public std::exception {
        [[nodiscard]] const char * what() const noexcept override { return "DB not found"; }
    };
    class DBNotSupported : public std::exception {
        [[nodiscard]] const char * what() const noexcept override { return "DB not supported"; }
    };
    class DBInvalidArgument : public std::exception {
        [[nodiscard]] const char * what() const noexcept override { return "DB invalid argument"; }
    };
    class DBIOError : public std::exception {
    public:
        [[nodiscard]] const char * what() const noexcept override { return "DB IO error"; }
    };
    class DBDefault : public std::exception {
        [[nodiscard]] const char * what() const noexcept override { return "DB default"; }
    };
}

#endif //UNIT_DBERROR_H
