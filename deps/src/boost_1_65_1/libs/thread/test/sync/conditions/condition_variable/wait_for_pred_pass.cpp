//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
// Copyright (C) 2011 Vicente J. Botet Escriba
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// <boost/thread/condition_variable>

// class condition_variable;

// condition_variable(const condition_variable&) = delete;

#include <boost/thread/condition_variable.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/thread.hpp>
#include <boost/detail/lightweight_test.hpp>
#include <cassert>
#include <iostream>

#if defined BOOST_THREAD_USES_CHRONO

class Pred
{
  int& i_;
public:
  explicit Pred(int& i) :
    i_(i)
  {
  }

  bool operator()()
  {
    return i_ != 0;
  }
};

boost::condition_variable cv;
boost::mutex mut;

int test1 = 0;
int test2 = 0;

int runs = 0;

void f()
{
  try {
    typedef boost::chrono::system_clock Clock;
    typedef boost::chrono::milliseconds milliseconds;
    boost::unique_lock < boost::mutex > lk(mut);
    assert(test2 == 0);
    test1 = 1;
    cv.notify_one();
    Clock::time_point t0 = Clock::now();
    int count=0;
    //bool r =
        (void)cv.wait_for(lk, milliseconds(250), Pred(test2));
    count++;
    Clock::time_point t1 = Clock::now();
    if (runs == 0)
    {
      // This test is spurious as it depends on the time the thread system switches the threads
      assert(t1 - t0 < milliseconds(250+1000));
      assert(test2 != 0);
    }
    else
    {
      assert(t1 - t0 - milliseconds(250) < milliseconds(count*250+2));
      assert(test2 == 0);
    }
    ++runs;
  } catch(...) {
    std::cout << "ERROR exception" << __LINE__ << std::endl;
    assert(false);
  }
}

int main()
{
  try
  {
    boost::unique_lock < boost::mutex > lk(mut);
    boost::thread t(f);
    BOOST_TEST(test1 == 0);
    while (test1 == 0)
      cv.wait(lk);
    BOOST_TEST(test1 != 0);
    test2 = 1;
    lk.unlock();
    cv.notify_one();
    t.join();
  } catch(...) {
    BOOST_TEST(false);
    std::cout << "ERROR exception" << __LINE__ << std::endl;
  }
  test1 = 0;
  test2 = 0;
  try
  {
    boost::unique_lock < boost::mutex > lk(mut);
    boost::thread t(f);
    BOOST_TEST(test1 == 0);
    while (test1 == 0)
      cv.wait(lk);
    BOOST_TEST(test1 != 0);
    lk.unlock();
    t.join();
  } catch(...) {
    BOOST_TEST(false);
    std::cout << "ERROR exception" << __LINE__ << std::endl;
  }

  return boost::report_errors();
}

#else
#error "Test not applicable: BOOST_THREAD_USES_CHRONO not defined for this platform as not supported"
#endif
