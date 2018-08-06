cmake_minimum_required(VERSION 3.10)
project(code_generator)

set(CMAKE_CXX_STANDARD 17)
set(BOOST_ROOT C:\\boost)
set(BOOST_INCLUDEDIR C:\\boost\\include\\boost-1_67)
set(BOOST_LIBRARYDIR C:\\boost\\lib)

find_package(Boost 1.67 REQUIRED COMPONENTS system-mgw63-mt-x32-1_67 filesystem-mgw63-mt-x32-1_67)
include_directories(${Boost_INCLUDE_DIR})
set(HEADER_FILES ast.h parser.h util.h replacer.h oil_file_generator.h msg_types_file_generator.h cpp_file_generator.h command_runner.h)
set(SOURCE_FILES main.cpp)
add_executable(code_generator ${SOURCE_FILES} ${HEADER_FILES})
target_link_libraries(code_generator ${Boost_FILESYSTEM_LIBRARY} ${Boost_SYSTEM} ${Boost_LIBRARIES})