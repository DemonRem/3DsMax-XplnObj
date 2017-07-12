#pragma once

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

#include <functional>
#include "Converters/Converterer.h"
#include "additional/utils/SemVersion.h"

namespace presenter {

	/********************************************************************************************************/
	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	/********************************************************************************************************/

	class Export {
	public:

		typedef std::vector<INode*> MainNodes;

		//-------------------------------------------------------------------------

		class IView {
		public:
			virtual ~IView() = default;
			std::function<void(const MSTR & where)> signalSaveLog;
			std::function<bool(const MainNodes & selectedNodes)> signalDoExport;
			std::function<void()> signalDonate;
			std::function<void()> signalCheckForUpdate;
			std::function<void()> signalAbout;

			virtual bool signalShowWindow(const MainNodes & allMainNodes) = 0;
			virtual void signalExportFinished(bool successful) = 0;
			virtual void signalUpdateAvailable(const sts::SemVersion & version) = 0;
		};

		//-------------------------------------------------------------------------

		explicit Export(IView * view);
		virtual ~Export() = default;

		//-------------------------------------------------------------------------

		bool startExport(const TCHAR * inFileName, Interface * inIp, bool suppressPrompts, bool selectedOnly);

		//-------------------------------------------------------------------------

	private:

		IView * mView;

		bool doExport(const MainNodes & nodes);
		MainNodes collectMainNodes() const;
		void printHeaderInfo() const;
		bool checkOptions() const;
		void printUpdateAvailability() const;
		void processNodes();

		MainNodes mNodes;
		MainNodes mSelectedNodes;

		TimeValue mTime = 0;
		Interface * mIp = nullptr;

		const TCHAR * mExpFileName = nullptr;
		bool mSuppressPrompts = false;
		bool mSelectedOnly = false;

		//-------------------------------------------------------------------------

	};

	/********************************************************************************************************/
	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	/********************************************************************************************************/
}
