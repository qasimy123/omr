/*******************************************************************************
 * Copyright (c) 2019, 2019 IBM Corp. and others
 *
 * This program and the accompanying materials are made available under
 * the terms of the Eclipse Public License 2.0 which accompanies this
 * distribution and is available at https://www.eclipse.org/legal/epl-2.0/
 * or the Apache License, Version 2.0 which accompanies this distribution and
 * is available at https://www.apache.org/licenses/LICENSE-2.0.
 *
 * This Source Code may also be made available under the following
 * Secondary Licenses when the conditions for such availability set
 * forth in the Eclipse Public License, v. 2.0 are satisfied: GNU
 * General Public License, version 2 with the GNU Classpath
 * Exception [1] and GNU General Public License, version 2 with the
 * OpenJDK Assembly Exception [2].
 *
 * [1] https://www.gnu.org/software/classpath/license.html
 * [2] http://openjdk.java.net/legal/assembly-exception.html
 *
 * SPDX-License-Identifier: EPL-2.0 OR Apache-2.0 OR GPL-2.0 WITH Classpath-exception-2.0 OR LicenseRef-GPL-2.0 WITH Assembly-exception
 *******************************************************************************/

#include "omrport.h"
#include <unistd.h>
#include <sys/ucontext.h>
#include "omrsignal_context.h"

#define NFPRS 33
#define NGPRS 32

void
fillInUnixSignalInfo(struct OMRPortLibrary *portLibrary, void *contextInfo, struct OMRUnixSignalInfo *signalInfo)
{
	signalInfo->platformSignalInfo.context = (ucontext_t *)contextInfo;
	/* module info is filled on demand */
}

uint32_t
infoForSignal(struct OMRPortLibrary *portLibrary, OMRUnixSignalInfo *info, int32_t index, const char **name, void **value)
{
	*name = "";

	switch (index) {
	case OMRPORT_SIG_SIGNAL_TYPE:
	case 0:
		*name = "J9Generic_Signal_Number";
		*value = &info->portLibrarySignalType;
		return OMRPORT_SIG_VALUE_32;

	case OMRPORT_SIG_SIGNAL_PLATFORM_SIGNAL_TYPE:
	case 1:
		*name = "Signal_Number";
		*value = &info->sigInfo->si_signo;
		return OMRPORT_SIG_VALUE_32;

	case OMRPORT_SIG_SIGNAL_ERROR_VALUE:
	case 2:
		*name = "Error_Value";
		*value = &info->sigInfo->si_errno;
		return OMRPORT_SIG_VALUE_32;

	case OMRPORT_SIG_SIGNAL_CODE:
	case 3:
		*name = "Signal_Code";
		*value = &info->sigInfo->si_code;
		return OMRPORT_SIG_VALUE_32;

	case OMRPORT_SIG_SIGNAL_HANDLER:
	case 4:
		*name = "Handler1";
		*value = &info->handlerAddress;
		return OMRPORT_SIG_VALUE_ADDRESS;

	case 5:
		*name = "Handler2";
		*value = &info->handlerAddress2;
		return OMRPORT_SIG_VALUE_ADDRESS;

	case OMRPORT_SIG_SIGNAL_INACCESSIBLE_ADDRESS:
	case 6:
		/* si_code > 0 indicates that the signal was generated by the kernel */
		if (info->sigInfo->si_code > 0) {
			if ((info->sigInfo->si_signo == SIGBUS) || (info->sigInfo->si_signo == SIGSEGV)) {
				*name = "InaccessibleAddress";
				*value = &info->sigInfo->si_addr;
				return OMRPORT_SIG_VALUE_ADDRESS;
			}
		}
		return OMRPORT_SIG_VALUE_UNDEFINED;

	default:
		return OMRPORT_SIG_VALUE_UNDEFINED;
	}
}

uint32_t
infoForFPR(struct OMRPortLibrary *portLibrary, OMRUnixSignalInfo *info, int32_t index, const char **name, void **value)
{
	struct sigcontext *const context = (struct sigcontext *)&info->platformSignalInfo.context->uc_mcontext;
	const char *n_fpr[NFPRS] = {
			"FT0",
			"FT1",
			"FT2",
			"FT3",
			"FT4",
			"FT5",
			"FT6",
			"FT7",
			"FS0",
			"FS1",
			"FA0",
			"FA1",
			"FA2",
			"FA3",
			"FA4",
			"FA5",
			"FA6",
			"FA7",
			"FS2",
			"FS3",
			"FS4",
			"FS5",
			"FS6",
			"FS7",
			"FS8",
			"FS9",
			"FS10",
			"FS11",
			"FT8",
			"FT9",
			"FT10",
			"FT11",
			"FCSR"
	};

	*name = "";

	if ((index >= 0) && (index < NFPRS)) {
		*name = n_fpr[index];
		*value = &(context->fpregs[index]);
		return OMRPORT_SIG_VALUE_FLOAT_64;
	}
	return OMRPORT_SIG_VALUE_UNDEFINED;
}

uint32_t
infoForGPR(struct OMRPortLibrary *portLibrary, OMRUnixSignalInfo *info, int32_t index, const char **name, void **value)
{
	struct sigcontext *const context = (struct sigcontext *)&info->platformSignalInfo.context->uc_mcontext;
	const char *n_gpr[NGPRS] = {
			"PC",
			"RA",
			"SP",
			"GP",
			"TP",
			"T0",
			"T1",
			"T2",
			"S0",
			"S1",
			"A0",
			"A1",
			"A2",
			"A3",
			"A4",
			"A5",
			"A6",
			"A7",
			"S2",
			"S3",
			"S4",
			"S5",
			"S6",
			"S7",
			"S8",
			"S9",
			"S10",
			"S11",
			"T3",
			"T4",
			"T5",
			"T6"
	};

	*name = "";

	if ((index >= 0) && (index < NGPRS)) {
		*name = n_gpr[index];
		*value = &(context->gregs[index]);
		return OMRPORT_SIG_VALUE_ADDRESS;
	}
	return OMRPORT_SIG_VALUE_UNDEFINED;
}

uint32_t
infoForControl(struct OMRPortLibrary *portLibrary, OMRUnixSignalInfo *info, int32_t index, const char **name, void **value)
{
	struct sigcontext *const context = (struct sigcontext *)&info->platformSignalInfo.context->uc_mcontext;
	*name = "";

	switch (index) {
	case OMRPORT_SIG_CONTROL_PC:
	case 0:
		*name = "PC";
		*value = &(context->gregs[0]);
		return OMRPORT_SIG_VALUE_ADDRESS;
		break;
	case OMRPORT_SIG_CONTROL_SP:
	case 2:
		*name = "SP";
		*value = &(context->gregs[2]);
		return OMRPORT_SIG_VALUE_ADDRESS;
		break;
	default:
		break;
	}

	return OMRPORT_SIG_VALUE_UNDEFINED;
}

uint32_t
infoForModule(struct OMRPortLibrary *portLibrary, OMRUnixSignalInfo *info, int32_t index, const char **name, void **value)
{
	struct sigcontext *const context = (struct sigcontext *)&info->platformSignalInfo.context->uc_mcontext;
	Dl_info *dl_info = &(info->platformSignalInfo.dl_info);
	*name = "";

	int dl_result = dladdr((void *)&(context->gregs[0]), dl_info);

	switch (index) {
	case OMRPORT_SIG_MODULE_NAME:
	case 0:
		*name = "Module";
		if (dl_result) {
			*value = (void *)(dl_info->dli_fname);
			return OMRPORT_SIG_VALUE_STRING;
		}
		break;
	case 1:
		*name = "Module_base_address";
		if (dl_result) {
			*value = (void *)&(dl_info->dli_fbase);
			return OMRPORT_SIG_VALUE_ADDRESS;
		}
		break;
	case 2:
		*name = "Symbol";
		if (dl_result) {
			if (NULL != dl_info->dli_sname) {
				*value = (void *)(dl_info->dli_sname);
				return OMRPORT_SIG_VALUE_STRING;
			}
		}
		break;
	case 3:
		*name = "Symbol_address";
		if (dl_result) {
			*value = (void *)&(dl_info->dli_saddr);
			return OMRPORT_SIG_VALUE_ADDRESS;
		}
		break;
	default:
		break;
	}

	return OMRPORT_SIG_VALUE_UNDEFINED;
}