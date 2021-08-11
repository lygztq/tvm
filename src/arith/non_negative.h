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

/*!
 * \file non_negative.h
 * \brief Non-negatvie expr checker.
 */
#ifndef TVM_ARITH_NON_NEGATIVE_H_
#define TVM_ARITH_NON_NEGATIVE_H_

#include <tvm/arith/analyzer.h>
#include <tvm/tir/expr_functor.h>

#include <unordered_map>


namespace tvm {
namespace arith {

using namespace tir;

/*!
 * \brief A visitor to check whether a expression is non-negative.
 * \note This checker is especially for the rewrite simplifier and canonical
 *  simplifier with symbolic var boundary. We guarantee all positive results
 *  are true, but negative results may also be derived from non-negative
 *  expressions since we cannot know the actual bound of some vars at the
 *  compilation time, especially under dynamic cases.
 */
class NonNegativeChecker::Impl : public ExprVisitor {
 public:
  /*! \brief Check whether the input expr is non-negative. */
  bool Check(const PrimExpr& expr) {
    non_negative_map_.clear();
    this->VisitExpr(expr);
    return MustNonNegative(expr);
  }

  void VisitExpr_(const VarNode* op) final {
    non_negative_map_[GetRef<Var>(op)] = false;
  }
  void VisitExpr_(const SizeVarNode* op) final {
    non_negative_map_[GetRef<SizeVar>(op)] = true;
  }
  void VisitExpr_(const AddNode* op) final {
    ExprVisitor::VisitExpr_(op);
    non_negative_map_[GetRef<Add>(op)] = MustNonNegative(op->a) && MustNonNegative(op->b);
  }
  void VisitExpr_(const SubNode* op) final {
    non_negative_map_[GetRef<Sub>(op)] = false;
  }
  void VisitExpr_(const MulNode* op) final {
    ExprVisitor::VisitExpr_(op);
    non_negative_map_[GetRef<Mul>(op)] = MustNonNegative(op->a) && MustNonNegative(op->b);
  }
  void VisitExpr_(const DivNode* op) final {
    ExprVisitor::VisitExpr_(op);
    non_negative_map_[GetRef<Div>(op)] = MustNonNegative(op->a) && MustNonNegative(op->b);
  }
  void VisitExpr_(const ModNode* op) final {
    ExprVisitor::VisitExpr_(op);
    non_negative_map_[GetRef<Mod>(op)] = MustNonNegative(op->b);
  }
  void VisitExpr_(const FloorDivNode* op) final {
    ExprVisitor::VisitExpr_(op);
    non_negative_map_[GetRef<FloorDiv>(op)] = MustNonNegative(op->a) && MustNonNegative(op->b);
  }
  void VisitExpr_(const FloorModNode* op) final {
    ExprVisitor::VisitExpr_(op);
    non_negative_map_[GetRef<FloorMod>(op)] = MustNonNegative(op->b);
  }
  void VisitExpr_(const MinNode* op) final {
    ExprVisitor::VisitExpr_(op);
    non_negative_map_[GetRef<Min>(op)] = MustNonNegative(op->a) && MustNonNegative(op->b);
  }
  void VisitExpr_(const MaxNode* op) final {
    ExprVisitor::VisitExpr_(op);
    non_negative_map_[GetRef<Max>(op)] = MustNonNegative(op->a) && MustNonNegative(op->b);
  }
  void VisitExpr_(const AndNode* op) final {
    non_negative_map_[GetRef<And>(op)] = true;
  }
  void VisitExpr_(const OrNode* op) final {
    non_negative_map_[GetRef<Or>(op)] = true;
  }
  void VisitExpr_(const IntImmNode* op) final {
    non_negative_map_[GetRef<IntImm>(op)] = (op->value >= 0);
  }
  void VisitExpr_(const FloatImmNode* op) final {
    non_negative_map_[GetRef<FloatImm>(op)] = (op->value >= 0);
  }

  bool MustNonNegative(const PrimExpr& e) {
    if (non_negative_map_.count(e) && non_negative_map_[e]) {
      return true;
    }
    return false;
  }
 private:
  std::unordered_map<PrimExpr, bool, ObjectPtrHash, ObjectPtrEqual> non_negative_map_;
};

} // namespace arith   
} // namespace tvm


#endif  // TVM_ARITH_NON_NEGATIVE_H_
