
#include "include/common/backtrace.h"

#include <gtest/gtest.h>
#include <queue>

void test_sig_segv() {
  int* p = nullptr;
  printf("*p = %d\n", *p);
}

void test_sig_abort() { abort(); }

void test_sig_abort_free() {
  int* p = (int*)malloc(sizeof(int));
  *p = 1;
  printf("test_sig_abort_free: 0x%p %d\n", p, *p);
  free(p);
  printf("free once\n");
  free(p);
  printf("free twice\n");
}

TEST(backtrace_test, segv) {
  printf("-----------test start------------\n");
  common::ErrorBacktraceRegister();

  // test_sig_segv();
  // test_sig_abort();
  // test_sig_abort_free();

  // 使用 EXPECT_DEATH 不会触发注册的backtarce函数
  printf("test_sig_segv \n");
  EXPECT_DEATH(test_sig_segv(), "");

  printf("test_sig_abort \n");
  EXPECT_DEATH(test_sig_abort(), "");

  printf("test_sig_abort_free \n");
  EXPECT_DEATH(test_sig_abort_free(), "");
}