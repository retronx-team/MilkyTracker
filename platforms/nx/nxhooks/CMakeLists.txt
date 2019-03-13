cmake_minimum_required(VERSION 2.6)

project(nxhooks)

include_directories(${PROJECT_SOURCE_DIR}/include)

add_library(dl STATIC src/dlstub.c)

add_library(nxhooks STATIC src/init.c src/sdl/cursor.c src/sdl/input.c)

target_link_libraries(dl nxhooks glad EGL glapi drm_nouveau mpg123 modplug vorbisidec ogg nx)
