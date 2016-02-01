// Esp.ThirdParty.LibOtr.h

#pragma once

using namespace System;

#include "OtrConnection.h"
#include "OtrMessage.h"
#include "OtrContact.h"
#include "UIOperations.h"

namespace Esp {
	namespace ThirdParty
	{
		namespace LibOtr
		{
			

			public ref class OtrManager
			{
			private:
				IntPtr _accountName;
				IntPtr _protocol;
				IntPtr _keyFile;
				IntPtr _instagFile;
				IntPtr _fingerPrintFile;
				OtrlUserState _state;
				UIOperations^ _operations;
			internal:
				void OtrlMessageInitiateSmp( ConnContext *context, const unsigned char *secret,	size_t secretlen);
				void OtrlMessageInitiateSmpQ(ConnContext *context, const char *question, const unsigned char *secret, size_t secretlen);
				void OtrlMessageRespondSmp(ConnContext *context, const unsigned char *secret, size_t secretlen);
				void OtrlMessageAbortSmp(ConnContext *context);

			public:
				OtrManager(
					IUIOperationProvider^ pProvider,
					System::String^ pAccountName, 
					System::String^pProtocol, 
					System::String^ pBaseFile);
				static OtrManager();
				~OtrManager();

				OtrContact^ GetContact(String^ pUserName, String^ pAccountName);
				OtrConnection^ FindConnection(OtrContact^ pContact);
				OtrMessage^ SendMessage(OtrContact^ pContact, String^ pMessage);
				OtrMessage^ ReceiveMessage(OtrContact^ pContact, String^ pMessage);
				unsigned int GenerateInstag(String ^pAccountName);
				bool GeneratePrivKey(String ^pAccountName);
				void WriteFingerprints();
			};
		}
	}
	
}
