#pragma once

#include "OtrContact.h"

using namespace System;


namespace Esp {
	namespace ThirdParty
	{
		namespace LibOtr
		{


			public enum class OtrConnectionState : char { Unencrypted, Encrpyted, Verfied, Terminated };
			public ref class OtrConnection {

			private:
				ConnContext* _context;
				bool _newConnection;
			internal:
				OtrConnection(ConnContext *pContext, bool pNewConnection);

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

				property OtrContact^ Contact {
					OtrContact^ get();
				}
			};
		}
	}
}

