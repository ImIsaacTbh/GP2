cmake_minimum_required(VERSION 3.20)

project(Labs LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Source files
set(SOURCES
    Display.cpp
    main.cpp
    MainGame.cpp
    Mesh.cpp
    Shader.cpp
    STB.c
    Texture.cpp
    Transform.cpp
)

add_executable(Labs ${SOURCES})

# Include + library directories
target_include_directories(Labs PRIVATE
    ${CMAKE_SOURCE_DIR}/../deps/include
)

target_link_directories(Labs PRIVATE
    ${CMAKE_SOURCE_DIR}/../deps/lib
)

target_link_libraries(Labs PRIVATE
    SDL2
    SDL2main
    glew32
    opengl32
)

file(COPY Resources DESTINATION ${CMAKE_BINARY_DIR})

if (WIN32)
    target_link_options(Labs PRIVATE "/SUBSYSTEM:CONSOLE")
endif()