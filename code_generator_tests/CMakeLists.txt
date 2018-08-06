project(code_generator_tests)

set(CODE_GENERATOR_HEADERS_DIR ../)

add_subdirectory(lib/googletest)
include_directories(lib/googletest/googletest/include)
include_directories(lib/googletest/googlemock/include)
include_directories(${CODE_GENERATOR_HEADERS_DIR})

add_executable(code_generator_tests test.cpp util.cpp)

target_link_libraries(code_generator_tests gtest gtest_main)