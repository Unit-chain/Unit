//
// Created by Kirill Zhukov on 16.11.2022.
//

#ifndef UNIT_UNITINITIATOR_H
#define UNIT_UNITINITIATOR_H
#include "iostream"
#include "thread"

class NoArgsException : public std::exception {
    std::string what() {
        return "No argc were given";
    }
};

class UnitInitiator {
public:
    [[noreturn]] static void init(int argc, char **argv);
};

void UnitInitiator::init(int argc, char **argv) {
    std::cout << "argc count: " << argc << std::endl;
    if (argc == 1) throw NoArgsException();
    for (int i = 0; i < argc; i++) printf("argv[%d] = %s\n", i, argv[i]);
}

#endif //UNIT_UNITINITIATOR_H
