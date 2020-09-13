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

#include "optimizer/VPConstraint.hpp"
#include "il/OMRDataTypes.hpp"
#include "optimizer/ValuePropagation.hpp"

/**
 * AbsValue is the abstract representation of a 'value'.
 * It is the basic unit used to perform abstract interpretation.
 */
class AbsValue
   {
   public:
   explicit AbsValue(TR::VPConstraint *constraint, TR::DataType dataType);

   /**
    * @brief Copy constructor
    */
   AbsValue(AbsValue* other);

   /**
    * @brief Merge with another AbsValue. 
    * @note This is an in-place merge. Other should have the exactly same dataType as self. 
    *
    * @param other Another AbsValue to be merged with
    * @param vp Value propagation 
    * @return NULL if failing to merge. Self if succeding to merge.
    */
   AbsValue* merge(AbsValue *other, TR::ValuePropagation *vp);

   /**
    * @brief Check whether the AbsValue is least precise abstract value.
    * @note Top denotes the least precise representation in lattice theory.
    *
    * @return true if it is top. false otherwise
    */
   bool isTop() { return _constraint == NULL; }

   /**
    * @brief Set to the least precise abstract value.
    */
   void setToTop() { _constraint = NULL; }

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

   /**
    * A set of methods for creating different kinds of AbsValue.
    * TR::ValuePropagation is required as a parameter since we are using TR::VPConstraint as the lattice.
    * Though it has nothing to do with the Value Propagation optimization.
    */
   static AbsValue* create(TR::Region&region, TR::VPConstraint *constraint, TR::DataType dataType);

   static AbsValue* create(TR::Region& region, AbsValue* other);
   
   static AbsValue* createClassObject(TR::Region& region, TR::ValuePropagation* vp, TR_OpaqueClassBlock* opaqueClass, bool mustBeNonNull);

   static AbsValue* createNullObject(TR::Region& region, TR::ValuePropagation* vp);
   static AbsValue* createArrayObject(TR::Region& region, TR::ValuePropagation* vp, TR_OpaqueClassBlock* arrayClass, bool mustBeNonNull, int32_t lengthLow, int32_t lengthHigh, int32_t elementSize);
   
   static AbsValue* createStringObject(TR::Region& region, TR::ValuePropagation* vp, TR::SymbolReference* symRef);

   static AbsValue* createIntConst(TR::Region& region, TR::ValuePropagation* vp, int32_t value);
   static AbsValue* createLongConst(TR::Region& region, TR::ValuePropagation* vp, int64_t value);
   static AbsValue* createIntRange(TR::Region& region, TR::ValuePropagation* vp, int32_t low, int32_t high);
   static AbsValue* createLongRange(TR::Region& region, TR::ValuePropagation* vp, int64_t low, int64_t high);

   static AbsValue* createTopInt(TR::Region& region);
   static AbsValue* createTopLong(TR::Region& region);

   static AbsValue* createTopObject(TR::Region& region);

   static AbsValue* createTopDouble(TR::Region& region);
   static AbsValue* createTopFloat(TR::Region& region);

   bool isNullObject() { return _constraint && _constraint->isNullObject(); }
   bool isNonNullObject() { return _constraint && _constraint->isNonNullObject(); }

   bool isArrayObject() { return _constraint && _constraint->asClass() && _constraint->getArrayInfo(); }
   bool isClassObject() { return _constraint && _constraint->asClass() && !_constraint->getArrayInfo(); }
   bool isStringObject() { return _constraint && _constraint->asConstString(); }
   bool isObject() { return isArrayObject() || isClassObject() || isStringObject(); }

   bool isIntConst() { return _constraint && _constraint->asIntConst(); }
   bool isIntRange() { return _constraint && _constraint->asIntRange(); }
   bool isInt() { return _constraint && _constraint->asIntConstraint(); }

   bool isLongConst() { return _constraint && _constraint->asLongConst(); }
   bool isLongRange() { return _constraint && _constraint->asLongRange(); }
   bool isLong() { return _constraint && _constraint->asLongConstraint(); }

   int32_t getParamPosition() { return _paramPos; }
   void setParamPosition(int32_t paramPos) { _paramPos = paramPos; }

   void setImplicitParam() { TR_ASSERT_FATAL(_paramPos == 0, "Cannot set as implicit param"); _isImplicitParameter = true; }

   TR::DataType getDataType() { return _dataType; }
   
   TR::VPConstraint* getConstraint() { return _constraint; }

   void print(TR::Compilation* comp, TR::ValuePropagation *vp);

   private:

   bool _isImplicitParameter;
   int32_t _paramPos; 
   
   TR::VPConstraint* _constraint;
   TR::DataType _dataType;
   };


#endif
