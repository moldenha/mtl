cmake_minimum_required(VERSION 3.16)
project(MetalAdd3Example)


# Library definition
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64") # Universal build for Apple Silicon and Intel


# add_library(METAL_CPP
#         ${CMAKE_CURRENT_SOURCE_DIR}/definition.cpp
#         )

find_library(METAL_LIBRARY Metal)
find_library(FOUNDATION_LIBRARY Foundation)
find_library(QUARTZCORE_LIBRARY QuartzCore)

# # Metal cpp library (linker)
# target_link_libraries(METAL_CPP
#         "-framework Metal"
#         "-framework Foundation"
#         "-framework QuartzCore"
#         )

file(GLOB_RECURSE SRC_FILES "src/*cpp")

#output paths
set(SHADERS_DIR "${CMAKE_SOURCE_DIR}/shaders")
set(SHADER_FILE "${SHADERS_DIR}/add3_kernel.metal")
set(AIR_FILE "${CMAKE_BINARY_DIR}/add3_kernel.air")
set(METALLIB_FILE "${CMAKE_BINARY_DIR}/default.metallib")

add_custom_command(
    OUTPUT ${AIR_FILE}
    COMMAND xcrun -sdk macosx metal -c ${SHADER_FILE} -o ${AIR_FILE}
    DEPENDS ${SHADER_FILE}
    COMMENT "Compiling add3_kernel.metal to AIR"
)

add_custom_command(
    OUTPUT ${METALLIB_FILE}
    COMMAND xcrun -sdk macosx metallib ${AIR_FILE} -o ${METALLIB_FILE}
    DEPENDS ${AIR_FILE}
    COMMENT "Linking AIR to metallib"
)

add_custom_target(
    MetalShaders ALL
    DEPENDS ${METALLIB_FILE}
)


# Metal cpp headers
include_directories(PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/../metal-cpp"
        )

# Create executable
add_executable(metal_add3 ${SRC_FILES})

# Add dependency AFTER the executable is defined
add_dependencies(metal_add3 MetalShaders)


target_link_libraries(metal_add3 ${METAL_LIBRARY} ${FOUNDATION_LIBRARY} ${QUARTZCORE_LIBRARY})

target_compile_definitions(metal_add3 PRIVATE METALLIB_PATH="${METALLIB_FILE}")

