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

#ifndef ABS_INTERPRETER_TEST_HPP
#define ABS_INTERPRETER_TEST_HPP

#include <gtest/gtest.h>
#include <exception>
#include "CompilerUnitTest.hpp"
#include "optimizer/abstractinterpreter/AbsValue.hpp"
#include "optimizer/GlobalValuePropagation.hpp"

namespace TRTest {

class AbsInterpreterTest : public TRTest::CompilerUnitTest
   {
   public:
   AbsInterpreterTest() :
         CompilerUnitTest()
      {
      _manager = new (_comp.allocator()) TR::OptimizationManager(_optimizer, TR::GlobalValuePropagation::create, OMR::globalValuePropagation);
      _vp = static_cast<TR::GlobalValuePropagation*>(TR::GlobalValuePropagation::create(_manager));
      _vp->initialize();
      }

   TR::ValuePropagation* vp() { return _vp; }

   private:
   TR::OptimizationManager* _manager;
   TR::GlobalValuePropagation* _vp;
   };

class AbsTestValue : public TR::AbsValue
   {
   public:
   AbsTestValue(TR::DataType dataType) :
         TR::AbsValue(dataType),
         _isTop(false)
   {}

   virtual TR::AbsValue* clone(TR::Region& region) const
      {
      TRTest::AbsTestValue* copy = new (region) TRTest::AbsTestValue(_dataType, _paramPos, _isTop);
      return copy;
      } 

   virtual bool isTop() const { return _isTop; }

   virtual void setToTop() { _isTop = true; }

   virtual TR::AbsValue* merge(const TR::AbsValue *other) { return this; }

   virtual void print(TR::Compilation* comp) const {} 

   private:

   AbsTestValue(TR::DataType dataType, int32_t paramPos, bool isTop) :
         TR::AbsValue(dataType, paramPos),
         _isTop(isTop)
   {}

   bool _isTop;

   };
}


#endif
