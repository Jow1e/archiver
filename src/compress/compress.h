#pragma once

#include <string_view>

#include "../huffman/huffman.h"
#include "../io/writer/writer.h"

class Compressor {
public:
    Compressor(const std::vector<std::string_view>& file_names, const std::string_view target_file);

    void Run();

private:
    BitWriter writer;
    std::vector<std::string_view> file_names;
    std::string_view target_file;

    void WriteCodes(std::string_view file_name, std::vector<Code>& codes, bool is_last_file);
};
