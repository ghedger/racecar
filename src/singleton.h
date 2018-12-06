// singleton.h
//
// Implements a singleton pattern.
//
// This file is part of racecar, a collectionof concurrency-related classes.
//
// Sortbench is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Racecar is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with sortbench.  If not, see <https://www.gnu.org/licenses/>.
//

#ifndef SINGLETON_H
#define SINGLETON_H

#include <sched.h>

namespace racecar {

// Lock
// Simple lock class for acquisition and release of lock
class Lock {
 public:
  Lock() { lock_ = 0; }
  ~Lock() {}
  // Acquire
  inline void Acquire() {
    #if 0
    while (lock_) {
      // Occupado, sister; Force a context switch and wait our turn
      // (TODO: Is this even necessary?)
      sched_yield();
    }
    #endif
    // Low-level lock on Intel processors
    __sync_lock_test_and_set(&lock_, 1);
  }
  // Release
  inline void Release() {
    // Low-level release on Intel chips
    __sync_lock_release(&lock_);
  }
 private:
  volatile int lock_;
};

// Singleton
// Template class to provide singleton lock on a given data type/class
template <class T> class Singleton {
 private:
  static Lock lock_;
  static T * object_;     // declaration
 protected:
  Singleton() {}
 public:
  static T * Instance();
  static void AcquireLock();
  static void ReleaseLock();
};

template <class T> Lock Singleton<T>::lock_;
template <class T> T * Singleton<T>::object_;   // definition
template <class T>
T *Singleton<T>::Instance() {
  // If nothing currently claims the lock, attempt to acquire it.
  if (object_ == 0) {
    lock_.Acquire();
    // Need to account for possibility that two threads met the condition
    // and only one must get the lock (create the instance)
    if (object_ == 0) {
      object_ = new T;
      std::cout << "New alloc" << std::endl;
    }
    lock_.Release();
  }
  return object_;
}

// AcquireLock
// Acquire the mutex lock
// Blocking call - waits until lock becomes available
template <class T>
void Singleton<T>::AcquireLock() {
  lock_.Acquire();
}

// ReleaseLock
// Relinquish the mutex lock
template <class T>
void Singleton<T>::ReleaseLock() {
  lock_.Release();
}
} // namespace racecar
#endif // #ifndef SINGLETON_H

