//
// Created by Kirill Zhukov on 16.11.2022.
//

#ifndef UNIT_UNITINITIATOR_H
#define UNIT_UNITINITIATOR_H
#include "iostream"
#include "thread"
#include "blockHandler/BlockHandler.h"
#include "blockHandler/ShardInserter.h"
#include "server/Server.h"

class NoArgsException : public std::exception {
    [[nodiscard]] const char * what() const noexcept override {
        return "No argc were given.";
    }
};
class MissingImportantFlags : public std::exception {
    [[nodiscard]] const char * what() const noexcept override {
        return "Missed important flags.";
    }
};
class InvalidFlag : public std::exception {
public:
    [[nodiscard]] const char * what() const noexcept override {
        return "Invalid flag.";
    }
};

// I know it's awful, but it'll exist till end of tests
[[noreturn]] void blockHandlingHandler(std::string &blockPath, std::string &userPath, std::string &transactionPath,
                                       std::string &historyPath, std::string &tokenPath, BIP44Result &bip44Result, unit::list<Shard> *shardList) {
    BlockHandler blockHandler = BlockHandler(blockPath, userPath, transactionPath, historyPath, tokenPath, bip44Result, shardList);
    blockHandler.startBlockGenerator();
}
[[noreturn]] void shardInserterHandler(unit::list<Shard> *shardList, TransactionPool *transactionPool, BIP44Result *bip44Result) {
    BasicLocalShardInserter shardInserter = BasicLocalShardInserter(shardList, transactionPool, *bip44Result);
    shardInserter.shardFactory();
}
[[noreturn]] void transactionServerHandler(TransactionPool *transactionPool, PendingPool *pool, std::string &userDBPath, std::string &historyPath,
                                           std::string &blockPath, std::string &transactionPath) {
    Server::start_server(transactionPool, pool, userDBPath, historyPath, blockPath, transactionPath);
}

class UnitInitiator {
public:
    [[noreturn]] static void init(int argc, char **argv);
    [[noreturn]] static void init(int argc, std::string argv[42]);
};

//void UnitInitiator::init(int argc, char **argv) {
//    std::cout << "argc count: " << argc << std::endl;
//    if (argc == 1) throw NoArgsException();
//    std::map<std::string, std::string> flagsValues{{"--userpath", {}},
//                                                   {"--historyPath", {}},
//                                                   {"--blockPath", {}},
//                                                   {"--transactionPath", {}},
//                                                   {"--tokenPath", {}},
//                                                   {"--mnemonicSize", {}},
//                                                   {"--mnemonic", {}},
//                                                   {"--walletIndex", {}},
//                                                   {"--passphrase", {}}};
//    for (int i = 1; i < argc; i++) {
//        std::string key = std::string(argv[i]);
//        if (!flagsValues.contains(key)) throw InvalidFlag();
//        if (key == "--mnemonic") {
//            if (flagsValues["--mnemonicSize"].empty()) throw MissingImportantFlags();
//            ++i;
//            for (int j = 0; j < std::stoi(flagsValues.at("--mnemonicSize")); ++j) {
//                std::string value = std::string(argv[j+i]);
//                flagsValues["--mnemonic"] += ((j > 0) ? " " : "") + value;
//            }
//            i += std::stoi(flagsValues.at("--mnemonicSize"))-1;
//            continue;
//        }
//        ++i; // as an argument may cause undefined behavior
//        std::string value = std::string(argv[i]);
//        flagsValues[key] = value;
//    }
//    for (auto& [key, value]: flagsValues) {
//        if (value.empty() && key != "--passphrase") throw MissingImportantFlags();
//    }
//    SHA3 sha3 = SHA3(SHA3::Bits256);
//    BIP39Result bip39Result = BIP39Result(flagsValues.at("--mnemonic"), (flagsValues.at("--passphrase").empty()) ?
//        sha3(std::to_string(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count())) :
//            flagsValues.at("--passphrase"));
//    BIP44 bip44;
//    BIP44Result bip44Result = bip44.generateAddress(bip39Result, 0, EXTERNAL_CHANGE, std::stoi((flagsValues.at("--walletIndex"))));
//    unit::list<Shard> shardList{};
//    TransactionPool transactionPool{};
//    std::thread blockTh(blockHandlingHandler, &flagsValues.at("--blockPath"),  &flagsValues.at("--userpath"),
//                        &flagsValues.at("--transactionPath"),  &flagsValues.at("--historyPath"),  &flagsValues.at("--tokenPath"), &bip44Result, &shardList);
//    blockTh.detach();
//    std::thread shardTh(shardInserterHandler, &shardList, &transactionPool, &bip44Result);
//    shardTh.detach();
//    std::thread serverTh(transactionServerHandler, &transactionPool, &flagsValues.at("--userpath"), &flagsValues.at("--historyPath"),
//                         &flagsValues.at("--blockPath"), &flagsValues.at("--transactionPath"));
//    serverTh.detach();
//    while (true) {}
//}

void UnitInitiator::init(int argc, std::string argv[42]) {
    std::cout << "argc count: " << argc << std::endl;
    if (argc == 1) throw NoArgsException();
    std::map<std::string, std::string> flagsValues{{"--userpath", {}},
                                                   {"--historyPath", {}},
                                                   {"--blockPath", {}},
                                                   {"--transactionPath", {}},
                                                   {"--tokenPath", {}},
                                                   {"--mnemonicSize", {}},
                                                   {"--mnemonic", {}},
                                                   {"--walletIndex", {}},
                                                   {"--passphrase", {}}};
    for (int i = 0; i < argc; i++) {
        std::string key = std::string(argv[i]);
        if (!flagsValues.contains(key)) {
            std::cout << "i:" << i << ", key: " << key << std::endl;
            throw InvalidFlag();
        }
        if (key == "--mnemonic") {
            if (flagsValues["--mnemonicSize"].empty()) throw MissingImportantFlags();
            ++i;
            for (int j = 0; j < std::stoi(flagsValues.at("--mnemonicSize")); ++j) {
                std::string value = std::string(argv[j+i]);
                flagsValues["--mnemonic"] += ((j > 0) ? " " : "") + value;
            }
            i += std::stoi(flagsValues.at("--mnemonicSize"))-1;
            continue;
        }
        ++i; // as an argument may cause undefined behavior
        std::string value = std::string(argv[i]);
        flagsValues[key] = value;
    }
    for (auto& [key, value]: flagsValues) {
        if (value.empty() && key != "--passphrase") throw MissingImportantFlags();
    }
    SHA3 sha3 = SHA3(SHA3::Bits256);
    BIP39Result bip39Result = BIP39Result(flagsValues.at("--mnemonic"), (flagsValues.at("--passphrase").empty()) ?
        sha3(std::to_string(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count())) :
            flagsValues.at("--passphrase"));
    BIP44 bip44;
    BIP44Result bip44Result = bip44.generateAddress(bip39Result, 0, EXTERNAL_CHANGE, std::stoi((flagsValues.at("--walletIndex"))));
    unit::list<Shard> shardList{};
    TransactionPool transactionPool{};
    PendingPool pendingPool{};
    Block *previous = nullptr;
    std::thread blockTh(blockHandlingHandler, std::ref(flagsValues.at("--blockPath")),  std::ref(flagsValues.at("--userpath")),
                        std::ref(flagsValues.at("--transactionPath")),  std::ref(flagsValues.at("--historyPath")),  std::ref(flagsValues.at("--tokenPath")), std::ref(bip44Result), &shardList);
    blockTh.detach();
    std::thread shardTh(shardInserterHandler, &shardList, &transactionPool, &bip44Result);
    shardTh.detach();
    std::thread serverTh(transactionServerHandler, &transactionPool, &pendingPool, std::ref(flagsValues.at("--userpath")), std::ref(flagsValues.at("--historyPath")),
                         std::ref(flagsValues.at("--blockPath")), std::ref(flagsValues.at("--transactionPath")));
    serverTh.detach();
    while (true) {}
}

#endif //UNIT_UNITINITIATOR_H
