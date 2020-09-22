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

template <typename Constraint>
TR::AbsStackMachineState<Constraint>::AbsStackMachineState(TR::Region &region) :
      _array(new (region) TR::AbsLocalVarArray<Constraint>(region)),
      _stack(new (region) TR::AbsOpStack<Constraint>(region))
   {
   }

template <typename Constraint>
TR::AbsStackMachineState<Constraint>* TR::AbsStackMachineState<Constraint>::clone(TR::Region& region) 
   {
   TR::AbsStackMachineState<Constraint>* copy = new (region) TR::AbsStackMachineState<Constraint>(region);

   copy->_array = _array->clone(region);
   copy->_stack = _stack->clone(region);

   return copy;
   }

template <typename Constraint>
void TR::AbsStackMachineState<Constraint>::merge(TR::AbsStackMachineState<Constraint>* other)
   {
   _array->merge(other->_array);
   _stack->merge(other->_stack);
   }

template <typename Constraint>
void TR::AbsStackMachineState<Constraint>::print(TR::Compilation* comp)
   {
   traceMsg(comp, "\n|| Contents of AbsStackMachineState ||\n");
   _array->print(comp);
   _stack->print(comp);
   }

template <typename Constraint>
TR::AbsOpStack<Constraint>* TR::AbsOpStack<Constraint>::clone(TR::Region &region)
   {
   TR::AbsOpStack<Constraint>* copy = new (region) TR::AbsOpStack<Constraint>(region);
   for (size_t i = 0; i < _container.size(); i ++)
      {
      copy->_container.push_back(_container[i]->clone(region));
      }
   return copy;
   }

template <typename Constraint>
TR::AbsValue<Constraint>* TR::AbsOpStack<Constraint>::pop()
   {
   TR_ASSERT_FATAL(size() > 0, "Pop an empty stack!");
   TR::AbsValue<Constraint> *value = _container.back();
   _container.pop_back();
   return value;
   }

template <typename Constraint>
void TR::AbsOpStack<Constraint>::merge(TR::AbsOpStack<Constraint>* other)
   {
   TR_ASSERT_FATAL(other->_container.size() == _container.size(), "Stacks have different sizes!");

   for (size_t i = 0; i < _container.size(); i ++)
      {
      _container[i]->merge(other->_container[i]);
      }
   }

template <typename Constraint>
void TR::AbsOpStack<Constraint>::setToTop()
   {
   for (size_t i = 0; i <  _container.size(); i ++)
      {
      _container[i]->setToTop();
      }
   }

template <typename Constraint>
void TR::AbsOpStack<Constraint>::print(TR::Compilation* comp)
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
      TR::AbsValue<Constraint> *value = _container[stackSize - i -1 ];
      traceMsg(comp, "S[%d] = ", stackSize - i - 1);
      if (value)
         value->print(comp);
      traceMsg(comp, "\n");
      }

   traceMsg(comp, "<bottom>\n");
   traceMsg(comp, "\n");
   }

template <typename Constraint>
TR::AbsLocalVarArray<Constraint>* TR::AbsLocalVarArray<Constraint>::clone(TR::Region& region)
   {
   TR::AbsLocalVarArray<Constraint>* copy = new (region) TR::AbsLocalVarArray<Constraint>(region);
   for (size_t i = 0; i < _container.size(); i ++)
      {
      copy->_container.push_back(_container[i] ? _container[i]->clone(region) : NULL);
      }
   return copy;
   }

template <typename Constraint>
void TR::AbsLocalVarArray<Constraint>::setToTop()
   {
   for (size_t i = 0; i < _container.size(); i ++)
      {
      if (_container[i])
         _container[i]->setToTop();
      }
   }

template <typename Constraint>
void TR::AbsLocalVarArray<Constraint>::merge(TR::AbsLocalVarArray<Constraint>* other)
   {
   const int32_t otherSize = other->size();
   const int32_t selfSize = size();
   const int32_t mergedSize = std::max(selfSize, otherSize);

   for (int32_t i = 0; i < mergedSize; i++)
      {
      TR::AbsValue<Constraint> *selfValue = i < size() ? at(i) : NULL;
      TR::AbsValue<Constraint> *otherValue = i < other->size() ? other->at(i) : NULL;

      if (!selfValue && !otherValue) 
         {
         continue;
         }
      else if (selfValue && otherValue) 
         {
         TR::AbsValue<Constraint>* mergedVal = selfValue->merge(otherValue);
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

template <typename Constraint>
void TR::AbsLocalVarArray<Constraint>::set(uint32_t index, TR::AbsValue<Constraint> *value)
   {
   if (size() <= index)
      {
      _container.resize(index + 1);
      }
   _container[index] = value;
   }

template <typename Constraint>
TR::AbsValue<Constraint>* TR::AbsLocalVarArray<Constraint>::at(uint32_t index)
   {
   TR_ASSERT_FATAL(index < size(), "Index out of range!");
   return _container[index]; 
   }

template <typename Constraint>
void TR::AbsLocalVarArray<Constraint>::print(TR::Compilation* comp)
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

template class TR::AbsStackMachineState<TR::VPConstraint>;
template class TR::AbsOpStack<TR::VPConstraint>;
template class TR::AbsLocalVarArray<TR::VPConstraint>;
template class TR::AbsArguments<TR::VPConstraint>;
