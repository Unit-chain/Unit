//
// Created by Kirill Zhukov on 10.06.2022.
//

#include "Block.h"
#include "../TXManager.h"

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
    return out << "{\"" << block.prev_hash << "\", " << block.net_version << ", \"" << block.index << "\", " << block.date << ", \"" << all_tx_to_string <<"\"}";
}

Block::~Block() {}

std::string Block::to_string() {
    std::ostringstream string_stream;
    string_stream << *this;
    return string_stream.str();
}

void Block::generate_hash() {
    std::string block_string = this->to_string();
    std::string first_hash = kec256::getHash(block_string, block_string.length());
    this->hash = kec256::getHash(first_hash, first_hash.length());
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
    net_version = netVersion;
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


