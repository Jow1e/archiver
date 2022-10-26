#include "decompress.h"

Decompressor::Decompressor(const std::string_view archive_name) : reader(archive_name) {
}

void Decompressor::Run() {
    while (RunOnFile()) {
    }
}

bool Decompressor::RunOnFile() {
    // read number of unique symbols
    UInt9T alphabet_size = reader.ReadBits(9).ToSymbol_();

    std::vector<Symbol> symbols;
    std::vector<size_t> num_codes_oflen;

    // read unique symbols
    for (size_t i = 0; i < alphabet_size; ++i) {
        symbols.push_back(reader.ReadBits(9).ToSymbol_());
    }

    UInt9T symbols_counter = 0;

    // read sequence of 9-bit words - number of codes with size of 1, 2, ..., max_code_length
    while (symbols_counter != alphabet_size) {
        UInt9T num_symbols = reader.ReadBits(9).ToSymbol_();

        num_codes_oflen.push_back(num_symbols);
        symbols_counter += num_symbols;
    }

    std::unordered_map<Symbol, Code> restored_codes;

    {
        std::reverse(symbols.begin(), symbols.end());

        BitSequence bit_code;

        size_t cur_isym = 0;

        for (size_t code_len = num_codes_oflen.size(); code_len > 0; --code_len) {
            size_t num_codes = num_codes_oflen[code_len - 1];

            bit_code.Resize(code_len);

            for (size_t i = 0; i < num_codes; ++i) {
                Symbol sym = symbols[cur_isym + i];
                restored_codes[sym] = {sym, bit_code};
                AddOne(bit_code);
            }

            cur_isym += num_codes;
        }
    }

    std::shared_ptr<TrieNode> root = TrieNode::BuildTrie(restored_codes);
    std::string file_name;

    while (true) {
        std::shared_ptr<TrieNode> cur_node = root;

        while (!cur_node->terminal) {
            Bit bit = reader.ReadBits(1).ToChar();

            if (bit == 0) {
                cur_node = cur_node->left;
            } else {
                cur_node = cur_node->right;
            }
        }

        Symbol symbol = cur_node->symbol;

        if (symbol == HuffmanTree::ExtraSymbols::FILENAME_END) {
            break;
        }

        file_name.push_back(static_cast<char>(symbol));
    }

    BitWriter writer(file_name);

    while (true) {
        std::shared_ptr<TrieNode> cur_node = root;

        while (!cur_node->terminal) {
            Bit bit = reader.ReadBits(1).ToChar();

            if (bit == 0) {
                cur_node = cur_node->left;
            } else {
                cur_node = cur_node->right;
            }
        }

        Symbol symbol = cur_node->symbol;

        if (symbol == HuffmanTree::ExtraSymbols::ARCHIVE_END) {
            writer.Flush();
            return false;
        }

        if (symbol == HuffmanTree::ExtraSymbols::ONE_MORE_FILE) {
            writer.Flush();
            return true;
        }

        BitSequence symbol_bits;
        unsigned char symbol_ch = symbol;
        symbol_bits.SetFromBytes(&symbol_ch, 8);
        writer.WriteBits(symbol_bits);
    }

    return false;
}

// TrieNode

std::shared_ptr<TrieNode> TrieNode::BuildTrie(const std::unordered_map<Symbol, Code>& codes) {
    std::shared_ptr<TrieNode> root = std::make_shared<TrieNode>();

    for (const auto& [symbol, code] : codes) {
        std::shared_ptr<TrieNode> cur_node = root;
        BitSequence bit_code = code.bit_code;

        for (size_t i = 0; i < bit_code.NumBits(); ++i) {
            Bit bit = bit_code.Get(i);

            if (bit == 0) {
                if (cur_node->left == nullptr) {
                    cur_node->left = std::make_shared<TrieNode>();
                }

                cur_node = cur_node->left;
            } else {
                if (cur_node->right == nullptr) {
                    cur_node->right = std::make_shared<TrieNode>();
                }

                cur_node = cur_node->right;
            }
        }

        cur_node->terminal = true;
        cur_node->symbol = symbol;
    }

    return root;
}
