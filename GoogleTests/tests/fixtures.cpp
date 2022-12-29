
#include <gtest/gtest.h>


class SkipFixture : public ::testing::Test {
protected:
    void SetUp() override {
        GTEST_SKIP() << "Skipping all tests for this fixture";
    }
};

/** Tests for SkipFixture won't be executed. **/
TEST_F(SkipFixture, SkipsOneTest) {
    EXPECT_EQ(5, 7);  // Won't fail
}

//===============================================================

class TestCaseFixing : public ::testing::Test {
protected:
    void SetUp() override {
        std::cout << "  **** TestCaseFixing::SetUp() ****" << std::endl;
    }

    void TearDown() override {
        std::cout << "  **** TestCaseFixing::TearDown() ****" << std::endl;
    }

    // Per-test-suite set-up.
    // Called before the first test in this test suite.
    // Can be omitted if not needed.
    static void SetUpTestSuite() {
        std::cout << "--- TestCaseFixing::SetUpTestSuite() ---- [PerTestSuite]" << std::endl;
    }

    // Per-test-suite tear-down.
    // Called after the last test in this test suite.
    // Can be omitted if not needed.
    static void TearDownTestSuite() {
        std::cout << "--- TestCaseFixing::TearDownTestSuite() ---- [PerTestSuite]" << std::endl;
    }


    std::string settings; // test environment objects available in each test
};

TEST_F(TestCaseFixing, TestWithFixture_1) {
    std::cout << "      TestWithFixture_1\n";
}

TEST_F(TestCaseFixing, TestWithFixture_2) {
    std::cout << "      TestWithFixture_1\n";
}
