
#include <gtest/gtest.h>


class FooTest : public testing::TestWithParam<const char*> {
    // You can implement all the usual fixture class members here.
    // To access the test parameter, call GetParam() from class TestWithParam<T>.
};

TEST_P(FooTest, ParametrizedTestsOne) {
    // Inside a test, access the test parameter with the GetParam() method of the TestWithParam<T> class:

    // EXPECT_TRUE(foo.Blah(GetParam()));
    std::cout << GetParam() << std::endl;
}

INSTANTIATE_TEST_SUITE_P(FirstParamsTests,
                         FooTest,
                         testing::Values("One", "Two", "Three"));

//--------------------------------------------------------------------------

// When you want to add parameters to a pre-existing fixture class:
class BaseTest : public testing::Test {

};

class BarTest : public BaseTest,
                public testing::WithParamInterface<const char*> {
};


TEST_P(BarTest, ParametrizedTestsTwo) {
    // Inside a test, access the test parameter with the GetParam() method of the TestWithParam<T> class:

    // EXPECT_TRUE(foo.Blah(GetParam()));
    std::cout << GetParam() << std::endl;
}

INSTANTIATE_TEST_SUITE_P(SecondParamsTests,
                         BarTest,
                         testing::Values("Four", "Five", "Six"));

//-----------------------------------------------------------------------------------
//                              Range tests
//-----------------------------------------------------------------------------------


class MyTestSuite : public testing::TestWithParam<int> {};


TEST_P(MyTestSuite, MyTest)
{
    std::cout << "Example Test Param: " << GetParam() << std::endl;
}

INSTANTIATE_TEST_SUITE_P(MyGroup,
                         MyTestSuite,
                         testing::Range(0, 10),
                         testing::PrintToStringParamName());