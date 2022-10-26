#include "bitseq.h"

BitSequence::BitSequence(size_t num_bits) {
    bits.resize(num_bits);
}

BitSequence::BitSequence(const unsigned char* data, size_t num_bits) {
    SetFromBytes(data, num_bits);
}

void BitSequence::Show() const {
    for (bool b : bits) {
        std::cout << b;
    }

    std::cout << std::endl;
}

unsigned char* BitSequence::GetInBytes() const {
    size_t num_bits = bits.size();
    size_t num_bytes = (num_bits + NumBitsInByte - 1) / NumBitsInByte;

    unsigned char* result = new unsigned char[num_bytes]{};

    for (size_t ibit = 0; ibit < num_bits; ++ibit) {
        size_t ibyte = ibit / NumBitsInByte;
        size_t bit_in_byte = ibit % NumBitsInByte;

        result[ibyte] |= Get(ibit) << bit_in_byte;
    }

    return result;
}

void BitSequence::SetFromBytes(const unsigned char* data, size_t num_bits) {
    bits.assign(num_bits, false);

    for (size_t ibit = 0; ibit < num_bits; ++ibit) {
        size_t ibyte = ibit / NumBitsInByte;
        size_t bit_in_byte = ibit % NumBitsInByte;

        Bit bit = (data[ibyte] >> bit_in_byte) & 1;
        Set(ibit, bit);
    }
}

void BitSequence::Reset() {
    bits.assign(bits.size(), false);
}

void BitSequence::Reverse() {
    std::reverse(bits.begin(), bits.end());
}

void BitSequence::Resize(size_t num_bits) {
    bits.resize(num_bits);
}

size_t BitSequence::NumBits() const {
    return bits.size();
}

size_t BitSequence::NumBytes() const {
    return (bits.size() + NumBitsInByte - 1) / NumBitsInByte;
}

unsigned char BitSequence::ToChar() const {
    unsigned char* ptr_symbol = GetInBytes();
    unsigned char result = ptr_symbol[0];

    delete[] ptr_symbol;
    return result;
}

uint16_t BitSequence::ToSymbol_() const {
    unsigned char* ptr_symbol = GetInBytes();

    uint16_t first_byte = ptr_symbol[0];
    uint16_t second_byte = ptr_symbol[1];

    uint16_t result = first_byte + (1 << NumBitsInByte) * second_byte;

    delete[] ptr_symbol;

    return result;
}

void BitSequence::Set(size_t index, Bit bit) {
    bits[index] = bit;
}

Bit BitSequence::Get(size_t index) const {
    return bits[index];
}
