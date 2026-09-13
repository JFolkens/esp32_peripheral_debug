file(GLOB_RECURSE SRC_FILES "./*.c" "./*.cpp")

file(GLOB_RECURSE
    ASSET_FILES
    CMAKE_CONFIGURE_DEPENDS     # Catch new files without a full clean
    "${CMAKE_CURRENT_LIST_DIR}/*.js"
    "${CMAKE_CURRENT_LIST_DIR}/*.css"
)

# Populate esp32/assets_esp32.h with links to asset files
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
include(GenerateAssetHeader)
generate_asset_header(ASSET_FILES ${ASSET_FILES})

# Register application
idf_component_register(
    SRCS ${SRC_FILES}
    INCLUDE_DIRS "." ${GENERATED_ASSETS_DIR}
    EMBED_TXTFILES ${ASSET_FILES}
)