#[[
Brief: Helper function for auto-generating the code at esp32/assets_esp32.h

Arguments:
    ASSET_FILES - (multi-valued) List of asset files in project.
]]
function(generate_asset_header)
    # Parse multi value arg for ASSET_FILES
    set(multi_value_args ASSET_FILES)
    cmake_parse_arguments(PARSE_ARGV 0 ARG "" "" "${multi_value_args}")

    set(GENERATED_ASSETS_H "${CMAKE_CURRENT_LIST_DIR}/esp32/assets_esp32.h")

    # Check asset files are found by CMAKE
    # IMPORTANT: File names must be unique, not just paths.
    message(STATUS "ASSET_FILES = ${ARG_ASSET_FILES}")

    #[[
    If your assets are printed by CMAKE but not being linked, try:

    xtensa-esp32-elf-nm -C .\build\esp-idf\main\libmain.a | Select-String "binary"

    ESP-IDF documentation claims the generated symbol will use full path,
    but the `nm` command says only file name is used.
    NOTE: asset file names must be unique.
    ]]

    set(EXTERN_DECLARATION_BLOCK "")
    set(EMBEDDED_ASSETS_BLOCK "")

    # Generate source code blocks
    foreach(ASSET_FILE ${ARG_ASSET_FILES})
        message(STATUS "Parsing asset: ${ASSET_FILE}")
        get_filename_component(ASSET_NAME "${ASSET_FILE}" NAME)
        string(REGEX REPLACE "[^A-Za-z0-9_]" "_" SYMBOL_BASE "${ASSET_NAME}")

        string(APPEND EXTERN_DECLARATION_BLOCK
"extern const unsigned char _binary_${SYMBOL_BASE}_start[];
extern const unsigned char _binary_${SYMBOL_BASE}_end[];
")

        string(APPEND EMBEDDED_ASSETS_BLOCK
"    {\"${ASSET_NAME}\", _binary_${SYMBOL_BASE}_start, _binary_${SYMBOL_BASE}_end},
")
    endforeach()

    # Update header file
    file(READ "${GENERATED_ASSETS_H}" ASSETS_TEMPLATE)
    string(REGEX REPLACE
          "<----- BEGIN EXTERN DECLARATION BLOCK ----->.*// <----- END EXTERN DECLARATION BLOCK ----->"
          "<----- BEGIN EXTERN DECLARATION BLOCK ----->
${EXTERN_DECLARATION_BLOCK}// <----- END EXTERN DECLARATION BLOCK ----->"
          ASSETS_WITH_EXTERN
          "${ASSETS_TEMPLATE}"
    )

    string(REGEX REPLACE
          "<----- BEGIN EMBEDDED ASSETS BLOCK ----->.*// <----- END EMBEDDED ASSETS BLOCK ----->"
          "<----- BEGIN EMBEDDED ASSETS BLOCK ----->
${EMBEDDED_ASSETS_BLOCK}    // <----- END EMBEDDED ASSETS BLOCK ----->"
           ASSETS_UPDATED
           "${ASSETS_WITH_EXTERN}"
    )

    file(WRITE "${GENERATED_ASSETS_H}" "${ASSETS_UPDATED}")

endfunction()