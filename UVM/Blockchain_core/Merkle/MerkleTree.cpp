//
// Created by Kirill Zhukov on 27.06.2022.
//

#include "MerkleTree.h"

MerkleTree::MerkleTree() = default;

void MerkleTree::buildTree(unsigned long long int currentIndex, unsigned long long int leftQueryBorder,
                           unsigned long long int rightQueryBorder) {
    if (leftQueryBorder == rightQueryBorder - 1) {
        if (leftQueryBorder < rightQueryBorder)
            tree[currentIndex] = initial_array[leftQueryBorder];
        return;
    } else {
        long long half = floor((rightQueryBorder + leftQueryBorder) / 2);
        buildTree(2 * currentIndex + 1, leftQueryBorder, half);
        buildTree(2 * currentIndex + 2, half, rightQueryBorder);
        std::string hash_a = "0x" + this->sha3(tree[2 * currentIndex + 1]);
        std::string hash_b = "0x" + this->sha3(tree[2 * currentIndex + 2]);
        tree[currentIndex] = "0x" + this->sha3(concat(&hash_a, &hash_b));
    }
}

MerkleTree::MerkleTree(const std::vector<std::string> &initialArray) : initial_array(initialArray) {
    this->initial_array = initialArray;
    this->tree = std::vector<std::string>((initialArray.size() * 8)); //, "0"
    this->buildTree(0, 0, initialArray.size());
}

void MerkleTree::print_tree() {
    for (const auto& str : this->tree) {
        std::cout << str << std::endl;
    }
}



const std::vector<std::string> &MerkleTree::getInitialArray() const {
    return initial_array;
}

void MerkleTree::setInitialArray(const std::vector<std::string> &initialArray) {
    initial_array = initialArray;
}
