/*******************************************************************************
 * Copyright (c) 1991, 2016 IBM Corp. and others
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

#include "omrcfg.h"
#include "omr.h"

#include "OMRVMThreadListIterator.hpp"

/**
 * @return the next VM Thread in the list
 * @return NULL if there are no more threads in the list
 */
OMR_VMThread *
GC_OMRVMThreadListIterator::nextOMRVMThread()
{
	if(_omrVMThread) {
		OMR_VMThread *currentOMRVMThread;
		currentOMRVMThread = _omrVMThread;
		_omrVMThread = _omrVMThread->_linkNext;
		if(_omrVMThread == _initialOMRVMThread) {
			_omrVMThread = NULL;
		}
		return currentOMRVMThread;
	}
	return NULL;
}
