#include "stdafx.h"
#include "OtrConnection.h"
#include "OtrManager.h"

using namespace System;
using namespace System::Runtime::InteropServices;

namespace Esp {
	namespace ThirdParty
	{
		namespace LibOtr
		{
			OtrConnection::OtrConnection(OtrManager^ pManager, ConnContext *pContext, bool pNewConnection)
			{
				_context = pContext;
				_manager = pManager;
				_newConnection = pNewConnection;			
			}


			OtrConnection::~OtrConnection()
			{

			}

			OtrConnectionState OtrConnection::State::get() {
				
				if (_context->msgstate == OTRL_MSGSTATE_ENCRYPTED)
					if (_context->active_fingerprint->trust &&
						_context->active_fingerprint->trust[0])
						return OtrConnectionState::Verfied;
					else
						return OtrConnectionState::Encrpyted;
				else if (_context->msgstate == OTRL_MSGSTATE_FINISHED)
					return OtrConnectionState::Terminated;

				return OtrConnectionState::Unencrypted;
			}

			int OtrConnection::OurInstag::get()
			{
				return _context->our_instance;
			}
			void OtrConnection::OurInstag::set(int pValue)
			{
				_context->our_instance = pValue;
			}

			int OtrConnection::TheirInstag::get()
			{
				return _context->their_instance;
			}

			void OtrConnection::TheirInstag::set(int pValue)
			{
				_context->their_instance = pValue;
			}

			System::String^ OtrConnection::OurFingerprint::get()
			{
				char ourHash[OTRL_PRIVKEY_FPRINT_HUMAN_LEN];

				memset(ourHash, 0, sizeof(ourHash));
				strncpy((char *)ourHash, ("missing"), OTRL_PRIVKEY_FPRINT_HUMAN_LEN-1);
				char * fingerprint = otrl_privkey_fingerprint(_manager->State, ourHash, _context->accountname,  (char *)_manager->Protocol.ToPointer());
				System::Diagnostics::Debug::WriteLine(L"AccName: " + gcnew System::String(_context->accountname));
				System::Diagnostics::Debug::WriteLine(L"Protocol: " + gcnew System::String((char *)_manager->Protocol.ToPointer()));
				if (!fingerprint) return gcnew System::String("NULL FINGERPRINT");
				return gcnew System::String(fingerprint);
			
			}

			OtrFingerprint^ OtrConnection::TheirFingerprint::get()
			{
				return gcnew OtrFingerprint(_context->active_fingerprint);
			}

			OtrManager^ OtrConnection::Manager::get() {
				return _manager;
			}

			OtrContact^ OtrConnection::Contact::get() {
				return gcnew OtrContact(gcnew String(_context->protocol), gcnew String(_context->username), gcnew String(_context->accountname));
			}

			void OtrConnection::InitiateSmp(String^ pQuestion, String^ pAnswer)
			{
				auto question = (const char *)Marshal::StringToHGlobalAnsi(pQuestion).ToPointer();
				auto secret = (const char *)Marshal::StringToHGlobalAnsi(pAnswer).ToPointer();

				try {
					_manager->OtrlMessageInitiateSmpQ(_context, question, (unsigned char*)secret, (int)strlen(secret));
				}
				finally {
					Marshal::FreeHGlobal((IntPtr)(void*)question);
					Marshal::FreeHGlobal((IntPtr)(void*)secret);
				}
			}

			void OtrConnection::InitiateSmp(String^ pSecret)
			{
				auto secret = (const char *)Marshal::StringToHGlobalAnsi(pSecret).ToPointer();
			
				try {
					_manager->OtrlMessageInitiateSmp(_context, (unsigned char*)secret, (int)strlen(secret));
				}
				finally {
					Marshal::FreeHGlobal((IntPtr)(void*)secret);
				}				
			}

			void OtrConnection::AbortSmp()
			{
				_manager->OtrlMessageAbortSmp(_context);
			}

			void OtrConnection::Finish()
			{
				char *userName = strdup(_context->username);
				char *accountName = strdup(_context->accountname);
				int added = 0;
				
				/*_context = otrl_context_find(_manager->State,
					userName,
					accountName,
					(char *)_manager->Protocol.ToPointer(), OTRL_INSTAG_BEST, 1, &added, nullptr, nullptr);*/
				otrl_message_disconnect_all_instances(_manager->State, _manager->Operations->_ops, nullptr, accountName, (char *)_manager->Protocol.ToPointer(), userName);
				

				_context = otrl_context_find(_manager->State,
					userName,
					accountName,
					(char *)_manager->Protocol.ToPointer(), OTRL_INSTAG_BEST, 1, &added, nullptr, nullptr);
			
				
			}

			void OtrConnection::AnswerSmp(String^ pSecret)
			{
				auto secret = (const char *)Marshal::StringToHGlobalAnsi(pSecret).ToPointer();

				try {
					_manager->OtrlMessageRespondSmp(_context, (unsigned char*)secret, (int)strlen(secret));
				}
				finally {
					Marshal::FreeHGlobal((IntPtr)(void*)secret);
				}
			}
		}
	}
}

