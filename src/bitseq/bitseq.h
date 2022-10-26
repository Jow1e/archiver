#pragma once

#include <cassert>
#include <iostream>
#include <algorithm>

#include <cstddef>
#include <vector>

constexpr uint8_t NumBitsInByte = 8;

using Bit = unsigned char;

class BitSequence {
public:
    BitSequence() {
    }
    explicit BitSequence(size_t num_bits);
    BitSequence(const unsigned char* data, size_t num_bits);

    void Show() const;

    unsigned char ToChar() const;
    uint16_t ToSymbol_() const;
    void SetFromBytes(const unsigned char* data, size_t num_bits);
    unsigned char* GetInBytes() const;

    void Reset();
    void Reverse();
    void Resize(size_t num_bits);

    size_t NumBits() const;
    size_t NumBytes() const;

    void Set(size_t index, Bit bit);
    Bit Get(size_t index) const;

private:
    std::vector<bool> bits;
};
