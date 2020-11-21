#include <gtest/gtest.h>

#include "lexerTests.cpp"
#include "astTests.cpp"
#include "generateCodeTests.cpp"

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv); 
    return RUN_ALL_TESTS();
}