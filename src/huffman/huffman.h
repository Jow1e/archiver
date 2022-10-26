#pragma once

#include <algorithm>
#include <string>
#include <queue>
#include <map>
#include <unordered_map>
#include <memory>

#include "../io/reader/reader.h"

using UInt9T = uint16_t;
using Symbol = UInt9T;

struct Code {
    Symbol symbol;
    BitSequence bit_code;

    Code() {
    }
    Code(Symbol symbol, BitSequence code);
    BitSequence ConvertSymbolToBits() const;
    size_t Size() const;
};

class HuffmanTree {
private:
    struct Node {
        Symbol symbol;
        size_t freq;
        bool is_leaf;

        std::shared_ptr<Node> left_child = nullptr;
        std::shared_ptr<Node> right_child = nullptr;

        Node(Symbol symbol, size_t freq, bool is_leaf);
        Symbol GetMinChild() const;
    };

    struct NodePtrComparator {
        bool operator()(const std::shared_ptr<Node> u, const std::shared_ptr<Node> v) const;
    };

    using HQueue = std::priority_queue<std::shared_ptr<Node>, std::vector<std::shared_ptr<Node>>, NodePtrComparator>;
    using Blocks = std::map<size_t, std::vector<Symbol>, std::greater<size_t>>;

    std::shared_ptr<Node> root;
    const std::string_view file_name;

    void SetHeapNodes(HQueue& nodes) const;
    void SetRoot();
    void SetCodesLength(const std::shared_ptr<Node> node, Blocks& code_blocks, size_t num_bits = 0) const;
    void SetHuffmanCodes(std::vector<Code>& codes);

public:
    enum ExtraSymbols { FILENAME_END = 256, ONE_MORE_FILE = 257, ARCHIVE_END = 258 };

    explicit HuffmanTree(const std::string_view file_name);
    std::vector<Code> Build();
};

void AddOne(BitSequence& code);
