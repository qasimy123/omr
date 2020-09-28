/*******************************************************************************
 * Copyright (c) 2020, 2020 IBM Corp. and others
 *
 * This program and the accompanying materials are made available under
 * the terms of the Eclipse Public License 2.0 which accompanies this
 * distribution and is available at http://eclipse.org/legal/epl-2.0
 * or the Apache License, Version 2.0 which accompanies this distribution
 * and is available at https://www.apache.org/licenses/LICENSE-2.0.
 *
 * This Source Code may also be made available under the following Secondary
 * Licenses when the conditions for such availability set forth in the
 * Eclipse Public License, v. 2.0 are satisfied: GNU General Public License,
 * version 2 with the GNU Classpath Exception [1] and GNU General Public
 * License, version 2 with the OpenJDK Assembly Exception [2].
 *
 * [1] https://www.gnu.org/software/classpath/license.html
 * [2] http://openjdk.java.net/legal/assembly-exception.html
 *
 * SPDX-License-Identifier: EPL-2.0 OR Apache-2.0 OR GPL-2.0 WITH Classpath-exception-2.0 OR LicenseRef-GPL-2.0 WITH Assembly-exception
 *******************************************************************************/

#ifndef INLINING_METHOD_SUMMARY_INCL
#define INLINING_METHOD_SUMMARY_INCL

#include "infra/deque.hpp"
#include "optimizer/abstractinterpreter/AbsValue.hpp"

namespace TR {

/**
 * A potential optimization will be unlocked by inlining.
 */
class PotentialOptimization
   {
   public:
   enum OptKind
      {
      BranchFolding,
      NullCheckFolding,
      InstanceOfFolding,
      CheckCastFolding
      };

   PotentialOptimization(int32_t bci, TR::ResolvedMethodSymbol* symbol, TR::PotentialOptimization::OptKind optKind) :
         _bytecodeIndex(bci),
         _symbol(symbol),
         _optKind(optKind)
      {}

   void trace(TR::Compilation* comp);

   const char* getOptKindName();

   private:
   int32_t _bytecodeIndex;
   TR::ResolvedMethodSymbol* _symbol;
   TR::PotentialOptimization::OptKind _optKind;
   };

/**
 * The inlinling method summary captures potentail optimizations after inlining a particular method.
 */
class InliningMethodSummary
   {
   public:
   InliningMethodSummary(TR::Region& region) :
         _opts(region)
      {}

   void addOpt(TR::PotentialOptimization* opt) { _opts.push_back(opt); }

   uint32_t getIndirectBenefit() { return _opts.size(); };

   void trace(TR::Compilation* comp);

   private:
   TR::deque<TR::PotentialOptimization*, TR::Region&> _opts;
   };

/**
 * A Predicate tests if the given value is a safe value to unlock an optimization.
 */
class BranchFoldingPredicate
   {
   public:
   enum Kind
      {
      IfEq,
      IfNe,
      IfLt,
      IfGt,
      IfLe,
      IfGe
      };

   static bool predicate(int32_t low, int32_t high, TR::BranchFoldingPredicate::Kind kind);

   static bool takeTheBranch(int32_t low, int32_t high, TR::BranchFoldingPredicate::Kind kind);
   static bool notTakeTheBranch(int32_t low, int32_t high, TR::BranchFoldingPredicate::Kind kind);
   };

class NullBranchFoldingPredicate
   {
   public:
   enum Kind 
      {
      IfNull,
      IfNonNull
      };

   static bool predicate(TR_YesNoMaybe isNonNull, TR::NullBranchFoldingPredicate::Kind kind);

   static bool takeTheBranch(TR_YesNoMaybe isNonNull, TR::NullBranchFoldingPredicate::Kind kind);
   static bool notTakeTheBranch(TR_YesNoMaybe isNonNull, TR::NullBranchFoldingPredicate::Kind kind);
   };

class NullCheckFoldingPredicate
   {
   public:
   static bool predicate(TR_YesNoMaybe isNonNull);

   static bool removeNullCheck(TR_YesNoMaybe isNonNull);
   static bool throwException(TR_YesNoMaybe isNonNull);
   };

class InstanceOfFoldingPredicate 
   {
   public:
   static bool predicate(TR_YesNoMaybe isNonNull, TR_OpaqueClassBlock* instanceClass, bool isFixedClass, TR_OpaqueClassBlock* castClass, TR_FrontEnd* fe);

   static bool foldToTrue(TR_YesNoMaybe isNonNull, TR_OpaqueClassBlock* instanceClass, bool isFixedClass, TR_OpaqueClassBlock* castClass, TR_FrontEnd* fe);
   static bool foldToFalse(TR_YesNoMaybe isNonNull, TR_OpaqueClassBlock* instanceClass, bool isFixedClass, TR_OpaqueClassBlock* castClass, TR_FrontEnd* fe);
   };

class CheckCastFoldingPredicate
   {
   public:
   static bool predicate(TR_YesNoMaybe isNonNull, TR_OpaqueClassBlock* checkClass, bool isFixedClass, TR_OpaqueClassBlock* castClass, TR_FrontEnd* fe);

   static bool checkCastSucceed(TR_YesNoMaybe isNonNull, TR_OpaqueClassBlock* checkClass, bool isFixedClass, TR_OpaqueClassBlock* castClass, TR_FrontEnd* fe);
   static bool throwException(TR_YesNoMaybe isNonNull, TR_OpaqueClassBlock* checkClass, bool isFixedClass, TR_OpaqueClassBlock* castClass, TR_FrontEnd* fe);
   };

}

#endif
