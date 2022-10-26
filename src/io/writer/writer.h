#pragma once

#include <fstream>
#include <string>
#include <string_view>

#include "../../bitseq/bitseq.h"

class BitWriter {
public:
    explicit BitWriter(const std::string_view file_name, size_t capacity_bytes = 1);

    void Open(const std::string_view file_name);
    void Close();

    void WriteBits(const BitSequence& bits);
    void Flush();

private:
    const size_t capacity_bytes;

    std::ofstream out;

    BitSequence buffer;
    ssize_t current_bit;

    void WriteBit(Bit bit);
};
