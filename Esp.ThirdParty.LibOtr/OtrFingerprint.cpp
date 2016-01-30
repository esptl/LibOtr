#include "stdafx.h"
#include "OtrFingerprint.h"


namespace Esp {
	namespace ThirdParty
	{

		namespace LibOtr
		{
			OtrFingerprint::OtrFingerprint(Fingerprint *pFingerprint)
			{
				_fingerprint = pFingerprint;
			}

			OtrFingerprintTrust OtrFingerprint::Trust::get() {
				if(otrl_context_is_fingerprint_trusted)
				{
					if (strcmp(_fingerprint->trust, "smp") == 0)
						return OtrFingerprintTrust::Smp;
					return OtrFingerprintTrust::Verified;
				}
				return OtrFingerprintTrust::Untrusted;				
			}

			void OtrFingerprint::Trust::set(OtrFingerprintTrust pTrust) {
				switch(pTrust)
				{
					case OtrFingerprintTrust::Verified :
						otrl_context_set_trust(_fingerprint, "verified");
						break;
					case OtrFingerprintTrust::Smp:
						otrl_context_set_trust(_fingerprint, "smp");
						break;
					default:
						otrl_context_set_trust(_fingerprint, NULL);
						break;
				}				
			}

			array<System::Byte>^ OtrFingerprint::Bytes::get() {
				if (!_fingerprint->fingerprint) return nullptr;

				auto result = gcnew array<System::Byte>(20);
				for (auto i = 0; i < 20; i++)
					result[i] = _fingerprint->fingerprint[i];
				return result;
			}

			String^ OtrFingerprint::ToString()
			{
				if (!_fingerprint->fingerprint) return "NULL";

				auto result = gcnew Text::StringBuilder();
				for (auto i = 0; i < 20; i++)
				{
					if (i % 4 == 0 && i)
						result->Append(" ");
					result->AppendFormat("{0:x2}", _fingerprint->fingerprint[i]);
				}
				return result->ToString();
			}
		}
	}
}
