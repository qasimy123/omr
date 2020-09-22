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

#ifndef ABS_STATE_INCL
#define ABS_STATE_INCL

#include "infra/deque.hpp"
#include "env/Region.hpp"
#include "optimizer/abstractinterpreter/AbsValue.hpp"

namespace TR {

/**
 * Abstract representation of the program state.
 */
class AbsState
   {
   };

/**
 * Abstract representation of the the arguments.
 * This is used for method invocation to pass arguments from caller to callee method during abstract interpretation.
 */
template <typename Constraint>
class AbsArguments
   {
   public:
   AbsArguments(uint32_t size, TR::Region& region) {_args = new (region) TR::AbsValue<Constraint>*[size]; _size = size; }

   /**
    * @brief Set an argument at position i
    * 
    * @param i The argument position
    * @param arg The argument
    */
   void set(uint32_t i, TR::AbsValue<Constraint>* arg) { TR_ASSERT_FATAL(i < _size, "Index out of range"); _args[i] = arg; }

   /**
    * @brief Get the arg at position i
    * 
    * @param i The argument position
    * @return The argument
    */
   TR::AbsValue<Constraint>* at(uint32_t i) { TR_ASSERT_FATAL(i < _size, "Index out of range"); return _args[i]; }

   uint32_t size() { return _size; }
   
   private:
   uint32_t _size;
   TR::AbsValue<Constraint>** _args;
   };
}

#endif
