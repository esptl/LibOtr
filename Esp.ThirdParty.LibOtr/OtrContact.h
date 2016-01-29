#pragma once

using namespace System;

namespace Esp {
	namespace ThirdParty
	{
		namespace LibOtr
		{
			public ref class OtrContact {
			private:
				IntPtr _userNameAnsi;
				IntPtr _accountNameAnsi;
				IntPtr _protocolAnsi;
				String ^_userName;
				String ^_accountName;
				String ^_protocol;
			internal:
				property const char *UserNameAnsi {
					const char *get();
				}
				property const char *AccountNameAnsi {
					const char *get();
				}
				property const char *ProtocolAnsi {
					const char *get();
				}
				
			public:
				OtrContact(String^ pProtocol, String^ pUserName, String^pAccountName);
				~OtrContact();

				property String^ UserName {
					String ^get();
				}
				property String ^AccountName {
					String ^get();
				}
				property String^ Protocol {
					String^ get();
				}

				virtual String^ ToString() override;
			};

		}
	}
}
