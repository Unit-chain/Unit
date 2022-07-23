//
// Created by Kirill Zhukov on 18.07.2022.
//

#ifndef VM_TEST_IMPLEMENTATION_LOGDURATION_H
#define VM_TEST_IMPLEMENTATION_LOGDURATION_H
#include "iostream"

class LogDuration {
public:
    LogDuration(std::string id)
            : id_(std::move(id)) {
    }

    ~LogDuration() {
        const auto end_time = std::chrono::steady_clock::now();
        const auto dur = end_time - start_time_;
        std::cout << std::endl << id_ << ": ";
        std::cout << "operation time"
                  << ": " << std::chrono::duration_cast<std::chrono::milliseconds>(dur).count()
                  << " ms" << std::endl;
    }

private:
    const std::string id_;
    const std::chrono::steady_clock::time_point start_time_ = std::chrono::steady_clock::now();
};

//    std::vector<Object> vector(1e7, Object("test"));
//    {
//        LogDuration logDuration = LogDuration("test");
//        std::vector<Object> vector1(std::move(vector));
//    }
//
//    std::vector<Object> vector1(1e7, Object());
//    {
//        LogDuration logDuration = LogDuration("test");
//        for (auto &obj : vector1) {
//            if(obj.object == nullptr)
//                1;
//        }
//    }

#endif //VM_TEST_IMPLEMENTATION_LOGDURATION_H
