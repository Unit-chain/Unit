#include <iostream>
#if 0
    #include "libdevcore/db/DBWriter.h"
    #include "libdevcore/datastructures/account/WalletAccount.h"
    #include "libdevcore/datastructures/blockchain/transaction/ValidTransaction.h"
    #include "libdevcore/datastructures/request/RawTransaction.h"
    #include "libdevcore/utils/StringUtil.h"
    #include "libdevcore/crypto/SHA3/sha3.h"
    #include "libdevcore/pointers/lazy_pointer.h"
#endif

int main() {
    #if 0
        std::cout << "Hello, World!" << std::endl;
        std::string key = "test";
        std::string path = "/Users/kirillzhukov/Documents/unit_db/";
        dbProvider::BatchProvider batchProvider = dbProvider::BatchProvider(path);
        std::shared_ptr<rocksdb::WriteBatch> writeBatch = dbProvider::BatchProvider::getBatch();
        writeBatch->Put(rocksdb::Slice(key), rocksdb::Slice("value"));
        operationDBStatus::DBCode dbCode = batchProvider.commitBatch(writeBatch);
        std::cout <<  *batchProvider.read<std::string>(&key).value << std::endl;
    #endif
    #if 0
        std::string str = R"({"address":"UNTufAafm3iZSjoLhjWq4bjwbb45qgw","balance":111000.0, "tokensBalance":{"token":10.0, "token2":111.1213}})";
        std::string strH = R"({"in":["test", "1312","132132","cdcsdcsdc", "wadawdadwda"], "out":["1312","132132","cdcsdcsdc", "wadawdadwda"]})";
        std::optional<WalletAccount*> anOptional = WalletAccount::parseWallet(&str);
        std::cout << anOptional.value()->serialize() << std::endl;
        WalletAccount *walletAccount = anOptional.value();
        walletAccount->parseHistory(&strH);
        walletAccount->subtractToken(1.9, "wadawda", "token");
        walletAccount->subtract(91111.01121, "wadawda");
        std::cout << walletAccount->tokensBalance << std::endl;
        std::cout << walletAccount->serialize() << std::endl;
        std::cout << walletAccount->serializeHistory() << std::endl;
    #endif
    #if 0
        boost::json::object object = boost::json::parse(R"({"test":{}})").as_object();
        object["test"].as_object()["token"] = 12123.123;
        object["test"].as_object()["token2"] = 123.123;
        object["test"].as_object()["token3"] = 123.123;
        object["test"].as_object()["token3"] = object["test"].as_object()["token3"].as_double() - 10;
        auto str = boost::json::serialize(object);
        std::cout << str << std::endl;
    #endif
    #if 0
        std::string str = "aawd";
        lazy_pointer<std::string> a = lazy_pointer<std::string>(str);
        lazy_pointer<std::string> b = lazy_pointer<std::string>(a);
        lazy_pointer<std::string> c = lazy_pointer<std::string>(b);
        std::cout << "a: " << a.getValue() << std::endl;
        std::cout << "b: " << b.getValue() << std::endl;
        std::cout << "c: " << c.getValue() << std::endl;
        b.destroy_current();
        std::cout << "b.value == nullptr: " << (b.getValue() == nullptr) << ", a.cnt = " << a.getCnt() << std::endl;
        std::cout << "a.cnt: " << c.getCnt() << std::endl;
        std::cout << "a.status: " << a.getStatus() << std::endl;
        c.destroy_current();
        std::cout << "a.destroy: " << a.destroy() << std::endl;
        std::cout << "a.value == nullptr: " << (a.getValue() == nullptr) << std::endl;
    #endif
    #if 0
        std::string str = R"({
          "instruction": "i_push_transaction",
          "data": {
            "from": "g2px1",
            "to": "merchant",
            "amount": 1.000003,
            "type": 0,
            "extradata": {
              "name": "a",
              "value": 1,
              "bytecode":"a"
            },
            "sign":"sadwa",
            "r":"sadwa",
            "s":"sadwa",
            "fee":0.0001
          }
        })";

        RawTransaction *rawTransaction = RawTransaction::parse(&str);
        rawTransaction->generateHash();
        std::cout << *rawTransaction->serializeToJsonTransaction() << std::endl;
    #endif
    #if 0
        std::string test = "Hello, World!";
        std::cout << *StringUtil::encode64(&test) << std::endl;
        double i1 = 222.11; int i2 = 22; int i3 = 33;
        std::string test2 = "[%s]: %s {i1=%f, i2=%d, i3=%d}";
        std::cout << *StringUtil::insertSubElement(&test2, "DEBUG", "Xyz failed", i1, i2, i3) << std::endl;
        std::string test3 = "[%s]: %s, %d";
        std::cout << *StringUtil::insertSubElement(&test3, "DEBUG", "Xyz failed", 1)  << std::endl;
    #endif
    return 0;
}
