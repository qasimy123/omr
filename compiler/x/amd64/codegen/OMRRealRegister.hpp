/*******************************************************************************
 * Copyright (c) 2000, 2022 IBM Corp. and others
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

#ifndef OMR_X86_AMD64_REAL_REGISTER_INCL
#define OMR_X86_AMD64_REAL_REGISTER_INCL

/*
 * The following #define and typedef must appear before any #includes in this file
 */
#ifndef OMR_REAL_REGISTER_CONNECTOR
#define OMR_REAL_REGISTER_CONNECTOR
namespace OMR { namespace X86 { namespace AMD64 { class RealRegister; } } }
namespace OMR { typedef OMR::X86::AMD64::RealRegister RealRegisterConnector; }
#else
#error OMR::X86::AMD64::RealRegister expected to be a primary connector, but a OMR connector is already defined
#endif

#include "x/codegen/OMRRealRegister.hpp"

#include <stdint.h>
#include "codegen/RegisterConstants.hpp"

namespace TR { class CodeGenerator; }

namespace OMR
{

namespace X86
{

namespace AMD64
{

class OMR_EXTENSIBLE RealRegister : public OMR::X86::RealRegister
   {
   protected:

   RealRegister(TR::CodeGenerator *cg) : OMR::X86::RealRegister(cg) {}

   RealRegister(TR_RegisterKinds   rk,
                uint16_t           w,
                RegState           s,
                RegNum             ri,
                RegMask            m,
                TR::CodeGenerator *cg) :
      OMR::X86::RealRegister(rk, w, s, ri, m, cg) {}

   public:

   static RegNum rIndex(uint8_t r)
      {
      switch(r)
         {
         case 8:
            return OMR::RealRegister::r8;
         case 9:
            return OMR::RealRegister::r9;
         case 10:
            return OMR::RealRegister::r10;
         case 11:
            return OMR::RealRegister::r11;
         case 12:
            return OMR::RealRegister::r12;
         case 13:
            return OMR::RealRegister::r13;
         case 14:
            return OMR::RealRegister::r14;
         case 15:
            return OMR::RealRegister::r15;
         default:
            TR_ASSERT(false, "rIndex is only valid for registers r8 to r15");
            return OMR::RealRegister::NoReg;
         }
      }

   static RegNum xmmIndex(uint8_t r)
      {
      switch(r)
         {
         case 0:
            return OMR::RealRegister::xmm0;
         case 1:
            return OMR::RealRegister::xmm1;
         case 2:
            return OMR::RealRegister::xmm2;
         case 3:
            return OMR::RealRegister::xmm3;
         case 4:
            return OMR::RealRegister::xmm4;
         case 5:
            return OMR::RealRegister::xmm5;
         case 6:
            return OMR::RealRegister::xmm6;
         case 7:
            return OMR::RealRegister::xmm7;
         case 8:
            return OMR::RealRegister::xmm8;
         case 9:
            return OMR::RealRegister::xmm9;
         case 10:
            return OMR::RealRegister::xmm10;
         case 11:
            return OMR::RealRegister::xmm11;
         case 12:
            return OMR::RealRegister::xmm12;
         case 13:
            return OMR::RealRegister::xmm13;
         case 14:
            return OMR::RealRegister::xmm14;
         case 15:
            return OMR::RealRegister::xmm15;
         default:
            TR_ASSERT(false, "xmmIndex is only valid for registers xmm0 to xmm15");
            return OMR::RealRegister::NoReg;
         }
      }

   static RegMask gprMask(RegNum idx)
      {
      switch(idx)
         {
         case OMR::RealRegister::NoReg:
            return OMR::RealRegister::noRegMask;
         case OMR::RealRegister::eax:
            return OMR::RealRegister::eaxMask;
         case OMR::RealRegister::ebx:
            return OMR::RealRegister::ebxMask;
         case OMR::RealRegister::ecx:
            return OMR::RealRegister::ecxMask;
         case OMR::RealRegister::edx:
            return OMR::RealRegister::edxMask;
         case OMR::RealRegister::edi:
            return OMR::RealRegister::ediMask;
         case OMR::RealRegister::esi:
            return OMR::RealRegister::esiMask;
         case OMR::RealRegister::ebp:
            return OMR::RealRegister::ebpMask;
         case OMR::RealRegister::esp:
            return OMR::RealRegister::espMask;
         case OMR::RealRegister::r8:
            return OMR::RealRegister::r8Mask;
         case OMR::RealRegister::r9:
            return OMR::RealRegister::r9Mask;
         case OMR::RealRegister::r10:
            return OMR::RealRegister::r10Mask;
         case OMR::RealRegister::r11:
            return OMR::RealRegister::r11Mask;
         case OMR::RealRegister::r12:
            return OMR::RealRegister::r12Mask;
         case OMR::RealRegister::r13:
            return OMR::RealRegister::r13Mask;
         case OMR::RealRegister::r14:
            return OMR::RealRegister::r14Mask;
         case OMR::RealRegister::r15:
            return OMR::RealRegister::r15Mask;
         default:
            TR_ASSERT(false, "gprMask is only valid for registers eax to r15");
            return OMR::RealRegister::noRegMask;
         }
      }

   static RegMask fprMask(RegNum idx)
      {
      switch(idx)
         {
         case OMR::RealRegister::NoReg:
            return OMR::RealRegister::noRegMask;
         case OMR::RealRegister::st0:
            return OMR::RealRegister::st0Mask;
         case OMR::RealRegister::st1:
            return OMR::RealRegister::st1Mask;
         case OMR::RealRegister::st2:
            return OMR::RealRegister::st2Mask;
         case OMR::RealRegister::st3:
            return OMR::RealRegister::st3Mask;
         case OMR::RealRegister::st4:
            return OMR::RealRegister::st4Mask;
         case OMR::RealRegister::st5:
            return OMR::RealRegister::st5Mask;
         case OMR::RealRegister::st6:
            return OMR::RealRegister::st6Mask;
         case OMR::RealRegister::st7:
            return OMR::RealRegister::st7Mask;
         default:
            TR_ASSERT(false, "fprMask is only valid for registers st0 to st7");
            return OMR::RealRegister::noRegMask;
         }
      }

   static RegMask xmmrMask(RegNum idx)
      {
      switch(idx)
         {
         case OMR::RealRegister::NoReg:
            return OMR::RealRegister::noRegMask;
         case OMR::RealRegister::xmm0:
            return OMR::RealRegister::xmm0Mask;
         case OMR::RealRegister::xmm1:
            return OMR::RealRegister::xmm1Mask;
         case OMR::RealRegister::xmm2:
            return OMR::RealRegister::xmm2Mask;
         case OMR::RealRegister::xmm3:
            return OMR::RealRegister::xmm3Mask;
         case OMR::RealRegister::xmm4:
            return OMR::RealRegister::xmm4Mask;
         case OMR::RealRegister::xmm5:
            return OMR::RealRegister::xmm5Mask;
         case OMR::RealRegister::xmm6:
            return OMR::RealRegister::xmm6Mask;
         case OMR::RealRegister::xmm7:
            return OMR::RealRegister::xmm7Mask;
         case OMR::RealRegister::xmm8:
            return OMR::RealRegister::xmm8Mask;
         case OMR::RealRegister::xmm9:
            return OMR::RealRegister::xmm9Mask;
         case OMR::RealRegister::xmm10:
            return OMR::RealRegister::xmm10Mask;
         case OMR::RealRegister::xmm11:
            return OMR::RealRegister::xmm11Mask;
         case OMR::RealRegister::xmm12:
            return OMR::RealRegister::xmm12Mask;
         case OMR::RealRegister::xmm13:
            return OMR::RealRegister::xmm13Mask;
         case OMR::RealRegister::xmm14:
            return OMR::RealRegister::xmm14Mask;
         case OMR::RealRegister::xmm15:
            return OMR::RealRegister::xmm15Mask;
         default:
            TR_ASSERT(false, "xmmrMask is only valid for registers xmm0 to xmm15");
            return OMR::RealRegister::noRegMask;
         }
      }

   void setRegisterNumber() { TR_ASSERT(0, "X86 RealRegister doesn't have setRegisterNumber() implementation"); }

   void setRegisterFieldInOpcode(uint8_t *opcodeByte)
      {
      *opcodeByte |= _fullRegisterBinaryEncodings[_registerNumber].id; // reg field is in bits 0-2 of opcode
      }

   /** \brief
   *    Fill vvvv field in a VEX prefix
   *
   *  \param opcodeByte
   *    The address of VEX prefix byte containing vvvv field
   */
   void setRegisterFieldInVEX(uint8_t *opcodeByte)
      {
      *opcodeByte ^= ((_fullRegisterBinaryEncodings[_registerNumber].needsRexForByte << 3) | _fullRegisterBinaryEncodings[_registerNumber].id) << 3; // vvvv is in bits 3-6 of last byte of VEX
      }

   void setRegisterFieldInModRM(uint8_t *modRMByte)
      {
      *modRMByte |= _fullRegisterBinaryEncodings[_registerNumber].id << 3; // reg field is in bits 3-5 of ModRM byte
      }

   void setRMRegisterFieldInModRM(uint8_t *modRMByte)
      {
      *modRMByte |= _fullRegisterBinaryEncodings[_registerNumber].id; // RM field is in bits 0-2 of ModRM byte
      }

   void setBaseRegisterFieldInSIB(uint8_t *SIBByte)
      {
      *SIBByte |= _fullRegisterBinaryEncodings[_registerNumber].id; // base register field is in bits 0-2 of SIB byte
      }

   void setIndexRegisterFieldInSIB(uint8_t *SIBByte)
      {
      *SIBByte |= _fullRegisterBinaryEncodings[_registerNumber].id << 3; // index register field is in bits 3-5 SIB byte
      }

   uint8_t rexBits(uint8_t rxbBits, bool isByteOperand)
      {
      uint8_t result;
      TR_RegisterBinaryEncoding be = _fullRegisterBinaryEncodings[_registerNumber];
      if(be.needsRexPlusRXB)
         // Basic Rex computation
         result = REX | rxbBits;
      else if(isByteOperand && be.needsRexForByte)
         // Special case: To use one of the new AMD64 byte registers, we
         // need a Rex prefix even though it has no r, x, or b bits set
         result = REX;
      else
         // No need for a rex prefix
         result = 0;
      return result;
      }

   // (AMD64: see x86-64 Architecture Programmer's Manual, Volume 3, section 1.7.2.
   bool needsDisp() { return _fullRegisterBinaryEncodings[_registerNumber].needsDisp; }
   bool needsSIB()  { return _fullRegisterBinaryEncodings[_registerNumber].needsSIB;  }

   private:

   // TODO: Consider making this back into a plain old byte for consistency with other platforms.
   static const struct TR_RegisterBinaryEncoding _fullRegisterBinaryEncodings[NumRegisters];
   };

}

}

}

#endif
