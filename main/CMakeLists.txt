file(GLOB_RECURSE MY_SOURCES "./*.c" "./*.cpp")

# Create directory for generated header files
set(GENERATED_ASSETS_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated_assets")
file(MAKE_DIRECTORY ${GENERATED_ASSETS_DIR})

# Asset files to embed
set(ASSET_FILES
    "web/assets/led_control.js"
    "web/assets/motor_control.js"
    "web/assets/pwm_control.js"
    "web/assets/styles.css"
)

set(ASSET_NAMES
    "led_control_js"
    "motor_control_js"
    "pwm_control_js"
    "styles_css"
)

# Process each asset file
list(LENGTH ASSET_FILES num_assets)
math(EXPR num_assets "${num_assets} - 1")

foreach(idx RANGE ${num_assets})
    list(GET ASSET_FILES ${idx} asset_file)
    list(GET ASSET_NAMES ${idx} asset_name)

    set(input_path "${CMAKE_CURRENT_SOURCE_DIR}/${asset_file}")
    set(output_path "${GENERATED_ASSETS_DIR}/${asset_name}.h")

    if(EXISTS ${input_path})
        file(READ ${input_path} file_contents)

        # Escape for CMake string
        string(REPLACE "\\" "\\\\" file_contents "${file_contents}")
        string(REPLACE "\"" "\\\"" file_contents "${file_contents}")

        # Generate header
        file(WRITE ${output_path}
            "#pragma once\n"
            "#include <string_view>\n"
            "\n"
            "namespace rover::web::assets {\n"
            "\n"
            "constexpr std::string_view get_${asset_name}() {\n"
            "    return std::string_view(R\"EMBEDDED_ASSET(\n"
            "${file_contents}\n"
            ")EMBEDDED_ASSET\");\n"
            "}\n"
            "\n"
            "}  // namespace rover::web::assets\n"
        )
        message(STATUS "Generated asset header: ${output_path}")
    else()
        message(WARNING "Asset file not found: ${input_path}")
    endif()
endforeach()

idf_component_register(
    SRCS ${MY_SOURCES}
    INCLUDE_DIRS "." ${GENERATED_ASSETS_DIR}
)