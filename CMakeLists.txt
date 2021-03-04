cmake_minimum_required(VERSION 3.0.0)
project(chip8 VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin)

set(SFML_STATIC_LIBRARIES TRUE)
find_package(SFML COMPONENTS audio graphics window system REQUIRED PATHS lib)

file(COPY res DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
file(COPY games DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

add_link_options("-mwindows")

file(GLOB src "src/*.cpp")
add_executable(chip8 ${src})

target_link_libraries(chip8 pthread sfml-audio sfml-graphics sfml-window sfml-system)
file(COPY "lib/SFML/bin/openal32.dll" DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

set(CMAKE_CXX_STANDARD_LIBRARIES "-static-libgcc -static-libstdc++ -lwsock32 -lws2_32 ${CMAKE_CXX_STANDARD_LIBRARIES}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Bstatic,--whole-archive -lwinpthread -Wl,--no-whole-archive")

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
