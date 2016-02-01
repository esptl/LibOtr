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

			OtrFingerprint^ OtrConnection::OurFingerprint::get()
			{
				return nullptr;
			}

			OtrFingerprint^ OtrConnection::TheirFingerprint::get()
			{
				return gcnew OtrFingerprint(_context->active_fingerprint);
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

