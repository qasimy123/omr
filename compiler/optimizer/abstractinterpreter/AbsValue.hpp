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

#ifndef ABS_VALUE_INCL
#define ABS_VALUE_INCL

#include "il/OMRDataTypes.hpp"
#include "compile/Compilation.hpp"
#include "optimizer/VPConstraint.hpp"
#include "optimizer/ValuePropagation.hpp"

namespace TR { class AbsVPValue; }

namespace TR {

/**
 * AbsValue is the abstract representation of a 'value'.
 * It is the basic unit used to perform abstract interpretation.
 * This class is an abstract class.
 */
class AbsValue
   {
   public:
   AbsValue(TR::DataType dataType) :
         _dataType(dataType),
         _paramPos(-1),
         _isImplicitParameter(false)
   {}

   /**
    * @brief Clone an abstract value
    * 
    * @param region The region where the cloned value will be allocated on.
    * @return the cloned abstract value
    */
   virtual TR::AbsValue* clone(TR::Region& region)=0;

   /**
    * @brief Merge with another AbsValue. 
    * @note This is an in-place merge. Other should have the exactly same dataType as self. 
    *
    * @param other Another AbsValue to be merged with
    * @return NULL if failing to merge. Self if succeding to merge.
    */
   virtual TR::AbsValue* merge(TR::AbsValue *other)=0;

   /**
    * @brief Check whether the AbsValue is least precise abstract value.
    * @note Top denotes the least precise representation in lattice theory.
    *
    * @return true if it is top. false otherwise
    */
   virtual bool isTop()=0;

   /**
    * @brief Set to the least precise abstract value.
    */
   virtual void setToTop()=0;

   /**
    * @brief Check if the AbsValue is a parameter.
    *
    * @return true if it is a pramater. false if not.
    */
   bool isParameter() { return _paramPos >= 0; }

   /**
    * @brief Check if the AbsValue is 'this' (the implicit parameter.)
    *
    * @return true if it is the implicit parameter. false otherwise.
    */   
   bool isImplicitParameter() { return _paramPos == 0 && _isImplicitParameter; }

   int32_t getParamPosition() { return _paramPos; }
   void setParamPosition(int32_t paramPos) { _paramPos = paramPos; }

   void setImplicitParam() { TR_ASSERT_FATAL(_paramPos == 0, "Cannot set as implicit param"); _isImplicitParameter = true; }

   TR::DataType getDataType() { return _dataType; }

   virtual void print(TR::Compilation* comp)=0;

   protected:

   bool _isImplicitParameter;
   int32_t _paramPos; 
   TR::DataType _dataType;
   };

/**
 * An AbsValue which uses VPConstraint as the constraint.
 */
class AbsVPValue : public AbsValue
   {
   public:
   AbsVPValue(TR::ValuePropagation*vp, TR::VPConstraint* constraint, TR::DataType dataType) :
         TR::AbsValue(dataType),
         _vp(vp),
         _constraint(constraint)
      {}

   TR::VPConstraint* getConstraint() { return _constraint; }

   virtual bool isTop() { return _constraint == NULL; }
   virtual void setToTop() { _constraint = NULL; }

   virtual TR::AbsValue* clone(TR::Region& region);
   virtual TR::AbsValue* merge(TR::AbsValue* other);
   virtual void print(TR::Compilation* comp);

   private:
   TR::ValuePropagation* _vp;
   TR::VPConstraint* _constraint;
   };

}

#endif
