cmake_minimum_required(VERSION 3.10)
project(ascii_art C)

set(CMAKE_C_STANDARD 11)

add_executable(ascii_art main.c stb_image.h stb_image_resize.h)

target_link_libraries(ascii_art m)
