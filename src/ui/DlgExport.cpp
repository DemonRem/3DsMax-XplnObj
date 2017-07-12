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
#include <3dsmaxport.h>
#pragma warning(pop)

#include "DlgExport.h "
#include "common/String.h"
#include "resource/resource.h"
#include "resource/ResHelper.h"
#include "objects/main/MainObjParamsWrapper.h"
#include "gup/ObjCommon.h"

namespace ui {

	/**************************************************************************************************/
	//////////////////////////////////////////* Static area *///////////////////////////////////////////
	/**************************************************************************************************/

	DlgExport * DlgExport::gExportDlg = nullptr;

	/**************************************************************************************************/
	//////////////////////////////////////////* Static area *///////////////////////////////////////////
	/**************************************************************************************************/

	void DlgExport::logCallback(sts::BaseLogger::eType type, const char * msg) {
		if (gExportDlg) {
			if (msg) {
				if (type == sts::BaseLogger::Msg) {
					gExportDlg->mLogText.append("- - - -   ").append(msg).append("\r\n");
				}
				else {
					if (type == sts::BaseLogger::Warning) {
						++gExportDlg->mWarningCount;
					}
					else {
						++gExportDlg->mErrorCount;
					}
					gExportDlg->mLogText.append(sts::BaseLogger::typeAsString(type))
							.append(": ").append(msg).append("\r\n");
				}
				gExportDlg->mEdtLog.setText(gExportDlg->mLogText);
			}
		}
	}

	/**************************************************************************************************/
	//////////////////////////////////////////* Static area *///////////////////////////////////////////
	/**************************************************************************************************/

	INT_PTR CALLBACK DlgExport::callBack(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
		DlgExport * theDlg;
		if (message == WM_INITDIALOG) {
			theDlg = reinterpret_cast<DlgExport*>(lParam);
			theDlg->mDlgMain.setup(hWnd);
			DLSetWindowLongPtr(hWnd, lParam);
		}
		else {
			if ((theDlg = DLGetWindowLongPtr<DlgExport *>(hWnd)) == nullptr) {
				return FALSE;
			}
			if (message == WM_DESTROY) {
				theDlg->mDlgMain.release();
			}
		}

		//------------------------------------------------------

		switch (message) {
			case WM_INITDIALOG: {
				theDlg->InitDlg(hWnd);
				break;
			}
			case WM_COMMAND: {
				switch (LOWORD(wParam)) {
					case BTN_OK: {
						if (!theDlg->mFinished) {
							theDlg->startExport();
						}
						else {
							EndDialog(hWnd, 1);
						}
						break;
					}
					case BTN_SAVE_LOG: {
						theDlg->saveLogRequest();
						break;
					}
					case BTN_SELECT_ALL: {
						theDlg->mLstObjects.checkAll(true);
						break;
					}
					case BTN_UNSELECT_ALL: {
						theDlg->mLstObjects.checkAll(false);
						break;
					}
					case BTN_CANCEL: {
						EndDialog(hWnd, 0);
						break;
					}
					case BTN_DONATE: {
						theDlg->signalDonate();
						break;
					}
					case BTN_CHECK_FOR_UPDATE: {
						theDlg->signalCheckForUpdate();
						break;
					}
					case BTN_ABOUT: {
						theDlg->signalAbout();
						break;
					}
					default: break;
				}
				break;
			}
			case WM_NOTIFY: {
				switch (LOWORD(wParam)) {
					case LST_OBJECTS: {
						LPNMHDR some_item = reinterpret_cast<LPNMHDR>(lParam);
						switch (some_item->code) {
							case LVN_ITEMCHANGED: {
								// Note: There is the loop call when the state is changed from the code while main nodes searching.
								LPNMLISTVIEW pnmv = reinterpret_cast<LPNMLISTVIEW>(lParam);
								if (pnmv->uChanged == LVIF_STATE) {
									theDlg->slotSelObjChanged(pnmv->iItem);
								}
								break;
							}
							case NM_CUSTOMDRAW: {
								return theDlg->mLstObjects.draw(lParam);
							}
							default: break;
						}
						break;
					}
					default: break;
				}
				break;
			}
			case WM_DESTROY: {
				theDlg->DestroyDlg(hWnd);
				break;
			}
			case WM_CLOSE: {
				EndDialog(hWnd, 0);
				break;
			}
			default: break;
		}
		return FALSE;
	}

	/**************************************************************************************************/
	////////////////////////////////////* Constructors/Destructor */////////////////////////////////////
	/**************************************************************************************************/

	DlgExport::DlgExport() {}

	DlgExport::~DlgExport() {}

	/**************************************************************************************************/
	//////////////////////////////////////////* Functions */////////////////////////////////////////////
	/**************************************************************************************************/

	bool DlgExport::signalShowWindow(const presenter::Export::MainNodes & allMainNodes) {
		DbgAssert(!gExportDlg);
		gExportDlg = this;
		mFinished = false;
		mErrorCount = 0;
		mWarningCount = 0;
		mNodes = allMainNodes;

		INT_PTR res = DialogBoxParam(ResHelper::hInstance, MAKEINTRESOURCE(DLG_EXPORT), GetCOREInterface()->GetMAXHWnd(),
									callBack, reinterpret_cast<LPARAM>(this));
		gExportDlg = nullptr;
		return res != 0;
	}

	void DlgExport::signalExportFinished(bool) {
		mFinished = true;
		mBtnSaveLog.enable();
		mBtnOk.setText(_T("Close"));
		mBtnCancel.setText(_T("Close"));
	}

	void DlgExport::signalUpdateAvailable(const sts::SemVersion & version) {
		mRemoteVersion = version;
	}

	/**************************************************************************************************/
	//////////////////////////////////////////* Functions */////////////////////////////////////////////
	/**************************************************************************************************/
	
	void DlgExport::printUpdateAvailable() {
		if (mRemoteVersion) {
			mBtnCheckUpdate.setText("Get update");
			CLWarning << "New version '" << mRemoteVersion.toString(false, false)
				<< "' is available. Please, press the '"
				<< sts::toMbString(mBtnCheckUpdate.text())
				<< "' button to get the new version." << "\r\n"
				<< "\tSee the 'changelog.txt' file there to get the information about the changes.";
		}
	}

	bool DlgExport::startExport() {
		Logger::registerUserConsoleCallback(&logCallback);
		mTime = GetCOREInterface()->GetTime();
		mRemoteVersion.clear();
		//----------------------------------
		presenter::Export::MainNodes selectedNodes;
		for (auto currMainNode : mMainNodesCollection) {
			if (currMainNode.first && mLstObjects.isChecked(currMainNode.second)) {
				selectedNodes.emplace_back(currMainNode.first);
			}
		}
		bool result = signalDoExport(selectedNodes);
		//----------------------------------
		CLMessage << "Export completed with:\r\n\t" << mErrorCount << " errors \r\n\t" << mWarningCount << " warnings";
		printUpdateAvailable();
		Logger::unregisterUserConsoleCallback(&logCallback);
		return result;
	}

	/**************************************************************************************************/
	///////////////////////////////////////////* Functions *////////////////////////////////////////////
	/**************************************************************************************************/

	void DlgExport::InitDlg(HWND hWnd) {
		CenterWindow(mDlgMain.hwnd(), mDlgMain.parent());
		SetWindowTextA(hWnd, "X-Plane Obj Export");
		mHWnd = hWnd;

		mLblVersion.setup(hWnd, LBL_VERSION);
		mBtnCheckUpdate.setup(hWnd, BTN_CHECK_FOR_UPDATE);
		mBtnSaveLog.setup(hWnd, BTN_SAVE_LOG);
		mBtnOk.setup(hWnd, BTN_OK);
		mBtnCancel.setup(hWnd, BTN_CANCEL);
		mBtnAbout.setup(hWnd, BTN_ABOUT);
		mBtnSelAll.setup(hWnd, BTN_SELECT_ALL);
		mBtnUnSelAll.setup(hWnd, BTN_UNSELECT_ALL);
		mBtnDonate.setup(hWnd, BTN_DONATE);
		mEdtLog.setup(hWnd, USER_OUPUT);
		mChkAutoExport.setup(hWnd, CHK_AUTOEXPORT);
		mLstObjects.setup(hWnd, LST_OBJECTS);
		mDlgMain.show();

		mBtnSaveLog.disable();

		sts::Str str(_T("Version: "));
		str.append(sts::toString(Logger::versionShortString()));
		mLblVersion.setText(str);

		mBtnSaveLog.setToolTip(_T("Saves log which can help the developers to determine the problems. Use it for bug reports."));
		mBtnDonate.setToolTip(_T("If you like this plugin please, support the development."));
		mBtnCheckUpdate.setToolTip(_T("Visit the download page for seeing if a new version is available."));
		mChkAutoExport.setToolTip(_T("If it is enabled then the export will be auto-started without pressing the 'Export' button. Value is saved with the scene."));

		for (const auto n : mNodes) {
			MainObjParamsWrapper wrapper(n, mTime, FOREVER);
			int idx = mLstObjects.addItem(n->GetName());
			mMainNodesCollection.emplace_back(NodeCollectionStruct(n, idx));
			mLstObjects.checkItem(idx, wrapper.isExportEnable());
		}

		loadConfigData();
		mDlgMain.show(true);

		if (mChkAutoExport.isChecked()) {
			startExport();
		}
	}

	void DlgExport::DestroyDlg(HWND /*hWnd*/) {
		saveConfigData();
		mLblVersion.release();
		mBtnCheckUpdate.release();
		mBtnSaveLog.release();
		mBtnOk.release();
		mBtnCancel.release();
		mBtnAbout.release();
		mBtnSelAll.release();
		mBtnUnSelAll.release();
		mBtnDonate.release();
		mChkAutoExport.release();
		mLstObjects.release();
	}

	/**************************************************************************************************/
	//////////////////////////////////////////* Functions */////////////////////////////////////////////
	/**************************************************************************************************/

	void DlgExport::saveConfigData() {
		ObjCommon * cmn = ObjCommon::instance();
		if (cmn) {
			Settings & conf = cmn->pSettings;
			conf.beginGroup(TOTEXT(DlgExport));
			conf.setValue(TOTEXT(mChkAutoExport), mChkAutoExport.isChecked());
			conf.endGroup();
		}
	}

	void DlgExport::loadConfigData() {
		ObjCommon * cmn = ObjCommon::instance();
		if (cmn) {
			Settings & conf = cmn->pSettings;
			conf.beginGroup(TOTEXT(DlgExport));
			mChkAutoExport.setState(conf.value(TOTEXT(mChkAutoExport), false));
			conf.endGroup();
		}
	}

	/**************************************************************************************************/
	//////////////////////////////////////////* Functions */////////////////////////////////////////////
	/**************************************************************************************************/

	void DlgExport::slotSelObjChanged(int idx) {
		for (auto & i : mMainNodesCollection) {
			if (i.second == idx) {
				MainObjParamsWrapper wrapper(i.first, mTime, FOREVER);
				wrapper.setExportEnable(mLstObjects.isChecked(idx));
				return;
			}
		}
	}

	void DlgExport::saveLogRequest() const {
		Interface8 * ip = GetCOREInterface8();
		MSTR fileName;
		MSTR initialDir(_T("C:\\Users\\%USERNAME%\\Documents"));
		FilterList extensionList;
		extensionList.Append(_T("Log files(*.log)"));
		extensionList.Append(_T("*.log"));
		bool res = ip->DoMaxSaveAsDialog(GetCOREInterface()->GetMAXHWnd(), _T("Log saving"), fileName, initialDir, extensionList);
		if (res) {
			signalSaveLog(fileName);
		}
	}

	/**************************************************************************************************/
	////////////////////////////////////////////////////////////////////////////////////////////////////
	/**************************************************************************************************/
}
