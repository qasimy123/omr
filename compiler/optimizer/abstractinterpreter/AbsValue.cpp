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

#include "optimizer/abstractinterpreter/AbsValue.hpp"

TR::AbsValue* TR::AbsVPValue::clone(TR::Region& region)
   {
   TR::AbsVPValue* copy = new (region) TR::AbsVPValue(_vp, _constraint, _dataType);
   copy->setParamPosition(_paramPos);

   if (_isImplicitParameter)
      copy->setImplicitParam();
   
   return copy;
   }

TR::AbsValue* TR::AbsVPValue::merge(TR::AbsValue *other)
   {
   TR_ASSERT_FATAL(other, "Cannot merge with a NULL AbsValue");

   if (other->getDataType() != _dataType) 
      return NULL;

   if (!_constraint)
      return this;

   if (_paramPos != other->getParamPosition()) 
      _paramPos = -1;

   if (_isImplicitParameter && !other->isImplicitParameter())
      _isImplicitParameter = false;

   TR::AbsVPValue* otherVPValue = static_cast<TR::AbsVPValue*>(other);

   if (otherVPValue->isTop()) 
      {
      setToTop();
      return this;
      }

   TR::VPConstraint *mergedConstraint = _constraint->merge(otherVPValue->getConstraint(), _vp);

   if (mergedConstraint) //mergedConstaint can be VPMergedIntConstraint or VPMergedLongConstraint. Turn them into VPIntRange or VPLongRange to make things easier.
      {
      if (mergedConstraint->asMergedIntConstraints())
         mergedConstraint = TR::VPIntRange::create(_vp, mergedConstraint->getLowInt(), mergedConstraint->getHighInt());
      else if (mergedConstraint->asMergedLongConstraints())
         mergedConstraint = TR::VPLongRange::create(_vp, mergedConstraint->getLowLong(), mergedConstraint->getHighLong());
      }

   _constraint = mergedConstraint;
   return this;
   }

void TR::AbsVPValue::print(TR::Compilation* comp)    
   {
   traceMsg(comp, "AbsValue: Type: %s ", TR::DataType::getName(_dataType));
   
   if (_constraint)
      {
      traceMsg(comp, "Constraint: ");
      _constraint->print(_vp);
      }
   else 
      {
      traceMsg(comp, "TOP (unknown) ");
      }

   traceMsg(comp, " param position: %d", _paramPos);

   if (_isImplicitParameter)
      traceMsg(comp, " {implicit param} ");
   }

