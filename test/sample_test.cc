
#include "include/common/sample.h"

#include <gtest/gtest.h>

TEST(sample_test, add_test) {
  printf("-----------test start------------\n");
  auto ret = common::add(1, 1);
  EXPECT_EQ(2, ret);
  EXPECT_EQ(4, common::add(1, 3));
  EXPECT_EQ(0, common::add(5, -5));

}