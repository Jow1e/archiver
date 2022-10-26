#pragma once

#include <fstream>
#include <string>
#include <string_view>

#include "../../bitseq/bitseq.h"

class BitReader {
public:
    explicit BitReader(const std::string_view file_name, ssize_t capacity_bytes = 1024);

    void Open(const std::string_view file_name);
    void Close();

    BitSequence ReadBits(size_t num_bits);
    ssize_t NumBytesInFile();

private:
    const ssize_t capacity_bytes;

    std::ifstream in;

    BitSequence buffer;
    size_t size_bytes;
    size_t current_bit;

    void Bufferize();
    Bit ReadBit();
};
