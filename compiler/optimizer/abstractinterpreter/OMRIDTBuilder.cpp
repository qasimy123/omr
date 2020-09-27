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

#include "optimizer/abstractinterpreter/IDTBuilder.hpp"

OMR::IDTBuilder::IDTBuilder(TR::ResolvedMethodSymbol* symbol, int32_t budget, TR::Region& region, TR::Compilation* comp, TR_InlinerBase* inliner) :
      _rootSymbol(symbol),
      _rootBudget(budget),
      _region(region),
      _comp(comp),
      _inliner(inliner),
      _idt(NULL)
   {}

TR::IDTBuilder* OMR::IDTBuilder::self()
   {
   return static_cast<TR::IDTBuilder*>(this);
   }

TR::IDT* OMR::IDTBuilder::buildIDT()
   {
   bool traceBIIDTGen = comp()->getOption(TR_TraceBIIDTGen);

   if (traceBIIDTGen)
      traceMsg(comp(), "\n+ IDTBuilder: Start building IDT |\n\n");

   TR_ResolvedMethod* rootMethod = _rootSymbol->getResolvedMethod();
   TR_ByteCodeInfo bcInfo;

   //This is just a fake callsite to make ECS work.
   TR_CallSite *rootCallSite = new (region()) TR_CallSite(
                                                rootMethod,
                                                NULL,
                                                NULL,
                                                NULL,
                                                NULL,
                                                rootMethod->containingClass(),
                                                0,
                                                0,
                                                rootMethod,
                                                _rootSymbol,
                                                _rootSymbol->getMethodKind() == TR::MethodSymbol::Kinds::Virtual || _rootSymbol->getMethodKind() == TR::MethodSymbol::Kinds::Interface ,
                                                _rootSymbol->getMethodKind() == TR::MethodSymbol::Kinds::Interface,
                                                bcInfo,
                                                comp());
                                                 
   TR_CallTarget *rootCallTarget = new (region()) TR_CallTarget(
                                    rootCallSite,
                                    _rootSymbol, 
                                    rootMethod, 
                                    NULL, 
                                    rootMethod->containingClass(), 
                                    NULL);
   
   //Initialize IDT
   _idt = new (region()) TR::IDT(region(), rootCallTarget, _rootSymbol, _rootBudget, comp());
   
   TR::IDTNode* root = _idt->getRoot();

   //generate the CFG for root call target 
   TR::CFG* cfg = self()->generateControlFlowGraph(rootCallTarget);

   if (!cfg) //Fail to generate a CFG
      return _idt;

   //add the IDT decendants
   self()->buildIDT2(root, NULL, -1, _rootBudget, NULL);

   if (traceBIIDTGen)
      traceMsg(comp(), "\n+ IDTBuilder: Finish building TR::IDT |\n");

   return _idt;
   }

void OMR::IDTBuilder::buildIDT2(TR::IDTNode* node, TR::AbsArguments* arguments, int32_t callerIndex, int32_t budget, TR_CallStack* callStack)
   {
   TR::ResolvedMethodSymbol* symbol = node->getResolvedMethodSymbol();
   TR_ResolvedMethod* method = node->getResolvedMethod();

   TR_CallStack* nextCallStack = new (region()) TR_CallStack(comp(), symbol, method, callStack, budget, true);

   TR::IDTBuilderVisitor visitor(self(), node, nextCallStack);

   self()->performAbstractInterpretation(node, visitor, arguments, callerIndex);
   }

void OMR::IDTBuilder::addNodesToIDT(TR::IDTNode*parent, int32_t callerIndex, TR_CallSite* callSite, float callRatio, TR::AbsArguments* arguments, TR_CallStack* callStack)
   {
   bool traceBIIDTGen = self()->comp()->getOption(TR_TraceBIIDTGen);

   if (callSite == NULL || callSite->_initialCalleeMethod == NULL)
      {
      if (traceBIIDTGen)
         traceMsg(comp(), "Do not have a callsite. Don't add\n");
      return;
      }

   if (callRatio * parent->getRootCallRatio() * 100 < 25)
      {
      if (traceBIIDTGen)
         traceMsg(comp(), "Root call ratio < 0.25. Don't add\n");
      return;
      }

   callSite->findCallSiteTarget(callStack, getInliner()); //Find all call targets

   // eliminate call targets that are not inlinable according to the policy
   // thus they won't be added to TR::IDT 
   self()->getInliner()->applyPolicyToTargets(callStack, callSite); 

   if (callSite->numTargets() == 0) 
      {
      if (traceBIIDTGen)
         traceMsg(comp(), "Do not have a call target. Don't add\n");
      return;
      }
      
   for (int32_t i = 0 ; i < callSite->numTargets(); i++)
      {
      TR_CallTarget* callTarget = callSite->getTarget(i);
      
      int32_t remainingBudget = parent->getBudget() - callTarget->_calleeMethod->maxBytecodeIndex();

      if (remainingBudget < 0 ) // no budget remains
         {
         if (traceBIIDTGen)
            traceMsg(comp(), "No budget left. Don't add\n");
         continue;
         }

      bool isRecursiveCall = callStack->isAnywhereOnTheStack(callTarget->_calleeMethod, 1);

      if (isRecursiveCall) //Stop for recursive call
         {
         if (traceBIIDTGen)
            traceMsg(comp(), "Recursive call. Don't add\n");  
         continue;
         }
         
      // The actual symbol for the callTarget->_calleeMethod.
      TR::ResolvedMethodSymbol* calleeMethodSymbol = TR::ResolvedMethodSymbol::create(comp()->trHeapMemory(), callTarget->_calleeMethod, comp());

      // generate the CFG of this call target and set the block frequencies. 
      TR::CFG* cfg = self()->generateControlFlowGraph(callTarget);

      if (!cfg)
         {
         if (traceBIIDTGen)
            traceMsg(self()->comp(), "Fail to generate a CFG. Don't add\n");  
         continue;
         }
         
      if (traceBIIDTGen)
         traceMsg(self()->comp(), "+ IDTBuilder: Adding a child Node: %s for TR::IDTNode: %s\n", calleeMethodSymbol->signature(comp()->trMemory()), parent->getName(comp()->trMemory()));

      TR::IDTNode* child = parent->addChild(
                              _idt->getNextGlobalIDTNodeIndex(),
                              callTarget,
                              calleeMethodSymbol,
                              callSite->_byteCodeIndex,
                              callRatio,
                              _idt->getRegion()
                              );
      
      _idt->increaseGlobalIDTNodeIndex();

      if (!self()->comp()->incInlineDepth(calleeMethodSymbol, callSite->_bcInfo, callSite->_cpIndex, NULL, !callSite->isIndirectCall(), 0))
         continue;
      
      // Build the IDT recursively
      self()->buildIDT2(child, arguments, callerIndex + 1, child->getBudget(), callStack);

      self()->comp()->decInlineDepth(true); 
      }
   }

void TR::IDTBuilderVisitor::visitCallSite(TR_CallSite* callSite, int32_t callerIndex, TR::Block* callBlock, TR::AbsArguments* arguments)
   {
   if (callBlock->getFrequency() < 6 || callBlock->isCold() || callBlock->isSuperCold())
      return;

   float callRatio = (float)callBlock->getFrequency() / (float)_idtNode->getCallTarget()->_cfg->getStart()->asBlock()->getFrequency();
   _idtBuilder->addNodesToIDT(_idtNode, callerIndex, callSite, callRatio, arguments, _callStack);
   }