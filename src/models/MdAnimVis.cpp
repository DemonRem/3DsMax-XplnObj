/*
**  Copyright(C) 2017, StepToSky
**
**  Redistribution and use in source and binary forms, with or without
**  modification, are permitted provided that the following conditions are met:
**
**  1.Redistributions of source code must retain the above copyright notice, this
**    list of conditions and the following disclaimer.
**  2.Redistributions in binary form must reproduce the above copyright notice,
**    this list of conditions and the following disclaimer in the documentation
**    and / or other materials provided with the distribution.
**  3.Neither the name of StepToSky nor the names of its contributors
**    may be used to endorse or promote products derived from this software
**    without specific prior written permission.
**
**  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
**  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
**  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
**  DISCLAIMED.IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
**  ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
**  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
**  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
**  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
**  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
**  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
**  Contacts: www.steptosky.com
*/

#include "MdAnimVis.h"
#include "models/io/AnimIO.h"
#include "common/Logger.h"
#include "common/String.h"

/**************************************************************************************************/
////////////////////////////////////* Constructors/Destructor */////////////////////////////////////
/**************************************************************************************************/

MdAnimVis::MdAnimVis() {
	reset();
}

/**************************************************************************************************/
///////////////////////////////////////////* Functions *////////////////////////////////////////////
/**************************************************************************************************/

void MdAnimVis::reset() {
	mEnable = false;
	mKeyList.clear();
}

void MdAnimVis::cloneData(INode * from, INode * to) {
	AnimIO::cloneVisibilityData(from, to);
}

/**************************************************************************************************/
///////////////////////////////////////////* Functions *////////////////////////////////////////////
/**************************************************************************************************/

void MdAnimVis::clearLink() {
	mNode = nullptr;
	reset();
}

bool MdAnimVis::linkNode(INode * node, bool loadData) {
	clearLink();
	if (node == nullptr) {
		return false;
	}

	if (!AnimIO::canApply(node)) {
		return false;
	}

	mNode = node;
	if (loadData) {
		if (!loadFromNode()) {
			clearLink();
			return false;
		}
	}
	return true;
}

/**************************************************************************************************/
//////////////////////////////////////////* Functions */////////////////////////////////////////////
/**************************************************************************************************/

void MdAnimVis::saveToNode(INode * node) const {
	if (node) {
		try {
			AnimIO::saveVisibilityToNode(node, *this);
		}
		catch (std::exception & e) {
			LCritical << "Can't save data to <" << sts::toMbString(node->GetName())
					<< "> object. Reason: <" << e.what() << ">";
		}
	}
}

bool MdAnimVis::loadFromNode(INode * node) {
	if (node) {
		try {
			return AnimIO::loadVisibilityFromNode(node, *this);
		}
		catch (std::exception & e) {
			LCritical << "Can't load data from <" << sts::toMbString(node->GetName())
					<< "> object. Reason: <" << e.what() << ">";
		}
	}
	return false;
}

/**************************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////////////
/**************************************************************************************************/
