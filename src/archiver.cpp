#include "bitseq/bitseq.h"
#include "huffman/huffman.h"
#include "compress/compress.h"
#include "decompress/decompress.h"
#include "io/reader/reader.h"
#include "io/writer/writer.h"

#include <stdexcept>
#include <string>
#include <vector>
#include <iostream>
#include <tuple>

std::vector<std::string_view> FilesForCompression(const std::vector<std::string_view>& args) {
    std::vector<std::string_view> result;

    size_t index = 0;

    while (index < args.size() && args[index] != "-c") {
        ++index;
    }

    ++index;

    while (index < args.size() && args[index] != "-d" && args[index] != "-h" && args[index] != "-c") {
        result.push_back(args[index]);
        ++index;
    }

    return result;
}

std::string_view FileForDecompression(const std::vector<std::string_view>& args) {
    size_t index = 0;

    while (index < args.size() && args[index] != "-d") {
        ++index;
    }

    ++index;

    if (index == args.size()) {
        throw std::runtime_error("Wrong input arguments");
    }

    if (index == args.size() + 1) {
        return "";
    }

    return args[index];
}

void PrintHelp() {
    std::string message =
        "Archiver\n"
        "* `archiver -c archive_name file1 [file2 ...]` - заархивировать файлы `file1, file2, ...` и сохранить "
        "результат в файл `archive_name`.\n"
        "* `archiver -d archive_name` - разархивировать файлы из архива `archive_name` и положить в текущую "
        "директорию.\n"
        "* `archiver -h` - вывести справку по использованию программы.";

    std::cout << message << std::endl;
}

void MaybePrintHelp(const std::vector<std::string_view>& args) {
    for (auto str : args) {
        if (str == "-h") {
            PrintHelp();
            break;
        }
    }
}

int main(int argc, char** argv) {
    std::vector<std::string_view> args(argc);

    for (int i = 0; i < argc; ++i) {
        args[i] = argv[i];
    }

    auto for_compr = FilesForCompression(args);
    std::string_view for_decompr = FileForDecompression(args);
    MaybePrintHelp(args);

    if (for_decompr.size() == 1) {
        throw std::runtime_error("Wrong input arguments");
    }

    if (!for_compr.empty()) {
        std::string_view archive_name = for_compr[0];
        std::vector<std::string_view> file_names =
            std::vector<std::string_view>(for_compr.begin() + 1, for_compr.end());
        Compressor cmpr(file_names, archive_name);
        cmpr.Run();
    }

    if (!for_decompr.empty()) {
        Decompressor decmpr(for_decompr);
        decmpr.Run();
    }
}
