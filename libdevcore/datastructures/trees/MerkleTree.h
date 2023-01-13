//
// Created by Kirill Zhukov on 27.06.2022.
//

#ifndef UNIT_MERKLETREE_H
#define UNIT_MERKLETREE_H
#include "iostream"
#include "vector"
#include "string"
#include "cmath"
#include "optional"
#include "SHA3/sha3.h"
#include "containers/vector.h"

class MerkleTree {
public:
    MerkleTree();

    explicit MerkleTree(const std::vector<std::string> &initialArray);

    static inline std::string concat(const std::string *psl,
                             const std::string *psr) noexcept {
        if (!(psl || psr)) return "";
        return !psl ? *psr : !psr ? *psl : *psl + *psr;
    }
    void print_tree();
    inline std::string get_root(){
        if (this->tree.empty() || this->initial_array.empty()) {
            std::vector<char> vec(32, '\0');
            std::string s(vec.begin(), vec.end());
            return sha3(sha3(s));
        }
        return (this->initial_array.size() == 1) ? this->sha3(sha3(this->tree[0])) : this->tree[0];
    }

    [[nodiscard]] const std::vector<std::string> *getInitialArray() const;
    void setInitialArray(const std::vector<std::string> &initialArray);

private:
    void buildTree(unsigned long long currentIndex, unsigned long long leftQueryBorder, unsigned long long rightQueryBorder);
    std::vector<std::string> initial_array;
    std::vector<std::string> tree;
    SHA3 sha3 = SHA3(SHA3::Bits256);
};

#endif //UNIT_MERKLETREE_H
