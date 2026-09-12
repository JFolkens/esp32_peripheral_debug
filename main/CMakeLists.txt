file(GLOB_RECURSE MY_SOURCES "./*.c" "./*.cpp")
file(GLOB_RECURSE ASSET_FILES "./*.js" "./*.css")

# Register application
idf_component_register(
    SRCS ${MY_SOURCES}
    INCLUDE_DIRS "." ${GENERATED_ASSETS_DIR}
    EMBED_TXTFILES ASSET_FILES
)