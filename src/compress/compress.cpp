#include "compress.h"

Compressor::Compressor(const std::vector<std::string_view>& file_names, const std::string_view target_file)
    : writer(target_file), file_names(file_names), target_file(target_file) {
}

void Compressor::Run() {
    size_t size = file_names.size();

    for (size_t index = 0; index < size; ++index) {
        bool is_last_file = (index + 1 == size);
        auto file_name = file_names[index];

        HuffmanTree ht(file_name);
        auto codes = ht.Build();

        WriteCodes(file_name, codes, is_last_file);
    }

    writer.Flush();
}

void Compressor::WriteCodes(std::string_view file_name, std::vector<Code>& codes, bool is_last_file) {
    // write number of unique symbols
    Symbol alphabet_size = static_cast<Symbol>(codes.size());
    BitSequence alphasz_bits;
    alphasz_bits.SetFromBytes(reinterpret_cast<unsigned char*>(&alphabet_size), 9);
    writer.WriteBits(alphasz_bits);

    size_t max_code_size = codes.back().Size();

    std::vector<Symbol> num_codes_lenof(max_code_size);

    // write unique symbols
    // calculate number of code with length
    for (const auto& code : codes) {
        BitSequence symbits = code.ConvertSymbolToBits();
        writer.WriteBits(symbits);

        size_t code_len = code.Size();
        ++num_codes_lenof[code_len - 1];
    }

    for (Symbol num_codes : num_codes_lenof) {
        BitSequence num_codes_in_bits;
        num_codes_in_bits.SetFromBytes(reinterpret_cast<unsigned char*>(&num_codes), 9);
        writer.WriteBits(num_codes_in_bits);
    }

    BitReader reader(file_name);
    std::unordered_map<Symbol, Code> sym2code;

    for (auto& code : codes) {
        sym2code[code.symbol] = code;
    }

    for (char c : file_name) {
        Code code = sym2code[c];
        writer.WriteBits(code.bit_code);
    }

    {
        Symbol file_name_ends_symbol = HuffmanTree::ExtraSymbols::FILENAME_END;
        Code code = sym2code[file_name_ends_symbol];
        writer.WriteBits(code.bit_code);
    }

    ssize_t file_size = reader.NumBytesInFile();

    for (ssize_t index = 0; index < file_size; ++index) {
        unsigned char symbol = reader.ReadBits(8).ToChar();
        Code code = sym2code[symbol];
        writer.WriteBits(code.bit_code);
    }

    {
        Symbol last_sym = HuffmanTree::ExtraSymbols::ONE_MORE_FILE;

        if (is_last_file) {
            last_sym = HuffmanTree::ExtraSymbols::ARCHIVE_END;
        }

        Code code = sym2code[last_sym];
        writer.WriteBits(code.bit_code);
    }
}
