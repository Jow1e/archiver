#include "reader.h"
#include <exception>
#include <stdexcept>

BitReader::BitReader(const std::string_view file_name, ssize_t capacity_bytes)
    : capacity_bytes(capacity_bytes), buffer(capacity_bytes * NumBitsInByte) {

    Open(file_name);
}

void BitReader::Open(const std::string_view file_name) {
    in.open(std::string(file_name), std::ios::in | std::ios::binary);

    if (!in) {
        throw std::runtime_error("Can't open file");
    }

    size_bytes = 0;
    current_bit = 0;
}

void BitReader::Close() {
    size_bytes = 0;
    current_bit = 0;
    in.close();
}

void BitReader::Bufferize() {
    unsigned char* tmp = new unsigned char[capacity_bytes]{};
    in.read(reinterpret_cast<char*>(tmp), capacity_bytes);

    buffer.SetFromBytes(tmp, capacity_bytes * NumBitsInByte);
    delete[] tmp;

    size_bytes = in.gcount();
    current_bit = 0;
}

Bit BitReader::ReadBit() {
    if (current_bit == size_bytes * NumBitsInByte) {
        Bufferize();
    }

    Bit result = buffer.Get(current_bit);
    ++current_bit;

    return result;
}

BitSequence BitReader::ReadBits(size_t num_bits) {
    BitSequence result(num_bits);

    for (size_t i = 0; i < num_bits; ++i) {
        Bit bit = ReadBit();
        result.Set(i, bit);
    }

    return result;
}

ssize_t BitReader::NumBytesInFile() {
    in.seekg(0, std::ios::end);
    ssize_t file_size = in.tellg();
    in.seekg(0, std::ios::beg);

    return file_size;
}
