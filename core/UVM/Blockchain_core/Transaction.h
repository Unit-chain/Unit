//
// Created by Kirill Zhukov on 10.06.2022.
//

#ifndef UVM_TRANSACTION_H
#define UVM_TRANSACTION_H
#include "chrono"
#include "rocksdb/db.h"

class Transaction {
public:
    Transaction();
    virtual ~Transaction();
    std::string from;
    std::string to;
    uint64_t type;
    uint64_t date = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    std::map<std::string, std::string> extra_data;
    std::string hash;
    std::string previous_hash;



    [[nodiscard]] const std::string &getFrom() const;

    void setFrom(const std::string &from);

    [[nodiscard]] const std::string &getTo() const;

    void setTo(const std::string &to);

    [[nodiscard]] uint64_t getType() const;

    void setType(uint64_t type);

    [[nodiscard]] uint64_t getDate() const;

    void setDate(uint64_t date);

    [[nodiscard]] const std::map<std::string, std::string> &getExtraData() const;

    void setExtraData(const std::map<std::string, std::string> &extraData);
};


#endif //UVM_TRANSACTION_H
