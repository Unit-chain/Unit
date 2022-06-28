//
// Created by Kirill Zhukov on 10.06.2022.
//

#include "Block.h"

//std::string Block::previous_block_num() {
//    rocksdb::Status s = TXManager::openDB_RO();
//    rocksdb::DB* db;
//    rocksdb::Options options;
//    options.create_if_missing = true;
//    rocksdb::Status status = rocksdb::DB::OpenForReadOnly(options, kDBPath, &db);
//    if(status.ok()) ;
//    return std::string();
//}

std::ostream &operator<<(std::ostream &out, const Block &block) {
    std::string all_tx_to_string;
    for (Transaction tx : block.transactions) {
        if (tx == block.transactions[block.transactions.size()-1]) {
            all_tx_to_string.append(tx.to_string());
            break;
        }
        all_tx_to_string.append(tx.to_string()).append(", ");
    }
    return out << "{\"" << block.prev_hash << "\", " << block.net_version << ", \"" << block.index << "\", " << block.date << ", [" << all_tx_to_string <<"]}";
}

Block::~Block() = default;

std::string Block::to_string() {
    std::ostringstream string_stream;
    string_stream << *this;
    return string_stream.str();
}

void Block::generate_hash() {
    if (this->transactions.empty()) {
        std::ostringstream string_stream;
        string_stream << R"({"index":)" << this->index << R"({, "prev_hash":")" << this->prev_hash << R"({", "timestamp":)" << this->date << "}";
        std::string doubled_hash = sha3(sha3(string_stream.str()));
        this->hash = doubled_hash;
        return;
    }

    std::vector<std::string> leafs(this->transactions.size());
    for (const Transaction& tx : this->transactions) {
        leafs.emplace_back(tx.hash);
    }

    MerkleTree merkleTree = MerkleTree(leafs);
    this->hash = merkleTree.get_root().value(); // it's guaranteed here that tree has root because transaction's vector is always !empty.
}

uint64_t Block::getDate() const {
    return date;
}

void Block::setDate(uint64_t date) {
    Block::date = date;
}

uint64_t Block::getIndex() const {
    return index;
}

void Block::setIndex(uint64_t index) {
    Block::index = index;
}

uint16_t Block::getNetVersion() const {
    return net_version;
}

void Block::setNetVersion(uint16_t netVersion) {
   this->net_version = netVersion;
}

const std::string &Block::getHash() const {
    return hash;
}

void Block::setHash(const std::string &hash) {
    Block::hash = hash;
}

const std::string &Block::getPrevHash() const {
    return prev_hash;
}

void Block::setPrevHash(const std::string &prevHash) {
    prev_hash = prevHash;
}

const std::vector<Transaction> &Block::getTransactions() const {
    return transactions;
}

void Block::setTransactions(const std::vector<Transaction> &transactions) {
    Block::transactions = transactions;
}

void Block::push_tx(Transaction &tx) {
    this->transactions.emplace_back(tx);
}

Block::Block(uint64_t index, uint16_t netVersion, const std::string &prevHash,
             const std::vector<Transaction> &transactions) : index(index), net_version(netVersion), prev_hash(prevHash),
                                                             transactions(transactions) {
    this->index = index;
    this->net_version = netVersion;
    this->prev_hash = prevHash;
    this->transactions = transactions;
}

Block::Block(uint64_t index, uint16_t netVersion, const std::string &prevHash) : index(index), net_version(netVersion),
                                                                                 prev_hash(prevHash) {
    this->index = index;
    this->net_version = netVersion;
    this->prev_hash = prevHash;
}

void Block::set_current_date() {
    this->date = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

Block::Block(uint64_t date, uint64_t index, uint16_t netVersion, const std::string &prevHash) : date(date),
                                                                                                index(index),
                                                                                                net_version(netVersion),
                                                                                                prev_hash(prevHash) {
    this->date = date;
    this->index = index;
    this->net_version = netVersion;
    this->prev_hash = prevHash;
}

Block::Block(uint64_t date, uint64_t index, uint16_t netVersion, const std::string &prevHash,
             const std::vector<Transaction> &transactions) : date(date), index(index), net_version(netVersion),
                                                             prev_hash(prevHash), transactions(transactions) {
    this->date = date;
    this->index = index;
    this->net_version = netVersion;
    this->prev_hash = prevHash;
    this->transactions = transactions;
}

Block::Block(uint64_t date, uint64_t index, uint16_t netVersion, const std::string &hash, const std::string &prevHash,
             const std::vector<Transaction> &transactions) : date(date), index(index), net_version(netVersion),
                                                             hash(hash), prev_hash(prevHash),
                                                             transactions(transactions) {
    this->date = date;
    this->index = index;
    this->net_version = netVersion;
    this->hash = hash;
    this->prev_hash = prevHash;
    this->transactions = transactions;
}

std::string Block::to_json_string() {
    std::string all_tx_to_json_string;
    for (Transaction tx : this->transactions) {
        if (tx == this->transactions[this->transactions.size()-1]) {
            all_tx_to_json_string.append(tx.to_json_string());
            break;
        }
        all_tx_to_json_string.append(tx.to_json_string()).append(", ");
    }
    std::ostringstream string_stream;
    string_stream << R"({"hash":")" << this->hash << R"(", "prev_hash":")" << this->prev_hash << R"(", "net_version":")" << this->net_version << R"(", "index":)" << this->index << R"(, "date":)" << this->date << R"(, "transactions": [)" << all_tx_to_json_string <<"]}";
    return string_stream.str();
}

std::string Block::to_json_with_tx_hash_only() {
    std::string all_tx_to_json_string;
    for (Transaction tx : this->transactions) {
        if (tx == this->transactions[this->transactions.size()-1]) {
            all_tx_to_json_string.append("\"").append(tx.hash).append("\"");
            break;
        }
        all_tx_to_json_string.append("\"").append(tx.hash).append("\", ");
    }
    std::ostringstream string_stream;
    string_stream << R"({"hash":")" << this->hash << R"(", "prev_hash":")" << this->prev_hash << R"(", "net_version":")" << this->net_version << R"(", "index":)" << this->index << R"(, "date":)" << this->date << R"(, "transactions": [)" << all_tx_to_json_string <<"]}";
    return string_stream.str();
}

Block::Block(uint16_t netVersion) : net_version(netVersion) {
    this->net_version = netVersion;
}


