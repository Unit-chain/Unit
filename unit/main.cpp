#include <iostream>

#if 0
    #include "libdevcore/bip44/ecdsa.hpp"
    #include "libdevcore/bip44/BIP32.hpp"
    #include "libdevcore/bip44/BIP39.hpp"
    #include "libdevcore/bip44/BIP44.hpp"
    #include "libdevcore/bip44/utils.hpp"
    using namespace std;
#endif

#if 0
    #include "boost/unordered_map.hpp"
    #include "libdevcore/db/DBProvider.h"
    #include "libdevcore/datastructures/account/WalletAccount.h"
    #include "libdevcore/datastructures/blockchain/transaction/ValidTransaction.h"
    #include "libdevcore/datastructures/request/RawTransaction.h"
    #include "libdevcore/utils/StringUtil.h"
    #include "libdevcore/crypto/SHA3/sha3.h"
    #include "libdevcore/pointers/lazy_pointer.h"
#endif

int main() {
    std::cout << "Hello, World!" << std::endl;
    #if 0
        std::string from = "UNTxp28nf2wLJzWm49YtLKyXGYh8vJ3";
        std::string to = "UNTxp28nf2wLJzWm49YtLKyXGYh8vJ3";
        std::string str = "sefesfs";
        std::string str1 = "sefesfs";
        std::string str2 = "sefesfs";
        boost::json::value test = boost::json::parse(R"({"extradata": {
                "name": "token",
                "value": 1e3,
                "bytecode": "null"
            }})");
        RawTransaction rawTransaction = RawTransaction(from, to, (uint64_t) 1, test.at("extradata"), str, str1, str2, 1e3, 1e3, 1000);
        std::cout << *rawTransaction.serializeForValidating() << std::endl;
    #endif
    #if 0
        boost::unordered_map<std::string, std::string> map;
        map["test"] = "tesst";
        for (auto& [key, value]: map) std::cout << key << " has value " << value << std::endl;
    #endif
    #if 0
        std::string testKey = "test0";
        std::string path = "/Users/kirillzhukov/Documents/unit_db/";
        DBWriter blockWriter = DBWriter(path);
        std::vector<std::string> vec;
        vec.reserve(10000);
        for (int i = 0; i < 10000; ++i) vec.emplace_back("test"+std::to_string(i));
        vec.insert(vec.begin(), testKey);
        operationDBStatus::DBTupleResponse tuple = blockWriter.getProvider()->multiRead(&vec);
        std::cout << "size in main: " << (int) tuple.statuses.at(0).code() << std::endl;
    #endif
    #if 0
        std::string key = "test";
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
        std::string path = "/Users/kirillzhukov/Documents/unit_db/";
        DBWriter blockWriter = DBWriter(path);
        std::deque<WriteBatch *> *deque = blockWriter.getDeque();
        std::shared_ptr<rocksdb::WriteBatch> writeBatch = blockWriter.getProvider()->getBatch();
        blockWriter.init();
        for (int i = 0; i < 100000; i++) writeBatch->Put(
                rocksdb::Slice(key+std::to_string(i)),
                rocksdb::Slice(str));
        deque->emplace_back(writeBatch.get());
        std::string testKey = key+"1";
        operationDBStatus::DBResponse<std::string> dbResponse = blockWriter.getProvider()->read<std::string>(&testKey);
        std::cout << std::endl << "reading error: " << (int) dbResponse.errorResponse << std::endl;
        std::cout << std::endl << "reading value: " <<  *dbResponse.value << std::endl;
    #endif
    #if 0
        std::string key = "test";
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
        std::string path = "/Users/kirillzhukov/Documents/unit_db/";
        dbProvider::BatchProvider batchProvider = dbProvider::BatchProvider(path);
        std::shared_ptr<rocksdb::WriteBatch> writeBatch = dbProvider::BatchProvider::getBatch();
        for (int i = 0; i < 100000; i++) writeBatch->Put(rocksdb::Slice(key+std::to_string(i)), rocksdb::Slice(str));
        operationDBStatus::DBCode dbCode = batchProvider.commitBatch(writeBatch);
        std::string testKey = key+"99999";
        std::cout <<  *batchProvider.read<std::string>(&testKey).value << std::endl;
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
    #if 0
        BIP44 bip44;
        BIP44Result r = bip44.generateWallet(PHRASE_24, 0, EXTERNAL_CHANGE);
        cout << "WALLET PATH: " << r.path << endl;
        cout << "WALLET PHRASE: " << r.mnemonic.phrase << endl;
        cout << "WALLET SEED: " << r.mnemonic.seed << endl;
        cout << "WALLET PRV: " << r.prv << endl;
        cout << "WALLET PUB: " << r.pub << endl;
        cout << "WALLET E_PRV: " << r.extended_prv << endl;
        cout << "WALLET E_PUB: " << r.extended_pub << endl;
        cout << "WALLET ADDRESS: " << r.address << endl;

        cout << "\nSigning message 'Hello!': \n";
        ECDSASignResult sig = ecdsa_sign_message("Hello!", r.prv);
        std::cout << "r: " << sig.r << std::endl;
        std::cout << "s: " << sig.s << std::endl;
        std::cout << "signature hash: " << sig.message_hash << std::endl;
        bool verified = ecdsa_verify_signature(sig.r, sig.s, "Hello!", r.address);
        cout << "Result of verification: "<<verified<<"\n";
        BIP44Result r2 = bip44.generateAddress(r.mnemonic, 0, EXTERNAL_CHANGE, 1);
        cout << "\n\nWALLET PATH: " << r2.path << endl;
        cout << "WALLET SEED: " << r2.mnemonic.seed << endl;
        cout << "WALLET E_XPRV: " << r2.extended_prv << endl;
        cout << "WALLET E_XPUB: " << r2.extended_pub << endl;
        cout << "WALLET ADDRESS: " << r2.address << endl;

        cout << "\nSigning message 'Hello!': \n";
        ECDSASignResult sig2 = ecdsa_sign_message("Hello!", r2.prv);
        std::cout << "r: " << sig2.r << std::endl;
        std::cout << "s: " << sig2.s << std::endl;
        bool verified2 = ecdsa_verify_signature(sig2.r, sig2.s, "Hello!", r2.address);
        cout << "Result of verification: "<<verified2<<"\n";
    #endif
    return 0;
}
