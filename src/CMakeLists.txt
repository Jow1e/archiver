cmake_minimum_required(VERSION 3.16)

project(archiver)

add_executable(
    ${PROJECT_NAME}
    archiver.cpp
    io/reader/reader.cpp
    io/writer/writer.cpp
    huffman/huffman.cpp
    bitseq/bitseq.cpp
    compress/compress.cpp
    decompress/decompress.cpp
)
