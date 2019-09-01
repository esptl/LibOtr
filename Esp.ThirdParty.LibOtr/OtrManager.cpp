// This is the main DLL file.

#include "stdafx.h"

#include "OtrManager.h"
using namespace System;
using namespace System::IO;
using namespace System::Runtime::InteropServices;



namespace Esp {
	namespace ThirdParty
	{
		namespace LibOtr
		{
			static OtrManager::OtrManager()
			{
				OTRL_INIT;
			}

			OtrManager::OtrManager(IUIOperationProvider^ pProvider, System::String^ pAccountName, System::String^pProtocol, System::String^ pBaseFile)
			{
				_accountName = Marshal::StringToHGlobalAnsi(pAccountName);
				_protocol = Marshal::StringToHGlobalAnsi(pProtocol);
				_keyFile = Marshal::StringToHGlobalAnsi(pBaseFile + ".key");
				_fingerPrintFile = Marshal::StringToHGlobalAnsi(pBaseFile + ".fingers");
				_instagFile = Marshal::StringToHGlobalAnsi(pBaseFile + ".instag");
				_state = otrl_userstate_create();
				_operations = gcnew UIOperations(this,pProvider);
				try
				{
					gcry_error_t err = 0;
					if (File::Exists(pBaseFile + ".key"))
						err = otrl_privkey_read(_state, (char *)_keyFile.ToPointer());
					if (err)
						throw gcnew System::Exception("Failed to read/generate priv key");
					EncryptFileA((char *)_keyFile.ToPointer());

					if (!File::Exists(pBaseFile + ".fingers"))
						err = otrl_privkey_write_fingerprints(_state, (char *)_fingerPrintFile.ToPointer());
					else
						err = otrl_privkey_read_fingerprints(_state, (char *)_fingerPrintFile.ToPointer(), nullptr, nullptr);

					if (err)
						throw gcnew System::Exception("Failed to read/generate finger prints");

					if (File::Exists(pBaseFile + ".instag"))
						err = otrl_instag_read(_state, (char *)_instagFile.ToPointer());

					if (err)
						throw gcnew System::Exception("Failed to read/generate instag file");


				}
				catch (Exception^) {

					otrl_userstate_free(_state);
					Marshal::FreeHGlobal(_accountName);
					Marshal::FreeHGlobal(_protocol);
					Marshal::FreeHGlobal(_keyFile);
					Marshal::FreeHGlobal(_fingerPrintFile);
					throw;
				}

			}

			OtrManager::~OtrManager()
			{
				otrl_privkey_write_fingerprints(_state, (char *)_fingerPrintFile.ToPointer());
				otrl_userstate_free(_state);
				Marshal::FreeHGlobal(_accountName);
				Marshal::FreeHGlobal(_protocol);
				Marshal::FreeHGlobal(_keyFile);
				Marshal::FreeHGlobal(_fingerPrintFile);
			}

			OtrContact^ OtrManager::GetContact(String^ pUserName, String^ pAccountName)
			{
				return gcnew OtrContact(gcnew String((char *)_protocol.ToPointer()), pUserName, pAccountName);
			}


			OtrMessage^ OtrManager::SendMessage(OtrContact^ pContact, String^ pMessage)
			{
				ConnContext *context;

				char *newMessage;
				auto message = (const char *)Marshal::StringToHGlobalAnsi(pMessage).ToPointer();
				try {
					auto err = otrl_message_sending(_state, _operations->_ops,
						nullptr, pContact->AccountNameAnsi, pContact->ProtocolAnsi,
						pContact->UserNameAnsi, OTRL_INSTAG_BEST, message, nullptr, &newMessage, OTRL_FRAGMENT_SEND_ALL_BUT_LAST,
						&context,
						nullptr,
						nullptr);
					if (newMessage != nullptr)
					{
						String^ newMsg = gcnew String(newMessage);
						otrl_message_free(newMessage);
						return gcnew OtrMessage(gcnew OtrConnection(this,context, false), newMsg);
					}
					if (err)
						return nullptr;
					return gcnew OtrMessage(gcnew OtrConnection(this,context, false), pMessage);
				}
				finally {
					Marshal::FreeHGlobal((IntPtr)(void*)message);
				}
			}

			OtrMessage^ OtrManager::ReceiveMessage(OtrContact^ pContact, String^ pMessage)
			{
				ConnContext *context = nullptr;

				char *newMessage = nullptr;
				auto message = (const char *)Marshal::StringToHGlobalAnsi(pMessage).ToPointer();
				try {
					auto err = otrl_message_receiving(_state, _operations->_ops,
						nullptr, pContact->AccountNameAnsi, pContact->ProtocolAnsi,
						pContact->UserNameAnsi, message, &newMessage,
						nullptr,
						&context,
						nullptr,
						nullptr);
					if (newMessage != nullptr)
					{
						String^ newMsg = gcnew String(newMessage);
						otrl_message_free(newMessage);
						return gcnew OtrMessage(gcnew OtrConnection(this, context, false), newMsg);
					}
					if (err)
						return nullptr;
					return gcnew OtrMessage(gcnew OtrConnection(this, context, false), pMessage);
				}
				finally {
					Marshal::FreeHGlobal((IntPtr)(void*)message);
				}
			}

			OtrConnection^ OtrManager::FindConnection(OtrContact^ pContact)
			{

				int added = 0;
				auto context = otrl_context_find(_state,
					pContact->UserNameAnsi,
					pContact->AccountNameAnsi,
					pContact->ProtocolAnsi, OTRL_INSTAG_BEST, 1, &added, nullptr, nullptr);
				if (context == nullptr)
					return nullptr;
				return gcnew OtrConnection(this, context, added == 1);
			}

			unsigned int OtrManager::GenerateInstag(String^ pAccountName)
			{
				auto account = (const char *)Marshal::StringToHGlobalAnsi(pAccountName).ToPointer();
				try {
					otrl_instag_generate(_state, (const char *)_instagFile.ToPointer(),account , (const char *)_protocol.ToPointer());
					auto instag = otrl_instag_find(_state,  account, (const char *)_protocol.ToPointer());
					
					return instag==nullptr? 0 : instag->instag;
				}
				finally {
					Marshal::FreeHGlobal((IntPtr)(void*)account);
				}
			}


			bool OtrManager::GeneratePrivKey(String^ pAccountName)
			{
				auto account = (const char *)Marshal::StringToHGlobalAnsi(pAccountName).ToPointer();
				try {
					return !otrl_privkey_generate(_state, (const char *)_keyFile.ToPointer(), account, (const char *)_protocol.ToPointer());			
				}
				finally {
					Marshal::FreeHGlobal((IntPtr)(void*)account);
				}
			}

			void OtrManager::WriteFingerprints()
			{
				otrl_privkey_write_fingerprints(_state,(const char*)_fingerPrintFile.ToPointer());
			}

			void OtrManager::OtrlMessageInitiateSmp(ConnContext *context, const unsigned char *secret, size_t secretlen)
			{
				otrl_message_initiate_smp(_state, _operations->_ops, nullptr, context, secret, secretlen);
			}

			void OtrManager::OtrlMessageInitiateSmpQ(ConnContext *context, const char *question, const unsigned char *secret, size_t secretlen)
			{
				otrl_message_initiate_smp_q(_state, _operations->_ops, nullptr, context, question, secret, secretlen);
			}

			void OtrManager::OtrlMessageRespondSmp(ConnContext *context, const unsigned char *secret, size_t secretlen)
			{
				otrl_message_respond_smp(_state, _operations->_ops, nullptr, context,  secret, secretlen);
			}

			void OtrManager::OtrlMessageAbortSmp(ConnContext *context)
			{
				otrl_message_abort_smp(_state, _operations->_ops, nullptr, context);
			}

			void OtrManager::Poll(IntPtr pData)
			{
				otrl_message_poll(_state, _operations->_ops, pData.ToPointer());
			}


			OtrlUserState OtrManager::State::get() {
				return _state;
			}

			IntPtr OtrManager::AccountName::get() {
				return _accountName;
			}

			IntPtr OtrManager::Protocol::get() {
				return _protocol;
			}
			UIOperations^ OtrManager::Operations::get()
			{
				return _operations;
			}

		}
	}
}