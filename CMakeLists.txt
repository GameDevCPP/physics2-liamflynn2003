cmake_minimum_required(VERSION 3.21)
project(physics2-liamflynn2003 LANGUAGES CXX)

set(SFML_DIR "C:/SFML-2.6.1/lib/cmake/SFML")
find_package(SFML 2.6.1 COMPONENTS system window graphics network audio REQUIRED)

add_subdirectory("lib/b2d/Box2D")
set(B2D_INCS "lib/b2d/Box2D/")

add_executable(physics2-liamflynn2003 src/main.cpp
        src/main.cpp)

target_include_directories(physics2-liamflynn2003 PRIVATE ${B2D_INCS} ${SFML_INCLUDE_DIRS})

target_link_libraries(physics2-liamflynn2003 PRIVATE sfml-graphics Box2D)
target_compile_features(physics2-liamflynn2003 PRIVATE cxx_std_17)

if (WIN32 AND BUILD_SHARED_LIBS)
    add_custom_command(TARGET physics2-liamflynn2003 POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:physics2-liamflynn2003> $<TARGET_FILE_DIR:physics2-liamflynn2003> COMMAND_EXPAND_LISTS)
endif()
# Handle DLL copying on Windows
if (WIN32)
    set(SFML_DLL_PATH "C:/SFML-2.6.1/bin")

    add_custom_command(TARGET physics2-liamflynn2003 POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${SFML_DLL_PATH}/sfml-graphics-d-2.dll"
            "${SFML_DLL_PATH}/sfml-window-d-2.dll"
            "${SFML_DLL_PATH}/sfml-system-d-2.dll"
            "${SFML_DLL_PATH}/sfml-audio-d-2.dll"
            "${CMAKE_CURRENT_BINARY_DIR}"
            COMMAND_EXPAND_LISTS)
endif()

# Installation
install(TARGETS physics2-liamflynn2003)
