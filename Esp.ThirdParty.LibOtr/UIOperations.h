#pragma once
#include "OtrConnection.h"
#include "OtrContact.h"
#include "OtrFingerprint.h"
using namespace System;
using namespace System::Runtime::InteropServices;

namespace Esp {
	namespace ThirdParty
	{
		namespace LibOtr
		{
			public enum class OtrErrorMessage : int { Unknown, EncryptionError, MessageNotInPrivate, MessageUnreadable, MessageMalformed };
			public enum class OtrSmpEvent : int { AskForSecret, AskForAnswer, Cheated, InProgress, Success, Failure, Abort };
			public enum class OtrMessageEvent : int {
				EncryptionRequired,
				EncryptionError,
				ConnectionEnded,
				SetupError,
				MessageReflected,
				MessageResent,
				ReceivedNotInPrivate,
				ReceivedUnreadable,
				ReceivedMalformed,
				LogHeartBeatReceived,
				LogHeartBeatSent,
				ReceivedGeneralError,
				ReceivedUnencrypted,
				ReceivedUnrecognised,
				OtherInstance
			};

			public enum class OtrConvertType { Sending, Recieving };
			public enum class OtrPolicy { Never, Default, Manual, Always };
			public interface class IUIOperationProvider {
				OtrPolicy OnGetPolicy(OtrConnection^ pConnection);
				void OnCreatePrivKey(String^ pAccountname);
				int OnLoggedIn(OtrContact^ pContact);
				int OnInjectMessage(OtrContact^ pContact, String^ pMessage);
				void OnUpdateContextList();
				void OnNewFingerPrint(OtrContact^ pContact, OtrFingerprint^ pFingerprint);
				void OnWriteFingerPrints();
				void OnGoneSecure(OtrConnection^ pConnection);
				void OnGoneInsecure(OtrConnection^ pConnection);
				void OnStillSecure(OtrConnection^ pConnection, bool pIsReply);
				int OnGetMaxMessageSize(OtrConnection^ pConnection);
				String^ OnGetAccountName(String ^pAccount);
				void OnReceivedSymKey(OtrConnection^ pConnection, unsigned int pUse, array<System::Byte>^ pUseData, array<System::Byte>^ pSymKey);
				String ^OnGetErrorMessage(OtrConnection^ pConnection, OtrErrorMessage pErrorCode);
				String ^OnGetResentMessagePrefix(OtrConnection^ pConnection);
				void OnSmpEvent(OtrConnection^ pConnection, OtrSmpEvent pEvent, String^ pQuestion, int pPercent);
				void OnMessageEvent(OtrConnection^ pConnection, OtrMessageEvent pEvent, String^ pMessage, int pGCryptErrorCode);
				void OnCreateInstag(String^ pAccountname);
				String ^OnConvertMessage(OtrConnection^ pConnection, OtrConvertType pCoversionType, String^ pMessage);

			



			};


	
			ref class UIOperations
			{
			private:
				IUIOperationProvider^ _provider;
				OtrManager^ _manager;
				GCHandle _gcGetPolicy;
				GCHandle _gcCreatePrivKey;
				GCHandle _gcIsLoggedIn;
				GCHandle _gcInjectMessage;
				GCHandle _gcUpdateContextList;
				GCHandle _gcNewFingerPrint;
				GCHandle _gcWriteFingerprints;
				GCHandle _gcGoneSecure;
				GCHandle _gcGoneInsecure;
				GCHandle _gcStillSecure;
				GCHandle _gcMaxMessageSize;
				GCHandle _gcAccountName;
				GCHandle _gcAccountNameFree;
				GCHandle _gcReceivedSymKey;
				GCHandle _gcOtrErrorMessage;
				GCHandle _gcOtrErrorMessageFree;
				GCHandle _gcResentPrefixMessage;
				GCHandle _gcResentPrefixMessageFree;
				GCHandle _gcHandleSmpEvent;
				GCHandle _gcHandleMsgEvent;
				GCHandle _gcCreateInstag;
				GCHandle _gcConvertMsg;
				GCHandle _gcConvertMsgFree;
				GCHandle _gcTimer;

				

				OtrlPolicy HookGetPolicy(void *pOpdata, ConnContext *pContext);
				void HookCreatePrivkey(void *opdata, const char *accountname, const char *protocol);
				int HookIsLoggedIn(void *opdata, const char *accountname, const char *protocol, const char *recipient);
				void HookInjectMessage(void *opdata, const char *accountname, const char *protocol, const char *recipient, const char *message);
				void HookUpdateContextList(void *opdata);
				void HookNewFingerprint(void *opdata, OtrlUserState us,
					const char *accountname, const char *protocol,
					const char *username, unsigned char fingerprint[20]);
				void UIOperations::HookWriteFingerprints(void *opdata);
				void UIOperations::HookGoneSecure(void *opdata, ConnContext *context);
				void UIOperations::HookGoneInsecure(void *opdata, ConnContext *context);
				void HookStillSecure(void *opdata, ConnContext *context, int is_reply);
				int HookMaxMessageSize(void *opdata, ConnContext *context);
				const char* HookGetAccountName(void *opdata, const char *account, const char *protocol);
				void HookAccountNameFree(void *opdata, const char *account_name);
				void  HookReceivedSymKey(void *opdata, ConnContext *context, unsigned int use, const unsigned char *usedata,
					size_t usedatalen, const unsigned char *symkey);
				const char* HookOtrErrorMessage(void *opdata, ConnContext *context, OtrlErrorCode err_code);
				void  HookOtrErrorMessageFree(void *opdata, const char *err_msg);
				const char* HookResentMsgPrefix(void *opdata, ConnContext *context);
				void  HookResentMsgPrefixFree(void *opdata, const char *prefix);
				void HookHandleSmpEvent(void *opdata, OtrlSMPEvent smp_event,
				ConnContext *context, unsigned short progress_percent, char *question);
				void HookHandleMsgEvent(void *opdata, OtrlMessageEvent msg_event, ConnContext *context, const char *message, gcry_error_t err);
				void HookCreateInstag(void *opdata, const char *accountname, const char *protocol);
				void HookConvertMsg(void *opdata, ConnContext *context, OtrlConvertType convert_type, char ** dest, const char *src);
				void HookConvertFree(void *opdata, ConnContext *context, char *dest);
				void HookTimerControl(void *opdata, unsigned int interval);

			internal:
				OtrlMessageAppOps* _ops;

			public:
				UIOperations(OtrManager^ pManager, IUIOperationProvider^ pProvider);
				~UIOperations();
			};
		}
	}
}

