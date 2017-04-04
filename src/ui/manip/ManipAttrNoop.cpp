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

#include "ManipAttrNoop.h"
#include "resource/resource.h"
#include "common/Logger.h"

extern HINSTANCE hInstance;

namespace ui {

	/**************************************************************************************************/
	//////////////////////////////////////////* Static area *///////////////////////////////////////////
	/**************************************************************************************************/

	INT_PTR ManipAttrNoop::panelProc(HWND /*hWnd*/, UINT /*msg*/, WPARAM /*wParam*/, LPARAM /*lParam*/) {
		return 0;
	}

	/**************************************************************************************************/
	////////////////////////////////////* Constructors/Destructor */////////////////////////////////////
	/**************************************************************************************************/

	ManipAttrNoop::ManipAttrNoop(MdManip * modelData)
		: mModelData(modelData) {
		assert(mModelData);
	}

	ManipAttrNoop::~ManipAttrNoop() {
		ManipAttrNoop::destroy();
	}

	/**************************************************************************************************/
	///////////////////////////////////////////* Functions *////////////////////////////////////////////
	/**************************************************************************************************/

	void ManipAttrNoop::create(HWND inParent) {
		assert(hInstance);
		assert(inParent);
		mHwnd.setup(CreateDialogParam(hInstance,
									MAKEINTRESOURCE(ROLL_MANIP_NOOP),
									inParent,
									reinterpret_cast<DLGPROC>(panelProc),
									reinterpret_cast<LPARAM>(this)));
		assert(mHwnd);
		if (mHwnd) {
			mHwnd.show(true);
		}
		else {
			LError << WinCode(GetLastError());
		}
	}

	void ManipAttrNoop::destroy() {
		if (mHwnd) {
			BOOL res = DestroyWindow(mHwnd.hwnd());
			if (!res) {
				LError << WinCode(GetLastError());
			}
			mHwnd.release();
		}
	}

	RECT ManipAttrNoop::rect() const {
		RECT r{0,0,0,0};
		if (mHwnd) {
			r = mHwnd.rect();
		}
		return r;
	}

	void ManipAttrNoop::move(const POINT & point) {
		if (mHwnd) {
			mHwnd.move(point);
		}
	}

	/**************************************************************************************************/
	//////////////////////////////////////////* Functions */////////////////////////////////////////////
	/**************************************************************************************************/

	void ManipAttrNoop::setManip(const xobj::AttrManipBase & manip) {
		if (manip.type() != mData.type()) {
			LError << "Incorrect manipulator: " << manip.type().toString();
			return;
		}
		mData = static_cast<const xobj::AttrManipNoop &>(manip);
	}

	/********************************************************************************************************/
	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	/********************************************************************************************************/

}
