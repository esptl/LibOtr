#include "stdafx.h"
#include "OtrMessage.h"



using namespace System;
using namespace System::Runtime::InteropServices;


namespace Esp {
	namespace ThirdParty
	{
		namespace LibOtr
		{
			OtrMessage::OtrMessage(OtrConnection^ pConnection, String ^pMessage)
			{
				_connection = pConnection;
				_message = pMessage;
			}

			String^ OtrMessage::Message::get() {
				return _message;
			}

			OtrConnection^ OtrMessage::Connection::get() {
				return _connection;
			}
		}
	}
}
