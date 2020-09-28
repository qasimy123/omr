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

#include "optimizer/abstractinterpreter/InliningMethodSummary.hpp"

void TR::PotentialOptimization::trace(TR::Compilation* comp)
   {
   traceMsg(comp, "Potential Optimization: %s at bytecode Index: %d in method %s\n", getOptKindName(), _bytecodeIndex, _symbol->signature(comp->trMemory()));
   }

const char* TR::PotentialOptimization::getOptKindName()
   {
   switch (_optKind)
      {
      case BranchFolding: return "Branch Folding";
      case NullCheckFolding: return "NullCheck Folding";
      case InstanceOfFolding: return "InstanceOf Folding";
      case CheckCastFolding: return "CheckCast Folding";
      default: TR_ASSERT_FATAL(false, "Unexpected type");
      }
   }

void TR::InliningMethodSummary::trace(TR::Compilation* comp)
   {
   traceMsg(comp, "Total %d Potential Optimizations will be unlocked if inlining this method\n", _opts.size());
   for (uint32_t i = 0; i < _opts.size(); i ++)
      {
      _opts[i]->trace(comp);
      }
   }

bool TR::BranchFoldingPredicate::predicate(int32_t low, int32_t high, TR::BranchFoldingPredicate::Kind kind)
   {
   return takeTheBranch(low, high, kind) || notTakeTheBranch(low, high, kind);
   }

bool TR::BranchFoldingPredicate::takeTheBranch(int32_t low, int32_t high, TR::BranchFoldingPredicate::Kind kind)
   {
   switch (kind)
      {
      case IfEq:
         {
         if (low == high && low == 0) 
            return true;

         return false;
         }
      case IfNe:
         {
         if (low >= 1 || high <= -1 )
            return true;

         return false;
         }
      case IfGe:
         {
         if (low >= 0)
            return true;

         return false;
         }
      case IfLe:
         {
         if (high <= 0)
            return true;
         
         return false;
         }
      case IfLt:
         {
         if (high <= -1)
            return true;

         return false;
         }
      case IfGt:
         {
         if (low >= 1)
            return true;

         return false;
         }
      default: TR_ASSERT_FATAL(false, "Unexpected type");
      }
   }

bool TR::BranchFoldingPredicate::notTakeTheBranch(int32_t low, int32_t high, TR::BranchFoldingPredicate::Kind kind)
   {
   switch (kind)
      {
      case IfEq:
         {
         if (low >= 1 || high <= -1)
            return true;

         return false;
         }
      case IfNe:
         {
         if (low == high && low == 0)
            return true;

         return false;
         }
      case IfGe:
         {
         if (high <= -1)
            return true;

         return false;
         }
      case IfLe:
         {
         if (low >= 1)
            return true;
         
         return false;
         }
      case IfLt:
         {
         if (low >= 0)
            return true;

         return false;
         }
      case IfGt:
         {
         if (high <= 0)
            return true;

         return false;
         }
      default: TR_ASSERT_FATAL(false, "Unexpected type");
      }
   }

bool TR::NullBranchFoldingPredicate::takeTheBranch(TR_YesNoMaybe isNonNull, Kind kind)
   {
   if ((isNonNull == TR_yes && kind == IfNonNull) || (isNonNull == TR_no && kind == IfNull))
      return true;

   return false;
   }

bool TR::NullBranchFoldingPredicate::notTakeTheBranch(TR_YesNoMaybe isNonNull, Kind kind)
   {
   if ((isNonNull == TR_yes && kind == IfNull) || (isNonNull == TR_no && kind == IfNonNull))
      return true;
   
   return false;
   }

bool TR::NullBranchFoldingPredicate::predicate(TR_YesNoMaybe isNonNull, Kind kind)
   {
   if (takeTheBranch(isNonNull, kind) || notTakeTheBranch(isNonNull, kind))
      return true;

   return false;
   }

bool TR::NullCheckFoldingPredicate::removeNullCheck(TR_YesNoMaybe isNonNull)
   {
   return isNonNull == TR_yes;
   }

bool TR::NullCheckFoldingPredicate::throwException(TR_YesNoMaybe isNonNull)
   {
   return isNonNull == TR_no;
   }

bool TR::NullCheckFoldingPredicate::predicate(TR_YesNoMaybe isNonNull)
   {
   if (removeNullCheck(isNonNull) || throwException(isNonNull))
      return true;

   return false;
   }

bool TR::InstanceOfFoldingPredicate::foldToTrue(TR_YesNoMaybe isNonNull, TR_OpaqueClassBlock* instanceClass, bool isFixedClass, TR_OpaqueClassBlock* castClass, TR_FrontEnd* fe)
   {
   if (!instanceClass || !castClass)
      return false;

   TR_YesNoMaybe isInstance = fe->isInstanceOf(instanceClass, castClass, isFixedClass, true);

   if (isInstance == TR_yes)
      return true;

   return false;
   }

bool TR::InstanceOfFoldingPredicate::foldToFalse(TR_YesNoMaybe isNonNull, TR_OpaqueClassBlock* instanceClass, bool isFixedClass, TR_OpaqueClassBlock* castClass, TR_FrontEnd* fe)
   {
   if (isNonNull == TR_no) //instanceof null
      return true;

   if (!instanceClass || !castClass)
      return false;

   TR_YesNoMaybe isInstance = fe->isInstanceOf(instanceClass, castClass, isFixedClass, true);

   if (isInstance == TR_no)
      return true;

   return false;
   }

bool TR::InstanceOfFoldingPredicate::predicate(TR_YesNoMaybe isNonNull, TR_OpaqueClassBlock* instanceClass, bool isFixedClass, TR_OpaqueClassBlock* castClass, TR_FrontEnd* fe)
   {
   if (foldToTrue(isNonNull, instanceClass, isFixedClass, castClass, fe) || foldToFalse(isNonNull, instanceClass, isFixedClass, castClass, fe))
      return true;

   return false;
   }

bool TR::CheckCastFoldingPredicate::checkCastSucceed(TR_YesNoMaybe isNonNull, TR_OpaqueClassBlock* checkClass, bool isFixedClass, TR_OpaqueClassBlock* castClass, TR_FrontEnd* fe)
   {
   if (isNonNull == TR_no) //checkcast null
      return true;

   if (!checkClass || !castClass)
      return false;

   TR_YesNoMaybe isInstance = fe->isInstanceOf(checkClass, castClass, isFixedClass, true);

   if (isInstance == TR_yes)
      return true;

   return false;
   }

bool TR::CheckCastFoldingPredicate::throwException(TR_YesNoMaybe isNonNull, TR_OpaqueClassBlock* checkClass, bool isFixedClass, TR_OpaqueClassBlock* castClass, TR_FrontEnd* fe)
   {
   if (!checkClass || !castClass)
      return false;

   TR_YesNoMaybe isInstance = fe->isInstanceOf(checkClass, castClass, isFixedClass, true);

   if (isInstance == TR_no)
      return true;

   return false;
   }

bool TR::CheckCastFoldingPredicate::predicate(TR_YesNoMaybe isNonNull, TR_OpaqueClassBlock* checkClass, bool isFixedClass, TR_OpaqueClassBlock* castClass, TR_FrontEnd* fe)
   {
   if (checkCastSucceed(isNonNull, checkClass, isFixedClass, castClass, fe) || throwException(isNonNull, checkClass, isFixedClass, castClass, fe))
      return true;
      
   return false;
   }
