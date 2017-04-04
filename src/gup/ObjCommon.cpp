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

#include "ObjCommon.h"
#include "ui/toolFrame/ToolFrame.h"

#ifndef IO_SAVE_CAST
#if MAX_VERSION_MAJOR > 14
#define IO_SAVE_CAST const BYTE
#else
#define IO_SAVE_CAST const void
#endif
#endif

/**************************************************************************************************/
/////////////////////////////////////////* Static area *////////////////////////////////////////////
/**************************************************************************************************/

extern HINSTANCE hInstance;

class ObjCommonClassDesc : public ClassDesc2 {
public:

	int IsPublic() override { return TRUE; }
	void * Create(BOOL /*loading = FALSE*/) override { return reinterpret_cast<void *>(ObjCommon::instance()); }
	HINSTANCE HInstance() override { return hInstance; }

	SClass_ID SuperClassID() override { return GUP_CLASS_ID; }
	Class_ID ClassID() override { return COMMON_CLASS_ID; }

	const TCHAR * ClassName() override { return _T("X-Common"); }
	const TCHAR * Category() override { return _T("X-Plane Obj coomon class"); }
	const TCHAR * InternalName() override { return _T("xCommonObject"); }

};

static ObjCommonClassDesc ObjGupDesc;

ClassDesc2 * GetObjCommonDesc() {
	return &ObjGupDesc;
}

/**************************************************************************************************/
////////////////////////////////////* Constructors/Destructor */////////////////////////////////////
/**************************************************************************************************/

ObjCommon::ObjCommon() {
	mToolFrame = nullptr;
	mConfig = Config::instance();
	mCloneNodeChunk = nullptr;
}

ObjCommon::~ObjCommon() {
	mConfig->free();
}

/**************************************************************************************************/
///////////////////////////////////////////* Functions *////////////////////////////////////////////
/**************************************************************************************************/

DWORD ObjCommon::Start() {
	mCloneNodeChunk = new CloneNodeChunk();
	mToolFrame = ui::ToolFrame::instance();
	mToolFrame->create();

	//gDlgExportMain.show(gDlgExportMainData);
	//mDlgHasNewVersion.show(mMdUpdateProgram);

	return GUPRESULT_KEEP;
}

void ObjCommon::Stop() {
	mToolFrame->free();
	delete mCloneNodeChunk;
}

/**************************************************************************************************/
///////////////////////////////////////////* Functions *////////////////////////////////////////////
/**************************************************************************************************/

DWORD_PTR ObjCommon::Control(DWORD /*param*/) {
	return 0;
}

/**************************************************************************************************/
///////////////////////////////////////////* Functions *////////////////////////////////////////////
/**************************************************************************************************/

IOResult ObjCommon::Save(ISave * /*isave*/) {
	// TODO needs a try to save/load plugin version to the root node
	return IO_OK;
}

IOResult ObjCommon::Load(ILoad * /*iload*/) {
	return IO_OK;
}

/**************************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////////////
/**************************************************************************************************/
