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

#pragma once

#pragma warning(push, 0)
#include <max.h>
#include <iparamb2.h>
#include <iparamm2.h>
#pragma warning(pop)

#include "objects/MouseCallback.h"

/**************************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////////////
/**************************************************************************************************/

class SmokeObject : public HelperObject {
public:

	SmokeObject();
	virtual ~SmokeObject();

	void DeleteThis() override { delete this; }

	//-------------------------------------------------------------------------

	CreateMouseCallBack * GetCreateMouseCallBack() override;

	//-------------------------------------------------------------------------

	IOResult Load(ILoad * iload) override;

	//-------------------------------------------------------------------------

	void BeginEditParams(IObjParam * ip, ULONG flags, Animatable * prev) override;
	void EndEditParams(IObjParam * ip, ULONG flags, Animatable * next) override;

	//-------------------------------------------------------------------------

	ObjectState Eval(TimeValue /*t*/) override;
	Object * ConvertToType(TimeValue t, Class_ID obtype) override;
	int CanConvertToType(Class_ID obtype) override;

	//-------------------------------------------------------------------------

	int Display(TimeValue t, INode * inode, ViewExp * vpt, int flags) override;
	static void GetMat(TimeValue t, INode * inode, ViewExp * vpt, Matrix3 & tm);
	int HitTest(TimeValue t, INode * inode, int type, int crossing, int flags, IPoint2 * p, ViewExp * vpt) override;
	int UsesWireColor() override;

	void GetLocalBoundBox(TimeValue t, INode * inode, ViewExp * vpt, Box3 & box) override;
	void GetWorldBoundBox(TimeValue t, INode * inode, ViewExp * vpt, Box3 & box) override;

	Animatable * SubAnim(int i) override;
	TSTR SubAnimName(int i) override;

	void makeIcon();

	//-------------------------------------------------------------------------

#if MAX_VERSION_MAJOR > 14
	const MCHAR * GetObjectName() override;
#else
	TCHAR * GetObjectName() override;
#endif

	Class_ID ClassID() override;
	SClass_ID SuperClassID() override;
	void GetClassName(TSTR & s) override;
	RefTargetHandle Clone(RemapDir & remap) override;
	void InitNodeName(TSTR & s) override;

	//-------------------------------------------------------------------------

	int GetParamBlockIndex(int id) override;
	int NumParamBlocks() override;
	IParamBlock2 * GetParamBlock(int i) override;
	IParamBlock2 * GetParamBlockByID(BlockID id) override;

	int NumRefs() override;
	RefTargetHandle GetReference(int i) override;
	void SetReference(int i, RefTargetHandle rtarg) override;

	//-------------------------------------------------------------------------

#if MAX_VERSION_MAJOR > 16
	RefResult NotifyRefChanged(const Interval & changeInt, RefTargetHandle hTarget,
								PartID & partID, RefMessage message, BOOL propagate) override;
#else
	RefResult NotifyRefChanged(Interval changeInt, RefTargetHandle hTarget,
								PartID & partID, RefMessage message) override;
#endif

	//-------------------------------------------------------------------------

private:

	IParamBlock2 * mParamsPb = nullptr;
	IParamBlock2 * mDisplayPb = nullptr;

	ClassDesc2 * mDesc;
	IObjParam * mIp = nullptr;
	Point3 mObjColor;
	Mesh mIconMesh;
	float mLastIconScale = 1.0f;

	static SmokeObject * mEditOb;
	static MouseCallback mMouseCallback;

};

/**************************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////////////
/**************************************************************************************************/