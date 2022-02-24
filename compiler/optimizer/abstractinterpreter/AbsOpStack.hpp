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

#ifndef ABS_OP_STACK_INCL
#define ABS_OP_STACK_INCL

#include "env/Region.hpp"
#include <vector>
#include "optimizer/abstractinterpreter/AbsValue.hpp"

namespace TR {

/**
 * Abstract representation of the operand stack.
 */
class AbsOpStack
   {
   public:

   /**
    * @brief Clone an op stack
    * 
    * @param region The memory region where the cloned op stack would like to allocated.
    * @return the op stack cloned.
    */
   TR::AbsOpStack* clone(TR::Region& region) const;

   /**
    * @brief Merge with another operand stack. This is in-place merge.
    *
    * @param other the operand stack to be merged with
    */
   void merge(const TR::AbsOpStack* other);

   /**
    * @brief Push an abstract value to the operand stack.
    * @note the abstract value to be pushed must be non-NULL.
    *
    * @param value the value to be pushed
    */
   void push(TR::AbsValue* value) { _container.push_back(value); }

   /**
    * @brief Peek the top value on the operand stack (stack top; not lattice top).
    * @note the stack must not be empty
    * 
    * @return the abstract value
    */
   TR::AbsValue* peek() const { TR_ASSERT_FATAL(size() > 0, "Peek an empty stack!"); return _container.back(); }

   /**
    * @brief Get and pop a value off of the operand stack.
    * @note the stack must not be empty.
    * 
    * @return the abstract value
    */
   TR::AbsValue* pop();

   bool empty() const { return _container.empty(); }
   size_t size() const { return _container.size(); }
  
   void print(TR::Compilation* comp) const;

   private:
   std::vector<TR::AbsValue*> _container; 
   };

}

#endif
