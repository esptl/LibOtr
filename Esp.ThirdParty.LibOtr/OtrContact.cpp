#include "stdafx.h"
#include "OtrContact.h"



using namespace System;
using namespace System::Runtime::InteropServices;


namespace Esp {
	namespace ThirdParty
	{
		namespace LibOtr
		{
			OtrContact::OtrContact(String^ pProtocol, String^ pUserName, String^pAccountName)
			{
				_accountNameAnsi = Marshal::StringToHGlobalAnsi(pAccountName);
				_accountName = pAccountName;
				_userNameAnsi = Marshal::StringToHGlobalAnsi(pUserName);
				_userName = pUserName;
				_protocolAnsi = Marshal::StringToHGlobalAnsi(pProtocol);
				_protocol = pProtocol;
			}

			OtrContact::~OtrContact()
			{
				Marshal::FreeHGlobal(_accountNameAnsi);
				Marshal::FreeHGlobal(_userNameAnsi);
				Marshal::FreeHGlobal(_protocolAnsi);
			}

			const char *OtrContact::UserNameAnsi::get(){
				return (const char *)_userNameAnsi.ToPointer();
			}
			const char *OtrContact::AccountNameAnsi::get() {
				return (const char *)_accountNameAnsi.ToPointer();
			}
			const char *OtrContact::ProtocolAnsi::get() {
				return (const char *)_protocolAnsi.ToPointer();
			}

			String^ OtrContact::UserName::get(){
				return _userName;
			}
			String ^OtrContact::AccountName::get(){
				return _accountName;
			}
			String^ OtrContact::Protocol::get(){
				return _protocol;
			}

			String^ OtrContact::ToString()
			{
				return _accountName;
			}
		}
	}
}