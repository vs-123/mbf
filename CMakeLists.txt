cmake_minimum_required(VERSION 3.10)

project(mbf)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(SRCS
   src/bf.c
   src/macromiser.c
   src/mbf.c
   src/tokeniser.c
   src/util.c
)

set(IDIRS
   ${CMAKE_SOURCE_DIR}/include 
   ${CMAKE_SOURCE_DIR}/vendors/dstr
)

add_executable(${PROJECT_NAME} ${SRCS} src/main.c)
add_executable(${PROJECT_NAME}_test ${SRCS} src/test.c)

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic -std=c99)
target_include_directories(${PROJECT_NAME} PRIVATE ${IDIRS})
target_include_directories(${PROJECT_NAME}_test PRIVATE ${IDIRS})
