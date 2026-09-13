
file(GLOB_RECURSE SRC_FILES "./*.c" "./*.cpp")

file(GLOB_RECURSE
    ASSET_FILES
    CMAKE_CONFIGURE_DEPENDS     # Catch new files without a full clean
    "${CMAKE_CURRENT_LIST_DIR}/*.js"
    "${CMAKE_CURRENT_LIST_DIR}/*.css"
)

# Check asset files are found by CMAKE
# IMPORTANT: File names must be unique, not just paths.
message(STATUS "ASSET_FILES = ${ASSET_FILES}")

#[[
If your assets are printed by CMAKE but not being linked, try:

xtensa-esp32-elf-nm -C .\build\esp-idf\main\libmain.a | Select-String "binary"

ESP-IDF documentation claims the generated symbol will use full path,
but the `nm` command says only file name is used.
NOTE: asset file names must be unique.
]]
#[[
set(GENERATED_WEB_ASSETS_H "${CMAKE_CURRENT_BINARY_DIR}/esp32/web_assets_esp32.h")
foreach(ASSET_FILE ${ASSET_FILES})
    # Form the linked symbol name. File name, with special characters as "_"
    get_filename_component(ASSET_NAME "${ASSET_FILE}" NAME)
    string(REGEX REPLACE "[^A-Za-z0-9_]" "_" SYMBOL_BASE "${ASSET_NAME}")

    # Write out header file table
    file(APPEND "${GENERATED_WEB_ASSETS_H}"
"extern const unsigned char _binary_${SYMBOL_BASE}_start[];
extern const unsigned char _binary_${SYMBOL_BASE}_end[];
"
    )
endforeach()
]]

# Register application
idf_component_register(
    SRCS ${SRC_FILES}
    INCLUDE_DIRS "." ${GENERATED_ASSETS_DIR}
    EMBED_TXTFILES ${ASSET_FILES}
)