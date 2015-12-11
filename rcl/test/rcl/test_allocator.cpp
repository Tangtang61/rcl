// Copyright 2015 Open Source Robotics Foundation, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <gtest/gtest.h>

#include "rcl/allocator.h"

#include "../memory_tools.hpp"

class TestAllocatorFixture : public::testing::Test
{
public:
  TestAllocatorFixture()
  {
    start_memory_checking();
    stop_memory_checking();
  }
  void SetUp()
  {
    set_on_unepexcted_malloc_callback([]() {EXPECT_FALSE(true) << "UNEXPECTED MALLOC";});
    set_on_unepexcted_realloc_callback([]() {EXPECT_FALSE(true) << "UNEXPECTED REALLOC";});
    set_on_unepexcted_free_callback([]() {EXPECT_FALSE(true) << "UNEXPECTED FREE";});
    start_memory_checking();
  }

  void TearDown()
  {
    assert_no_malloc_end();
    assert_no_realloc_end();
    assert_no_free_end();
    stop_memory_checking();
    set_on_unepexcted_malloc_callback(nullptr);
    set_on_unepexcted_realloc_callback(nullptr);
    set_on_unepexcted_free_callback(nullptr);
  }
};

/* Tests the default allocator.
 */
TEST_F(TestAllocatorFixture, test_default_allocator_normal) {
  ASSERT_NO_MALLOC(
    rcl_allocator_t allocator = rcl_get_default_allocator();
  )
  size_t mallocs = 0;
  size_t reallocs = 0;
  size_t frees = 0;
  set_on_unepexcted_malloc_callback([&mallocs]() {
    mallocs++;
  });
  set_on_unepexcted_realloc_callback([&reallocs]() {
    reallocs++;
  });
  set_on_unepexcted_free_callback([&frees]() {
    frees++;
  });
  assert_no_malloc_begin();
  assert_no_realloc_begin();
  assert_no_free_begin();
  void * allocated_memory = allocator.allocate(1024, allocator.state);
  EXPECT_EQ(mallocs, 1);
  EXPECT_NE(allocated_memory, nullptr);
  allocated_memory = allocator.reallocate(allocated_memory, 2048, allocator.state);
  EXPECT_EQ(reallocs, 1);
  EXPECT_NE(allocated_memory, nullptr);
  allocator.deallocate(allocated_memory, allocator.state);
  EXPECT_EQ(mallocs, 1);
  EXPECT_EQ(reallocs, 1);
  EXPECT_EQ(frees, 1);
}
