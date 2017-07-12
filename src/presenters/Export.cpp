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

#pragma warning(push, 0)
#include <max.h>
#pragma warning(pop)

#include "Export.h"
#include "Info.h"
#include "models/MdLinks.h"
#include "ui/Factory.h"
#include "common/Logger.h"
#include "common/NodeVisitor.h"
#include "converters/ConverterUtils.h"
#include "update/UpdateChecker.h"
#include "gup/ObjCommon.h"

namespace presenter {

	/**************************************************************************************************/
	////////////////////////////////////* Constructors/Destructor */////////////////////////////////////
	/**************************************************************************************************/

	Export::Export(IView * view)
		: mView(view) {

		DbgAssert(mView);
		mView->signalDonate = &MdLinks::openDonate;
		mView->signalCheckForUpdate = &MdLinks::openPluginBinary;
		mView->signalAbout = &ui::Factory::showAboutWindow;

		mView->signalSaveLog = [](const MSTR & where) { Logger::instance()->saveLog(where); };
		mView->signalStartExport = std::bind(&Export::startExport, this);
	}

	/**************************************************************************************************/
	//////////////////////////////////////////* Functions */////////////////////////////////////////////
	/**************************************************************************************************/

	Export::MainNodes Export::collectMainNodes() const {
		MainNodes nodes;
		auto fn = [&nodes](INode * n) ->bool {
			if (MainObjParamsWrapper::isMainObj(n)) {
				nodes.emplace_back(n);
			}
			return true;
		};
		NodeVisitor::visitChildrenOf(mIp->GetRootNode(), fn);
		return nodes;
	}

	void Export::printHeaderInfo() const {
		LMessage << "====================== Export obj ======================";
		CLMessage << "File to process: " << sts::toMbString(mExpFileName);
		CLMessage << "Current frame: " << mTime;
	}

	bool Export::checkOptions() const {
		if (mSelectedOnly) {
			const char * msg = "'Selected Only' option is not supported.";
			MessageBoxA(GetActiveWindow(), msg, "Warning", MB_OK | MB_ICONWARNING);
			CLError << msg;
			return false;
		}

		if (mSuppressPrompts) {
			const char * msg = "'Suppress Prompts' option is not supported.";
			MessageBoxA(GetActiveWindow(), msg, "Warning", MB_OK | MB_ICONWARNING);
			CLError << msg;
			return false;
		}

		if (mNodes.empty()) {
			const char * msg = "There are no X-Obj objects found.";
			MessageBoxA(GetActiveWindow(), msg, "Warning", MB_OK | MB_ICONWARNING);
			CLError << msg;
			return false;
		}

		if (mSelectedNodes.empty()) {
			const char * msg = "There are no selected X-Obj objects.";
			MessageBoxA(GetActiveWindow(), msg, "Warning", MB_OK | MB_ICONWARNING);
			CLError << msg;
			return false;
		}
		return true;
	}

	void Export::printUpdateAvailability() const {
		UpdateChecker::Update upd = ObjCommon::instance()->updateInfo();
		if (upd.valid) {
			if (upd.error.empty()) {
				// todo using settings current version
				if (upd.version > SemVersion(XIO_VERSION_MAJOR, XIO_VERSION_MINOR, XIO_VERSION_PATCH)) {
					mView->signalUpdateAvailable(upd.version);
				}
			}
			else {
				CLError << "Error during check update the plugin. The log contains more information. Please, inform the developers.";
			}
		}
	}

	/**************************************************************************************************/
	//////////////////////////////////////////* Functions */////////////////////////////////////////////
	/**************************************************************************************************/

	bool Export::startExport(const MainNodes & selectedNodes) {
		bool result = true;
		mSelectedNodes = selectedNodes;
		mTime = GetCOREInterface()->GetTime();
		//----------------------------------
		printHeaderInfo();
		//----------------------------------
		if (!checkOptions()) {
			mView->signalExportFinished(false);
			return false;
		}
		//----------------------------------
		processNodes();
		//----------------------------------
		printUpdateAvailability();
		//----------------------------------
		mView->signalExportFinished(true);
		return result;
	}

	void Export::processNodes() {
		std::string mainFileName(sts::toMbString(mExpFileName));
		bool produceDerivedFiles = mSelectedNodes.size() > 1;
		CLMessage << "Found " << mNodes.size() << " main object. Selected " << mSelectedNodes.size();
		for (auto currMainNode : mMainNodesCollection) {
			if (!currMainNode.first) {
				continue;
			}
			if (!mLstObjects.isChecked(currMainNode.second)) {
				CLMessage << "Object " << sts::toMbString(currMainNode.first->GetName()) << " is skipped.";
				continue;
			}

			std::string exportFilePath(mainFileName);
			const TCHAR * nodeName = currMainNode.first->GetName();

			if (produceDerivedFiles) {
				if (_tcslen(nodeName) < 1) {
					CLError << "A main object has incorrect name. The name is used for creating the obj file name.";
					continue;
				}
				exportFilePath = sts::toMbString(changeFileName(mExpFileName, nodeName));
				CLMessage << "Create derived file path: " << sts::toMbString(mExpFileName) << " -> " << exportFilePath;
			}

			xobj::ObjMain xMain;
			ConverterUtils::toXTMatrix(ConverterUtils::TOOGL_MTX, xMain.pMatrix);
			MainObjParamsWrapper mwrapper(currMainNode.first, GetCOREInterface()->GetTime(), FOREVER);
			result = mConverterer.toXpln(&mwrapper, xMain) ? TRUE : FALSE;
			if (result == FALSE) {
				CLError << "Export object: \"" << sts::toMbString(nodeName) << "\" is FAILED";
				break;
			}

			std::string signature = sts::MbStrUtils::joinStr(XIO_ORGANIZATION_NAME, " ", XIO_PROJECT_NAME, ": ",
															XIO_VERSION_STRING, "-", XIO_RELEASE_TYPE, "+[", XIO_COMPILE_DATE, "]");
			xMain.pExportOptions.setSignature(signature);

			if (!xMain.exportToFile(exportFilePath)) {
				CLError << "Export object: \"" << sts::toMbString(currMainNode.first->GetName()) << "\" is FAILED";
			}
			else {
				CLMessage << "Export object: \"" << sts::toMbString(currMainNode.first->GetName()) << "\" is OK";
			}
		}
	}

	/**************************************************************************************************/
	////////////////////////////////////////////////////////////////////////////////////////////////////
	/**************************************************************************************************/
}
