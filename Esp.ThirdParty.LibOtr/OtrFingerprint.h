#include "stdafx.h"
#pragma once


using namespace System;


namespace Esp {
	namespace ThirdParty
	{
		namespace LibOtr
		{
			public enum class OtrFingerprintTrust : char { Untrusted, Verified, Smp };

			public ref class OtrFingerprint
			{
			private:
				Fingerprint *_fingerprint;
			internal:
				OtrFingerprint(Fingerprint *pFingerPrint);

			public:
				property OtrFingerprintTrust Trust {
					OtrFingerprintTrust get();
					void set(OtrFingerprintTrust pTrust);
				}
			};
		}
	}
}

