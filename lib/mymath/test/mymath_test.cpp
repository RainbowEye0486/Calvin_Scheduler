# include <gtest/gtest.h>
# include "mymath/mymath.h"

using namespace mymath;


TEST (myMathTest, add) {

    EXPECT_EQ(myadd(2, 3), 5);
}

TEST (myMathTest, sub) {
    EXPECT_EQ(mysub(2, 3), -1);
}

TEST (myMathTest, sqrt) {
    EXPECT_EQ(mysqrt(2), 1.41421);
}