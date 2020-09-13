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

#include "optimizer/abstractinterpreter/AbsStackMachineState.hpp"

AbsStackMachineState::AbsStackMachineState(TR::Region &region) :
      _array(region),
      _stack(region)
   {
   }

AbsStackMachineState::AbsStackMachineState(AbsStackMachineState* other, TR::Region& region) :
      _array(other->_array, region),
      _stack(other->_stack, region)
   {
   }

void AbsStackMachineState::merge(AbsStackMachineState* other, TR::ValuePropagation *vp)
   {
   _array.merge(other->_array, vp);
   _stack.merge(other->_stack, vp);
   }


void AbsStackMachineState::print(TR::Compilation* comp, TR::ValuePropagation *vp)
   {
   traceMsg(comp, "\n|| Contents of AbsStackMachineState ||\n");
   _array.print(comp,vp);
   _stack.print(comp,vp);
   }

AbsOpStack::AbsOpStack(TR::Region &region) :
      _stack(region)
   {
   }

AbsOpStack::AbsOpStack(AbsOpStack &other, TR::Region &region) :
      _stack(region)
   {
   for (size_t i = 0; i < other.size(); i ++)
      push(AbsValue::create(region, other._stack[i]));  
   }

AbsValue* AbsOpStack::pop()
   {
   TR_ASSERT_FATAL(size() > 0, "Pop an empty stack!");
   AbsValue *value = _stack.back();
   _stack.pop_back();
   return value;
   }

void AbsOpStack::merge(AbsOpStack &other, TR::ValuePropagation *vp)
   {
   TR_ASSERT_FATAL(other._stack.size() == _stack.size(), "Stacks have different sizes!");

   for (size_t i = 0; i < _stack.size(); i ++)
      _stack[i]->merge(other._stack[i], vp);
   }

void AbsOpStack::setToTop()
   {
   for (size_t i = 0; i <  _stack.size(); i ++)
      {
      _stack[i]->setToTop();
      }
   }

void AbsOpStack::print(TR::Compilation* comp, TR::ValuePropagation *vp)
   {
   traceMsg(comp, "Contents of Abstract Operand Stack:\n");
   
   const int32_t stackSize = size();

   if (stackSize == 0)
      {
      traceMsg(comp, "<empty>\n");
      traceMsg(comp, "\n");
      return;
      }
   
   traceMsg(comp, "<top>\n");

   for (int32_t i = 0; i < stackSize; i++) 
      {
      AbsValue *value = _stack[stackSize - i -1 ];
      traceMsg(comp, "S[%d] = ", stackSize - i - 1);
      if (value)
         value->print(comp, vp);
      traceMsg(comp, "\n");
      }

   traceMsg(comp, "<bottom>\n");
   traceMsg(comp, "\n");
   }

AbsLocalVarArray::AbsLocalVarArray(TR::Region &region) :
      _array(region)
   {
   }

AbsLocalVarArray::AbsLocalVarArray(AbsLocalVarArray &other, TR::Region& region) :
      _array(region)
   {
   for (size_t i = 0; i < other._array.size(); i++)
      {
      _array.push_back(other._array[i] ? AbsValue::create(region, other._array[i]) : NULL);
      }
   }

void AbsLocalVarArray::setToTop()
   {
   for (size_t i = 0; i < _array.size(); i ++)
      {
      if (_array[i])
         _array[i]->setToTop();
      }
   }
   
void AbsLocalVarArray::merge(AbsLocalVarArray &other, TR::ValuePropagation *vp)
   {
   const int32_t otherSize = other.size();
   const int32_t selfSize = size();
   const int32_t mergedSize = std::max(selfSize, otherSize);

   for (int32_t i = 0; i < mergedSize; i++)
      {
      AbsValue *selfValue = i < size() ? at(i) : NULL;
      AbsValue *otherValue = i < other.size() ? other.at(i) : NULL;

      if (!selfValue && !otherValue) 
         {
         continue;
         }
      else if (selfValue && otherValue) 
         {
         AbsValue* mergedVal = selfValue->merge(otherValue, vp);
         set(i, mergedVal);
         } 
      else if (selfValue) 
         {
         set(i, selfValue);
         } 
      else if (otherValue)
         {
         set(i, otherValue);
         }
      }
   }

void AbsLocalVarArray::set(uint32_t index, AbsValue *value)
   {
   if (size() <= index)
      {
      _array.resize(index + 1);
      }
   _array[index] = value;
   }
   
AbsValue* AbsLocalVarArray::at(uint32_t index)
   {
   TR_ASSERT_FATAL(index < size(), "Index out of range!");
   return _array[index]; 
   }

void AbsLocalVarArray::print(TR::Compilation* comp, TR::ValuePropagation *vp)
   {
   traceMsg(comp, "Contents of Abstract Local Variable Array:\n");
   const int32_t arraySize = size();
   for (int32_t i = 0; i < arraySize; i++)
      {
      traceMsg(comp, "A[%d] = ", i);
      if (!at(i))
         {
         traceMsg(comp, "NULL\n");
         continue;
         }
      at(i)->print(comp, vp);
      traceMsg(comp, "\n");
      }
   traceMsg(comp, "\n");
   }
