
#include "common/string_utils.h"
#include <gtest/gtest.h>

TEST(common, component_string) {
  printf("-----------test start------------\n");

  std::string in = "aa,bb,cc,ddddd,,eee\n,ffff&%,";

  std::vector<std::string> strs;

  {
    gomros::common::StringUtils::SplitString(in, strs, ',');

    for (int i = 0; i < strs.size(); i++) {
      std::cout << i << " [" << strs[i] << "]"
                << "\n";
    }
    std::cout << std::endl;
  }

  {
    std::cout << "keep middle empty str" << std::endl;
    gomros::common::StringUtils::SplitString(in, strs, ',', true);

    for (int i = 0; i < strs.size(); i++) {
      std::cout << i << " [" << strs[i] << "]"
                << "\n";
    }
    std::cout << std::endl;
  }

  {
    std::cout << "keep mid & end empty str" << std::endl;
    gomros::common::StringUtils::SplitString(in, strs, ',', true, true);

    for (int i = 0; i < strs.size(); i++) {
      std::cout << i << " [" << strs[i] << "]"
                << "\n";
    }
    std::cout << std::endl;
  }
}

TEST(common, component_string_params) {
  gomros::common::StringUtils::ParamsType params;

  params["name"] = "abc";
  params["age"] = "18";
  params["height"] = "180.123";

  std::string ret;
  gomros::common::StringUtils::EncodeParams(params, ret, '#');
  std::cout << "encode data : " << ret << std::endl;

  params.clear();
  EXPECT_TRUE(gomros::common::StringUtils::DecodeParams(ret, params, '#'));

  ASSERT_EQ(params["name"], "abc");
  ASSERT_EQ(params["age"], "18");
  ASSERT_EQ(params["height"], "180.123");
}
