GOOGLETEST_DIR = code_generator_tests/lib/googletest
GTEST_SRCDIR = $$GOOGLETEST_DIR/googletest
GMOCK_SRCDIR = $$GOOGLETEST_DIR/googlemock

test {
    INCLUDEPATH *= $$GTEST_SRCDIR \
                   $$GTEST_SRCDIR/include \
                   $$GMOCK_SRCDIR \
                   $$GMOCK_SRCDIR/include

    SOURCES += $$GTEST_SRCDIR/src/gtest-all.cc \
               $$GMOCK_SRCDIR/src/gmock-all.cc
}
