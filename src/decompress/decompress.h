#pragma once

#include "../io/reader/reader.h"
#include "../io/writer/writer.h"
#include "../huffman/huffman.h"

class Decompressor {
public:
    explicit Decompressor(const std::string_view archive_name);

    void Run();

private:
    BitReader reader;

    bool RunOnFile();
};

struct TrieNode {
    static std::shared_ptr<TrieNode> BuildTrie(const std::unordered_map<Symbol, Code>& codes);

    std::shared_ptr<TrieNode> left = nullptr;
    std::shared_ptr<TrieNode> right = nullptr;

    bool terminal = false;
    Symbol symbol = '\0';
};
