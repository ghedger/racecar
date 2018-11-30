// main.cc
//
// Entry point for testing racecar concurrency library.
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

#include <stdio.h>
#include <pthread.h>
#include <memory.h>
#include <iostream>
#include <time.h>

#include "singleton.h"

namespace racecar {
// timeToDie tells all the threads it's time to quit and go home.
bool time_to_die = false;
bool completed_threads= false;

// worker
// This is the worker thread process.  It
void *worker(void *params)
{
  struct timespec ts = {0, 100000000L };
  uintptr_t ident = reinterpret_cast<std::uintptr_t>(params);
  while (!time_to_die) {
      int *singletonInt = Singleton<int>::Instance();
        Singleton<int>::AcquireLock();
        std::cout << std::hex << ident << " ";
        *singletonInt = 17;
        ts.tv_nsec = (rand() & 1000000) | 1;
        nanosleep(&ts, NULL);
        Singleton<int>::ReleaseLock();

  }
  return nullptr;
}

// initThreads
void *initThreads(const int thread_tot, const int seconds_to_sleep)
{
  // Allocate thread pointers
  pthread_t *pthread = (pthread_t *) malloc(thread_tot * sizeof(pthread_t));
  if (!pthread) {
    std::cout << "Allocation error" << std::endl;
    return nullptr;
  }

  int error = 0;
  for (auto i = 0; i < thread_tot; i++) {
    error = pthread_create(pthread + i, NULL, &racecar::worker, (void *)(pthread + i));
    if (error) {
      std::cout << __FUNCTION__  << ":" << __LINE__ << std::endl;
    }
  }
  return pthread;
}

// shutdownThreads

void shutdownThreads(void *pthread)
{
  if(pthread) {
    free(pthread);
  }
}

} // namespace racecar

// printUsage
void printUsage()
{
  std::cout << "racecar" << std::endl;
  std::cout << std::endl;
  std::cout << "Racecar is a concurrency library" << std::endl;
  std::cout << std::endl;
  std::cout << "Usage:" << std::endl;
  std::cout << "\tracecar <thread_total> <seconds_to_sleep>" << std::endl;
}

// main
int main(int argc, const char *argv[])
{
  // Validate params
  if (argc < 3) {
    printUsage();
    return -1;
  }

  int thread_tot, seconds_to_sleep;
  thread_tot = 0;
  seconds_to_sleep = 1;

  sscanf(argv[1], "%d", &thread_tot);
  sscanf(argv[2], "%d", &seconds_to_sleep);

  if (thread_tot <= 0 || seconds_to_sleep <= 0) {
    printUsage();
    return -1;
  }

  void *threads = racecar::initThreads(thread_tot, seconds_to_sleep);

  struct timespec ts = {seconds_to_sleep, 10000000L };

  nanosleep(&ts, NULL);

  racecar::shutdownThreads(threads);

  // Instantiate some primitive types
  return 0;
}

