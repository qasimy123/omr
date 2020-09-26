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

TR::AbsStackMachineState::AbsStackMachineState(TR::Region &region) :
      _array(new (region) TR::AbsLocalVarArray(region)),
      _stack(new (region) TR::AbsOpStack(region))
   {
   }

TR::AbsState* TR::AbsStackMachineState::clone(TR::Region& region) 
   {
   TR::AbsStackMachineState* copy = new (region) TR::AbsStackMachineState(region);

   copy->_array = _array->clone(region);
   copy->_stack = _stack->clone(region);

   return copy;
   }

void TR::AbsStackMachineState::merge(TR::AbsState* other)
   {
   TR::AbsStackMachineState* otherStackMachineState = static_cast<TR::AbsStackMachineState*>(other);
   _array->merge(otherStackMachineState->_array);
   _stack->merge(otherStackMachineState->_stack);
   }

void TR::AbsStackMachineState::print(TR::Compilation* comp)
   {
   traceMsg(comp, "\n|| Contents of AbsStackMachineState ||\n");
   _array->print(comp);
   _stack->print(comp);
   }

TR::AbsOpStack* TR::AbsOpStack::clone(TR::Region &region)
   {
   TR::AbsOpStack* copy = new (region) TR::AbsOpStack(region);
   for (size_t i = 0; i < _container.size(); i ++)
      {
      copy->_container.push_back(_container[i]->clone(region));
      }
   return copy;
   }

TR::AbsValue* TR::AbsOpStack::pop()
   {
   TR_ASSERT_FATAL(size() > 0, "Pop an empty stack!");
   TR::AbsValue *value = _container.back();
   _container.pop_back();
   return value;
   }

void TR::AbsOpStack::merge(TR::AbsOpStack* other)
   {
   TR_ASSERT_FATAL(other->_container.size() == _container.size(), "Stacks have different sizes!");

   for (size_t i = 0; i < _container.size(); i ++)
      {
      _container[i]->merge(other->_container[i]);
      }
   }

void TR::AbsOpStack::setToTop()
   {
   for (size_t i = 0; i <  _container.size(); i ++)
      {
      _container[i]->setToTop();
      }
   }

void TR::AbsOpStack::print(TR::Compilation* comp)
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
      TR::AbsValue *value = _container[stackSize - i -1 ];
      traceMsg(comp, "S[%d] = ", stackSize - i - 1);
      if (value)
         value->print(comp);
      traceMsg(comp, "\n");
      }

   traceMsg(comp, "<bottom>\n");
   traceMsg(comp, "\n");
   }

TR::AbsLocalVarArray* TR::AbsLocalVarArray::clone(TR::Region& region)
   {
   TR::AbsLocalVarArray* copy = new (region) TR::AbsLocalVarArray(region);
   for (size_t i = 0; i < _container.size(); i ++)
      {
      copy->_container.push_back(_container[i] ? _container[i]->clone(region) : NULL);
      }
   return copy;
   }

void TR::AbsLocalVarArray::setToTop()
   {
   for (size_t i = 0; i < _container.size(); i ++)
      {
      if (_container[i])
         _container[i]->setToTop();
      }
   }

void TR::AbsLocalVarArray::merge(TR::AbsLocalVarArray* other)
   {
   const int32_t otherSize = other->size();
   const int32_t selfSize = size();
   const int32_t mergedSize = std::max(selfSize, otherSize);

   for (int32_t i = 0; i < mergedSize; i++)
      {
      TR::AbsValue *selfValue = i < size() ? at(i) : NULL;
      TR::AbsValue *otherValue = i < other->size() ? other->at(i) : NULL;

      if (!selfValue && !otherValue) 
         {
         continue;
         }
      else if (selfValue && otherValue) 
         {
         TR::AbsValue* mergedVal = selfValue->merge(otherValue);
         set(i, mergedVal);
         } 
      else if (selfValue) 
         {
         set(i, selfValue);
         } 
      else
         {
         set(i, otherValue);
         }
      }
   }

void TR::AbsLocalVarArray::set(uint32_t index, TR::AbsValue *value)
   {
   if (size() <= index)
      {
      _container.resize(index + 1);
      }
   _container[index] = value;
   }

TR::AbsValue* TR::AbsLocalVarArray::at(uint32_t index)
   {
   TR_ASSERT_FATAL(index < size(), "Index out of range!");
   return _container[index]; 
   }

void TR::AbsLocalVarArray::print(TR::Compilation* comp)
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
      at(i)->print(comp);
      traceMsg(comp, "\n");
      }
   traceMsg(comp, "\n");
   }
