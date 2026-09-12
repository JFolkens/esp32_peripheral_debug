# This CMake script converts binary files (JS, CSS) to C++ header files
# Usage: cmake -P embed_assets.cmake

function(embed_asset INPUT_FILE OUTPUT_FILE VAR_NAME)
    # Read the input file as binary
    file(READ ${INPUT_FILE} FILE_CONTENTS HEX)

    # Get the file size
    file(SIZE ${INPUT_FILE} FILE_SIZE)

    # Generate the header file
    file(WRITE ${OUTPUT_FILE}
        "#pragma once\n\n"
        "#include <cstddef>\n"
        "#include <cstdint>\n\n"
        "namespace rover::web::assets {\n\n"
        "constexpr const char ${VAR_NAME}[] = {\n"
    )

    # Convert hex to individual bytes and write to file
    string(LENGTH ${FILE_CONTENTS} HEX_LENGTH)
    math(EXPR BYTE_COUNT "${FILE_SIZE}")

    set(i 0)
    set(line_count 0)
    while(i LESS HEX_LENGTH)
        string(SUBSTRING ${FILE_CONTENTS} ${i} 2 HEX_BYTE)

        if(line_count EQUAL 0)
            file(APPEND ${OUTPUT_FILE} "    ")
        endif()

        file(APPEND ${OUTPUT_FILE} "0x${HEX_BYTE}, ")

        math(EXPR i "${i} + 2")
        math(EXPR line_count "${line_count} + 1")

        if(line_count EQUAL 16)
            file(APPEND ${OUTPUT_FILE} "\n")
            set(line_count 0)
        endif()
    endwhile()

    # Add null terminator for string data
    file(APPEND ${OUTPUT_FILE}
        "\n    0x00  // Null terminator\n"
        "};\n\n"
        "constexpr size_t ${VAR_NAME}_size = ${FILE_SIZE};\n\n"
        "}  // namespace rover::web::assets\n"
    )
endfunction()

# Get input and output file paths from command line
if(NOT DEFINED INPUT_FILE OR NOT DEFINED OUTPUT_FILE OR NOT DEFINED VAR_NAME)
    message(FATAL_ERROR "Usage: cmake -P embed_assets.cmake -DINPUT_FILE=<file> -DOUTPUT_FILE=<file> -DVAR_NAME=<name>")
endif()

embed_asset(${INPUT_FILE} ${OUTPUT_FILE} ${VAR_NAME})
message(STATUS "Generated ${OUTPUT_FILE} from ${INPUT_FILE}")
