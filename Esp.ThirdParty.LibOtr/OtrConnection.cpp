#include "stdafx.h"
#include "OtrConnection.h"


using namespace System;

namespace Esp {
	namespace ThirdParty
	{
		namespace LibOtr
		{
			OtrConnection::OtrConnection(ConnContext *pContext, bool pNewConnection)
			{
				_context = pContext;
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

			OtrContact^ OtrConnection::Contact::get() {
				return gcnew OtrContact(gcnew String(_context->protocol), gcnew String(_context->username), gcnew String(_context->accountname));
			}
		}
	}
}

