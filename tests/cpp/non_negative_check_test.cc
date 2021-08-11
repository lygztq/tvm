/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#include <dmlc/logging.h>
#include <gtest/gtest.h>
#include <tvm/arith/analyzer.h>
#include <tvm/te/operation.h>

#include <tvm/node/structural_equal.h>

TEST(NonNegative, SizeVar) {
  tvm::arith::NonNegativeChecker checker;
  auto x = tvm::te::SizeVar("x");
  CHECK_EQ(checker(x), true);
}

TEST(NonNegative, Var) {
  tvm::arith::NonNegativeChecker checker;
  auto x = tvm::te::var("x");
  CHECK_EQ(checker(x), false);
}

TEST(NonNegative, SizeVarMulAdd) {
  tvm::arith::NonNegativeChecker checker;
  auto x = tvm::te::SizeVar("x");
  auto y = tvm::te::SizeVar("y");
  auto z = tvm::te::SizeVar("z");
  auto e1 = tvm::mul(x, y);
  auto e2 = tvm::add(e1, z);
  CHECK_EQ(checker(e2), true);
}

TEST(NonNegative, SizeVarMulAddSub) {
  tvm::arith::NonNegativeChecker checker;
  auto x = tvm::te::SizeVar("x");
  auto y = tvm::te::SizeVar("y");
  auto z = tvm::te::SizeVar("z");
  auto e1 = tvm::mul(x, y);
  auto e2 = tvm::add(e1, z);
  auto e3 = tvm::sub(e2, x);
  CHECK_EQ(checker(e3), false);
}

// TEST(NonNegative, StructuralEqual) {
//   auto x = tvm::te::SizeVar("x");
//   auto y = tvm::te::SizeVar("y");
//   auto z = tvm::te::SizeVar("z");
//   auto e1 = tvm::mul(x, tvm::mul(y, z));
//   auto e2 = tvm::mul(z, tvm::mul(y, x));
//   ICHECK(tvm::StructuralEqual()(e1, e2));
// }

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  testing::FLAGS_gtest_death_test_style = "threadsafe";
  return RUN_ALL_TESTS();
}
