
#include "include/common/threadpool.h"

#include <gtest/gtest.h>

class ThreadPoolTestWork : public common::basic_work {
 public:
  ThreadPoolTestWork(int id) : id(id){};
  ~ThreadPoolTestWork() {}

  virtual void run() { printf("id = %d is running ... \n", id); };

 private:
  int id;
};

TEST(sample_test, threadpool) {
  printf("-----------test start------------\n");

  auto tpool = new common::ThreadPool();

  int work_num = 10000;

  for (size_t i = 0; i < work_num; i++) {
    auto item = std::make_shared<ThreadPoolTestWork>(i);
    tpool->AddNewWork(item, std::to_string(i % 10));
  }

  sleep(5);

  printf("-----------delete tpool------------\n");

  delete tpool;
}