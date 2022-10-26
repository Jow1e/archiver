#include "writer.h"

BitWriter::BitWriter(const std::string_view file_name, size_t capacity_bytes)
    : capacity_bytes(capacity_bytes), buffer(capacity_bytes * NumBitsInByte) {

    Open(file_name);
}

void BitWriter::Open(const std::string_view file_name) {
    out.open(std::string(file_name), std::ios::out | std::ios::binary);
    current_bit = 0;
}

void BitWriter::Close() {
    Flush();
    out.close();
}

void BitWriter::Flush() {
    if (current_bit != 0) {
        unsigned char* tmp = buffer.GetInBytes();

        out.write(reinterpret_cast<char*>(tmp), (current_bit + NumBitsInByte - 1) / NumBitsInByte);

        buffer.Reset();
        delete[] tmp;

        current_bit = 0;
    }
}

void BitWriter::WriteBits(const BitSequence& bits) {
    size_t size = bits.NumBits();
    auto bits_cp = bits;
    bits_cp.Reverse();

    for (size_t i = 0; i < size; ++i) {
        Bit bit = bits_cp.Get(i);
        WriteBit(bit);
    }
}

void BitWriter::WriteBit(Bit bit) {
    if (current_bit == capacity_bytes * NumBitsInByte) {
        Flush();
    }

    buffer.Set(current_bit, bit);
    ++current_bit;
}
