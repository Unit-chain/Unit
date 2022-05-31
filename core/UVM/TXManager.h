//
// Created by Kirill Zhukov on 31.05.2022.
//

#ifndef UVM_TXMANAGER_H
#define UVM_TXMANAGER_H
#include <cassert>
#include "string"
#include "rocksdb/db.h"

class TXManager {
public:
    TXManager();
    virtual ~TXManager();
    void pushTXToDB();
};


#endif //UVM_TXMANAGER_H
