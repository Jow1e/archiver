#include "huffman.h"

// Code

Code::Code(Symbol symbol, BitSequence code) : symbol(symbol), bit_code(code) {
}

BitSequence Code::ConvertSymbolToBits() const {
    return BitSequence(reinterpret_cast<const unsigned char*>(&symbol), 9);
}

size_t Code::Size() const {
    return bit_code.NumBits();
}

// huffman node

HuffmanTree::Node::Node(Symbol symbol, size_t freq, bool is_leaf) : symbol(symbol), freq(freq), is_leaf(is_leaf) {
}

Symbol HuffmanTree::Node::GetMinChild() const {
    Symbol sym0 = (left_child) ? (left_child->symbol) : 0;
    Symbol sym1 = (right_child) ? (right_child->symbol) : 0;
    return std::min(sym0, sym1);
}

// custom std::shared_ptr<Node> comparator

bool HuffmanTree::NodePtrComparator::operator()(const std::shared_ptr<Node> u, const std::shared_ptr<Node> v) const {
    auto uch = u->GetMinChild();
    auto vch = v->GetMinChild();
    return std::tie(u->freq, u->symbol, uch) > std::tie(v->freq, v->symbol, vch);
}

// huffman tree

HuffmanTree::HuffmanTree(const std::string_view file_name) : file_name(file_name) {
}

void HuffmanTree::SetHeapNodes(HQueue& nodes) const {
    BitReader reader(file_name);
    std::unordered_map<Symbol, size_t> freqs;

    for (const char symbol : file_name) {
        ++freqs[symbol];
    }

    ssize_t file_size = reader.NumBytesInFile();

    for (ssize_t index = 0; index < file_size; ++index) {
        unsigned char symbol = reader.ReadBits(8).ToChar();
        ++freqs[symbol];
    }

    freqs[ExtraSymbols::FILENAME_END] = 1;
    freqs[ExtraSymbols::ONE_MORE_FILE] = 1;
    freqs[ExtraSymbols::ARCHIVE_END] = 1;

    for (const auto& [symbol, freq] : freqs) {
        std::shared_ptr<Node> node = std::make_shared<Node>(symbol, freq, true);
        nodes.push(node);
    }
}

void HuffmanTree::SetRoot() {
    HQueue nodes;

    SetHeapNodes(nodes);

    while (nodes.size() > 1) {
        auto u = nodes.top();
        nodes.pop();
        auto v = nodes.top();
        nodes.pop();

        auto freq = u->freq + v->freq;
        std::shared_ptr<Node> node = std::make_shared<Node>('\0', freq, false);

        node->left_child = u;
        node->right_child = v;

        nodes.push(node);
    }

    root = nodes.top();
    nodes.pop();
}

void HuffmanTree::SetCodesLength(const std::shared_ptr<Node> node, Blocks& blocks, size_t num_bits) const {
    if (node->is_leaf) {
        blocks[num_bits].push_back(node->symbol);
        return;
    }

    auto left = node->left_child;
    auto right = node->right_child;

    if (left) {
        SetCodesLength(left, blocks, num_bits + 1);
    }

    if (right) {
        SetCodesLength(right, blocks, num_bits + 1);
    }
}

void AddOne(BitSequence& code) {
    ssize_t i = static_cast<ssize_t>(code.NumBits()) - 1;

    while (i >= 0 && code.Get(i) == 1) {
        code.Set(i, 0);
        --i;
    }

    if (i >= 0) {
        code.Set(i, 1);
    }
}

void HuffmanTree::SetHuffmanCodes(std::vector<Code>& result) {
    Blocks blocks;
    SetCodesLength(root, blocks);

    BitSequence bit_code;

    for (auto& [code_length, symbols] : blocks) {
        std::sort(symbols.begin(), symbols.end());

        bit_code.Resize(code_length);

        for (Symbol symbol : symbols) {
            auto code = Code(symbol, bit_code);
            result.push_back(code);
            AddOne(bit_code);
        }
    }

    std::reverse(result.begin(), result.end());
}

std::vector<Code> HuffmanTree::Build() {
    SetRoot();

    std::vector<Code> codes;

    SetHuffmanCodes(codes);

    return codes;
}
