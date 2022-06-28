//
// Created by Kirill Zhukov on 27.06.2022.
//

#ifndef UVM_MERKLETREE_H
#define UVM_MERKLETREE_H
#include "iostream"

class MerkleTree {
public:
    MerkleTree();
    static inline std::string concat(const std::string *psl,
                             const std::string *psr) noexcept {
        if (!(psl || psr)) return "";
        return !psl ? *psr : !psr ? *psl : *psl + *psr;
    }
private:

};

#endif //UVM_MERKLETREE_H
