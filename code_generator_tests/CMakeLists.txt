project(code_generator_tests)

#set(CMAKE_CXX_FLAGS "-fprofile-arcs -ftest-coverage -fPIC -O0")
#set(CMAKE_CXX_OUTPUT_EXTENSION_REPLACE 1)

set(CMAKE_CXX_STANDARD 17)
set(BOOST_ROOT C:\\boost)
set(BOOST_INCLUDEDIR C:\\boost\\include\\boost-1_67)
set(BOOST_LIBRARYDIR C:\\boost\\lib)

find_package(Boost 1.67 REQUIRED COMPONENTS system-mgw63-mt-x32-1_67 filesystem-mgw63-mt-x32-1_67)

add_subdirectory(lib/googletest)
include_directories(lib/googletest/googletest/include)
include_directories(lib/googletest/googlemock/include)
include_directories(${Boost_INCLUDE_DIR})
include_directories(../include)

set(HEADER_FILES ../include/ast.h ../include/command_runner.h ../include/cpp_file_generator.h ../include/msg_types_file_generator.h ../include/oil_file_generator.h ../include/parser.h ../include/replacer.h ../include/util.h)
set(SOURCE_FILES main_test.cpp util_test.cpp)
add_executable(code_generator_tests ${HEADER_FILES} ${SOURCE_FILES})

target_link_libraries(code_generator_tests gtest gtest_main ${Boost_LIBRARIES})