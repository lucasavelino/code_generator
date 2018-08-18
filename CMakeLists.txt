cmake_minimum_required(VERSION 3.10)
project(code_generator)

add_subdirectory(code_generator_tests)


find_package(Boost 1.67 REQUIRED COMPONENTS system-mgw63-mt-x32-1_67 filesystem-mgw63-mt-x32-1_67)


include_directories(${Boost_INCLUDE_DIR})
include_directories(include)

set(HEADER_FILES include/ast.h include/command_runner.h include/cpp_file_generator.h include/msg_types_file_generator.h include/oil_file_generator.h include/parser.h include/replacer.h include/util.h)
set(SOURCE_FILES main.cpp)
add_executable(code_generator ${SOURCE_FILES})
target_link_libraries(code_generator ${Boost_FILESYSTEM_LIBRARY} ${Boost_SYSTEM} ${Boost_LIBRARIES})