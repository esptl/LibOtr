#pragma once

#include "OtrContact.h"
#include "OtrFingerprint.h"
//#include "OtrManager.h"

using namespace System;


namespace Esp {
	namespace ThirdParty
	{
		namespace LibOtr
		{
		
			public enum class OtrConnectionState : char { Unencrypted, Encrpyted, Verfied, Terminated };
			ref class OtrManager;
			public ref class OtrConnection {

			private:
				ConnContext* _context;
				OtrManager^ _manager;
				bool _newConnection;
			internal:
				OtrConnection(OtrManager^ pManager,ConnContext *pContext, bool pNewConnection);

			public:
				~OtrConnection();

				property OtrConnectionState State {
					OtrConnectionState get();
				}

				property int OurInstag {
					int get();
					void set(int);
				}

				property int TheirInstag {
					int get();
					void set(int);
				}

				property System::String^ OurFingerprint {
					System::String^ get();
				}

				property OtrFingerprint^ TheirFingerprint {
					OtrFingerprint^ get();
				}

				property OtrManager^ Manager {
					OtrManager^ get();
				}

				property OtrContact^ Contact {
					OtrContact^ get();
				}

				void InitiateSmp(String^ pQuestion, String^ pAnswer);
				void InitiateSmp(String^ pSecret);
				void AbortSmp();
				void Finish();
				void AnswerSmp(String^ pSecret);
			};
		}
	}
}

