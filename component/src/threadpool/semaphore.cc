
#include "threadpool/semaphore.h"

#include <fcntl.h> /* For O_* constants */
#include <semaphore.h>
#include <sys/stat.h> /* For mode constants */

namespace gomros {
namespace threadpool {

class SemaphoreImpl {
 public:
  SemaphoreImpl(int value) : value(value) { sem_init(&sema, 0, value); }

  SemaphoreImpl(const std::string& name, int value) {
    sem_open(name.c_str(), O_CREAT, O_CREAT | O_RDWR, value);
    this->is_ipc = true;
  }
  ~SemaphoreImpl() {
    if (is_ipc)
      sem_close(&sema);
    else
      sem_destroy(&sema);
  }

  void Wait() { sem_wait(&sema); }
  
  void Signal(uint16_t limit) {
    sem_getvalue(&sema, &value);

    if (value <= limit) {
      sem_post(&sema);
    }
  }

  bool TimeWait(int timeout_ms) {
    timespec t;
    t.tv_sec = timeout_ms / 1000;
    t.tv_nsec = (timeout_ms % 1000) * 1000000;
    sem_timedwait(&sema, &t);
  }

 private:
  int value;
  sem_t sema;
  bool is_ipc = false;  // 是否是进程间信号量，true:是
};

// Semaphore

Semaphore::Semaphore(int value) { impl = new SemaphoreImpl(value); }

Semaphore::Semaphore(const std::string& name, int value) {
  impl = new SemaphoreImpl(name, value);
}

Semaphore::~Semaphore() { delete impl; }

void Semaphore::Wait() { impl->Wait(); }

void Semaphore::Signal(uint16_t limit) { impl->Signal(limit); }

bool Semaphore::TimeWait(int timeout_ms) { return impl->TimeWait(timeout_ms); }

}  // namespace threadpool
}  // namespace gomros