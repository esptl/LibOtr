#pragma once

using namespace System;
#include "OtrConnection.h"

namespace Esp {
	namespace ThirdParty
	{
		namespace LibOtr
		{
			public ref class OtrMessage
			{
			private:
				OtrConnection^ _connection;
				String ^_message;
			internal:
				OtrMessage(OtrConnection^ pConnection, String ^pMessage);
			public:
				property String^ OtrMessage::Message {
					String^ get();
				}

				property OtrConnection^ Connection {
					OtrConnection^ get();
				}
			};
		}
	}
}
