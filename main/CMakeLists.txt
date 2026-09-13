file(GLOB_RECURSE SOURCE_FILES "./*.c" "./*.cpp")
file(GLOB_RECURSE ASSET_FILES "${CMAKE_CURRENT_LIST_DIR}/*.js" "${CMAKE_CURRENT_LIST_DIR}/*.css")

# Check that files at web/assets are actually located
message(STATUS "ASSET_FILES = ${ASSET_FILES}")

# Register application
idf_component_register(
    SRCS ${SOURCE_FILES}
    INCLUDE_DIRS "." ${GENERATED_ASSETS_DIR}
    EMBED_TXTFILES ${ASSET_FILES}
)