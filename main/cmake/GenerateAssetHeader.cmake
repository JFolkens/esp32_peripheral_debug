function(generate_asset_header, ASSET_FILES, GENERATED_ASSETS_H)
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
    foreach(ASSET_FILE ${ASSET_FILES})
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
    file(READ GENERATED_ASSETS_H ASSETS_TEMPLATE)
    string(REPLACE
          "/* <----- EXTERN DECLARATION BLOCK -----> */"
          "${EXTERN_DECLARATION_BLOCK}"
          ASSETS_WITH_EXTERN
          ASSETS_TEMPLATE
    )

    string(REPLACE
           "/* <------ EMBEDDED ASSETS BLOCK -----> */"
           "${EMBEDDED_ASSETS_BLOCK}"
           ASSETS_UPDATED
           ASSETS_WITH_EXTERN
    )

    file(WRITE GENERATED_ASSETS_H "${ASSETS_UPDATED}")

endfunction()