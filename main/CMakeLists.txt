file(GLOB_RECURSE MY_SOURCES "./*.c" "./*.cpp")

idf_component_register(
    SRCS ${MY_SOURCES}
    INCLUDE_DIRS "."
)