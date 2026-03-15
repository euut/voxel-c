cmake_minimum_required(VERSION 3.28)

project(minecraft-c)

set(CMAKE_C_STANDARD 11)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(lib/glfw)
add_subdirectory(lib/cglm)

add_library(glad STATIC lib/glad/src/glad.c)

add_executable(${PROJECT_NAME}
    src/main.c
    src/game.c
    src/window.c
    src/renderer.c
    src/shader.c
    src/texture.c
    src/camera.c
    src/input.c
    src/mesh.c
    src/block.c
    src/chunk.c
    src/chunkmap.c
    src/world.c
)

find_package(OpenGL REQUIRED)

target_include_directories(${PROJECT_NAME} PRIVATE src)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/lib/stb)
target_include_directories(glad PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/lib/glad/include)

target_link_libraries(${PROJECT_NAME} PRIVATE glfw glad cglm OpenGL::GL)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/res
    $<TARGET_FILE_DIR:${PROJECT_NAME}>/res
)