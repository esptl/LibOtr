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
		}
	}
}
