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

#ifndef ABS_STACK_MACHINE_STATE_INCL
#define ABS_STACK_MACHINE_STATE_INCL

#include "env/Region.hpp"
#include "infra/deque.hpp"
#include "optimizer/abstractinterpreter/AbsValue.hpp"
#include "optimizer/abstractinterpreter/AbsState.hpp"

namespace TR {
/**
 * Abstract representation of the operand stack.
 * Constraint is the type of the constraint that abstract value would like to use.
 */
class AbsOpStack
   {
   public:
   explicit AbsOpStack(TR::Region& region) :
         _container(region)
      {}

   /**
    * @brief Clone an op stack
    * 
    * @param region The memory region where the cloned op stack would like to allocated on.
    * @return the op stack cloned.
    */
   TR::AbsOpStack* clone(TR::Region& region);

   /**
    * @brief Merge with another operand stack. This is in-place merge.
    *
    * @param other the operand stack to be merged with
    */
   void merge(TR::AbsOpStack* other);

   /**
    * @brief Push an abstract value to the operand stack.
    * @note the abstract value to be pushed must be non-NULL.
    *
    * @param value the value to be pushed
    */
   void push(TR::AbsValue* value) { TR_ASSERT_FATAL(value, "Push a NULL value"); _container.push_back(value); }

   /**
    * @brief Peek the operand stack
    *
    * @return the abstract value
    */
   TR::AbsValue* peek() { TR_ASSERT_FATAL(size() > 0, "Peek an empty stack!"); return _container.back(); }

   /**
    * @brief Get and pop the value
    *
    * @return the abstract value
    */
   TR::AbsValue* pop();

   /**
    * @brief Set all the abstract values in this operand stack to the least precise ones
    */
   void setToTop();

   bool empty()  { return _container.empty(); }
   size_t size()  { return _container.size(); }
  
   void print(TR::Compilation* comp);

   private:
   TR::deque<TR::AbsValue*, TR::Region&> _container; 
   };

/**
.* Abstract representation of the local variable array.
 */
class AbsLocalVarArray
   {
   public:
   explicit AbsLocalVarArray(TR::Region &region) :
         _container(region)
      {}

   /**
    * @brief Clone an local var array
    * 
    * @param region The memory region where the cloned local var array would like to be allocated on
    * @return the cloned local var array
    */
   TR::AbsLocalVarArray *clone(TR::Region& region);

   /**
    * @brief Merge with another abstract local var array. This is in-place merge.
    *
    * @param other The array to be merged with.
    * @param vp value propagation
    */
   void merge(TR::AbsLocalVarArray* other);
   
   /**
    * @brief Get the abstract value at index i.
    *
    * @param i the local var array index
    * @return the abstract value
    */
   TR::AbsValue *at(uint32_t i);

   /**
    * @brief Set the abstract value at index i.
    *
    * @param i the local var array index
    * @param value the abstract value to be set
    */
   void set(uint32_t i, TR::AbsValue* value);

   /**
    * @brief Set all the abstract values in this local var array to the least precise ones.
    */
   void setToTop();

   size_t size() { return _container.size(); }
   void print(TR::Compilation* comp);

   private:
   TR::deque<TR::AbsValue*, TR::Region&> _container;
   };

/**
.* Abstract representation of the program state of the stack machine. 
 */
class AbsStackMachineState : public AbsState
   {
   public:
   explicit AbsStackMachineState(TR::Region &region);

   /**
    * @brief clone an stack machine state
    * 
    * @param region The memory region where the state would like to be allocated on
    * @return the cloned state
    * 
    */
   virtual TR::AbsState* clone(TR::Region& region);

   /**
    * @brief Set an abstract value at index i of the local variable array in this state.
    *
    * @param i the local variable array index
    * @param value the value to be set
    */
   void set(uint32_t i, TR::AbsValue* value) { _array->set(i, value); }

   /**
    * @brief Get the AbsValue at index i of the local variable array in this state.
    *
    * @param i the local variable array index
    * @return the abstract value
    */
   TR::AbsValue *at(uint32_t i) { return _array->at(i); }

   /**
    * @brief Push an AbsValue to the operand stack in this state.
    *
    * @param value the value to be pushed
    */
   void push(TR::AbsValue* value) { _stack->push(value); }

   /**
    * @brief Get and pop the AbsValue of the operand stack in this state.
    * 
    * @return the abstract value
    */
   TR::AbsValue* pop() { return _stack->pop(); }

   /**
    * @brief Peek the operand stack in this state.
    *
    * @return the abstract value
    */
   TR::AbsValue* peek() { return _stack->peek();  }

   /**
    * @brief Merge with another state. This is in-place merge.
    *
    * @param other another state to be merged with
    */
   virtual void merge(TR::AbsState* other);
   
   /**
    * @brief Set all the abstract values in this state to the least precise ones.
    */
   virtual void setToTop() { _stack->setToTop(); _array->setToTop(); }

   size_t getStackSize() {  return _stack->size();  }
   size_t getArraySize() {  return _array->size();  }
   
   virtual void print(TR::Compilation* comp);

   private:
   TR::AbsLocalVarArray* _array;
   TR::AbsOpStack* _stack;
   };
}


#endif
