#include "stdafx.h"
#include "UIOperations.h"


using namespace System::Runtime::InteropServices;

namespace Esp {
	namespace ThirdParty
	{
		namespace LibOtr
		{
			delegate OtrlPolicy DelegateGetPolicy(void*, ConnContext*);
			delegate void DelegateCreatePrivkey(void *, const char *, const char *);
			delegate int DelegateIsLoggedIn(void *, const char *, const char *, const char *);
			delegate void DelegateInjectMessage(void *, const char *, const char *, const char *, const char *);
			delegate void DelegateNotify(void *);
			delegate void DelegateConnectionNotify(void *, ConnContext*);
			delegate const char * DelegateConnectionString(void *, ConnContext*);
			delegate void DelegateStillSecure(void *, ConnContext*,int);
			delegate int DelegateMaxMessageSize(void *, ConnContext*);
			delegate void DelegateNewFingerPrint(void *, OtrlUserState ,const char *, const char *,const char *, unsigned char [20]);
			delegate const char *DelegateAccountName(void *, const char *, const char *);
			delegate void DelegateStringFree(void *, const char *);
			delegate void DelegateReceivedSymKey(void *, ConnContext *, unsigned int, const unsigned char *, size_t, const unsigned char *);
			delegate const char* DelegateOtrErrorMessage(void *, ConnContext *, OtrlErrorCode);
			delegate void DelegateHandleSmpEvent(void *, OtrlSMPEvent,ConnContext *, unsigned short, char *);
			delegate void DelegateHandleMsgEvent(void *, OtrlMessageEvent, ConnContext *, const char *, gcry_error_t);
			delegate void DelegateCreateInstag(void *, const char *, const char *);
			delegate void DelegateConvertMsg(void *, ConnContext *,	OtrlConvertType, char ** , const char *);
			delegate void DelegateConvertMsgFree(void *, ConnContext *, char *);
			delegate void DelegateTimer(void *, unsigned int);

			typedef OtrlPolicy(__stdcall *policyhook)(void *opdata, ConnContext *context);
			typedef void(__stdcall *createprivhook)(void *, const char *, const char *);
			typedef int(__stdcall *isloggedinhook)(void *, const char *, const char *, const char *);
			typedef void(__stdcall *injectmessagehook)(void *, const char *, const char *, const char *, const char *);
			typedef    void (__stdcall *notifyhook) (void *);
			typedef void(__stdcall *connectionnotifyhook)(void *,ConnContext*);
			typedef const char *(__stdcall *connectionstringhook)(void *, ConnContext*);
			typedef void(__stdcall *stillsecurehook)(void *, ConnContext*,int);
			typedef int(__stdcall *maxmessagesizehook)(void *, ConnContext*);
			typedef void(__stdcall *newfingerprinthook)(void *, OtrlUserState, const char *, const char *, const char *, unsigned char[20]);
			typedef const char*(__stdcall *accountnamehook)(void *, const char *, const char *);
			typedef void(__stdcall *stringfreehook)(void *, const char *);
			typedef void(__stdcall *receivedsymkeyhook)(void *, ConnContext *, unsigned int, const unsigned char *, size_t, const unsigned char *);
			typedef const char *(__stdcall *otrerrormessagehook)(void *, ConnContext *, OtrlErrorCode);
			typedef void (__stdcall *handlesmpeventhook)(void *, OtrlSMPEvent, ConnContext *, unsigned short, char *);
			typedef void(__stdcall *handlemsgeventhook)(void *, OtrlMessageEvent, ConnContext *, const char *, gcry_error_t);
			typedef void(__stdcall *createinstaghook)(void *, const char *, const char *);
			typedef void(__stdcall *convertmsghook)(void *, ConnContext *, OtrlConvertType, char **, const char *);
			typedef void(__stdcall *convertmsgfreehook)(void *, ConnContext *, char *);
			typedef void(__stdcall *timerhook)(void *, unsigned int);

			UIOperations::UIOperations(OtrManager^ pManager,IUIOperationProvider^ pProvider)
			{
				_provider = pProvider;
				_manager = pManager;

				_ops = (OtrlMessageAppOps*)Marshal::AllocHGlobal(sizeof(OtrlMessageAppOps)).ToPointer();

				auto getPolicy = gcnew DelegateGetPolicy(this,&UIOperations::HookGetPolicy);
				_gcGetPolicy = GCHandle::Alloc(getPolicy);
				IntPtr ipGetPolicy = Marshal::GetFunctionPointerForDelegate(getPolicy);
				_ops->policy = (policyhook)ipGetPolicy.ToPointer();
				
				auto createPrivkey = gcnew DelegateCreatePrivkey(this, &UIOperations::HookCreatePrivkey);
				_gcCreatePrivKey = GCHandle::Alloc(createPrivkey);
				IntPtr ipCreatePrivkey = Marshal::GetFunctionPointerForDelegate(createPrivkey);
				_ops->create_privkey = (createprivhook)ipCreatePrivkey.ToPointer();

				auto isLoggedIn = gcnew DelegateIsLoggedIn(this, &UIOperations::HookIsLoggedIn);
				_gcIsLoggedIn = GCHandle::Alloc(isLoggedIn);
				IntPtr ipIsLoggedIn = Marshal::GetFunctionPointerForDelegate(isLoggedIn);
				_ops->is_logged_in = (isloggedinhook)ipIsLoggedIn.ToPointer();

				auto injectMessage = gcnew DelegateInjectMessage(this, &UIOperations::HookInjectMessage);
				_gcInjectMessage = GCHandle::Alloc(injectMessage);
				IntPtr ipInjectMessage = Marshal::GetFunctionPointerForDelegate(injectMessage);
				_ops->inject_message = (injectmessagehook)ipInjectMessage.ToPointer();

				auto updateContextList = gcnew DelegateNotify(this, &UIOperations::HookUpdateContextList);
				_gcUpdateContextList = GCHandle::Alloc(updateContextList);
				IntPtr ipUpdateContextList = Marshal::GetFunctionPointerForDelegate(updateContextList	);
				_ops->update_context_list = (notifyhook)ipUpdateContextList.ToPointer();

				auto newFingerPrint = gcnew DelegateNewFingerPrint(this, &UIOperations::HookNewFingerprint);
				_gcNewFingerPrint = GCHandle::Alloc(newFingerPrint);
				IntPtr ipNewFingerPrint = Marshal::GetFunctionPointerForDelegate(newFingerPrint);
				_ops->new_fingerprint = (newfingerprinthook)ipNewFingerPrint.ToPointer();

				auto writeFingerprints = gcnew DelegateNotify(this, &UIOperations::HookWriteFingerprints);
				_gcWriteFingerprints = GCHandle::Alloc(writeFingerprints);
				IntPtr ipWriteFingerprints = Marshal::GetFunctionPointerForDelegate(writeFingerprints);
				_ops->write_fingerprints = (notifyhook)ipWriteFingerprints.ToPointer();

				auto goneSecure = gcnew DelegateConnectionNotify(this, &UIOperations::HookGoneSecure);
				_gcGoneSecure = GCHandle::Alloc(goneSecure);
				IntPtr ipGoneSecure = Marshal::GetFunctionPointerForDelegate(goneSecure);
				_ops->gone_secure = (connectionnotifyhook)ipGoneSecure.ToPointer();

				auto goneInsecure = gcnew DelegateConnectionNotify(this, &UIOperations::HookGoneInsecure);
				_gcGoneInsecure = GCHandle::Alloc(goneInsecure);
				IntPtr ipGoneInsecure = Marshal::GetFunctionPointerForDelegate(goneInsecure);
				_ops->gone_insecure = (connectionnotifyhook)ipGoneInsecure.ToPointer();

				auto stillSecure = gcnew DelegateStillSecure(this, &UIOperations::HookStillSecure);
				_gcStillSecure = GCHandle::Alloc(stillSecure);
				IntPtr ipStillSecure = Marshal::GetFunctionPointerForDelegate(stillSecure);
				_ops->still_secure = (stillsecurehook)ipStillSecure.ToPointer();

				auto maxMessageSize = gcnew DelegateMaxMessageSize(this, &UIOperations::HookMaxMessageSize);
				_gcMaxMessageSize = GCHandle::Alloc(maxMessageSize);
				IntPtr ipMaxMessageSize = Marshal::GetFunctionPointerForDelegate(maxMessageSize);
				_ops->max_message_size = (maxmessagesizehook)ipMaxMessageSize.ToPointer();

				auto accountName = gcnew DelegateAccountName(this, &UIOperations::HookGetAccountName);
				_gcAccountName = GCHandle::Alloc(accountName);
				IntPtr ipAccountName = Marshal::GetFunctionPointerForDelegate(accountName);
				_ops->account_name = (accountnamehook)ipAccountName.ToPointer();

				auto accountNameFree = gcnew DelegateStringFree(this, &UIOperations::HookAccountNameFree);
				_gcAccountNameFree = GCHandle::Alloc(accountNameFree);
				IntPtr ipAccountNameFree = Marshal::GetFunctionPointerForDelegate(accountNameFree);
				_ops->account_name_free = (stringfreehook)ipAccountNameFree.ToPointer();

				auto receivedSymKey = gcnew DelegateReceivedSymKey(this, &UIOperations::HookReceivedSymKey);
				_gcReceivedSymKey = GCHandle::Alloc(receivedSymKey);
				IntPtr ipReceivedSymKey = Marshal::GetFunctionPointerForDelegate(receivedSymKey);
				_ops->received_symkey = (receivedsymkeyhook)ipReceivedSymKey.ToPointer();

				auto otrErrorMessage = gcnew DelegateOtrErrorMessage(this, &UIOperations::HookOtrErrorMessage);
				_gcOtrErrorMessage = GCHandle::Alloc(otrErrorMessage);
				IntPtr ipOtrErrorMessage = Marshal::GetFunctionPointerForDelegate(otrErrorMessage);
				_ops->otr_error_message = (otrerrormessagehook)ipOtrErrorMessage.ToPointer();

				auto otrErrorMessageFree = gcnew DelegateStringFree(this, &UIOperations::HookOtrErrorMessageFree);
				_gcOtrErrorMessageFree = GCHandle::Alloc(otrErrorMessageFree);
				IntPtr ipOtrErrorMessageFree = Marshal::GetFunctionPointerForDelegate(otrErrorMessageFree);
				_ops->otr_error_message_free = (stringfreehook)ipOtrErrorMessageFree.ToPointer();

				auto resentPrefixMessage = gcnew DelegateConnectionString(this, &UIOperations::HookResentMsgPrefix);
				_gcResentPrefixMessage = GCHandle::Alloc(resentPrefixMessage);
				IntPtr ipResentPrefixMessage = Marshal::GetFunctionPointerForDelegate(resentPrefixMessage);
				_ops->resent_msg_prefix = (connectionstringhook)ipResentPrefixMessage.ToPointer();

				auto resentPrefixMessageFree = gcnew DelegateStringFree(this, &UIOperations::HookResentMsgPrefixFree);
				_gcResentPrefixMessageFree = GCHandle::Alloc(resentPrefixMessageFree);
				IntPtr ipResentPrefixMessageFree = Marshal::GetFunctionPointerForDelegate(resentPrefixMessageFree);
				_ops->otr_error_message_free = (stringfreehook)ipResentPrefixMessageFree.ToPointer();

				auto handleSmpEvent = gcnew DelegateHandleSmpEvent(this, &UIOperations::HookHandleSmpEvent);
				_gcHandleSmpEvent = GCHandle::Alloc(handleSmpEvent);
				IntPtr ipHandleSmpEvent = Marshal::GetFunctionPointerForDelegate(handleSmpEvent);
				_ops->handle_smp_event= (handlesmpeventhook)ipHandleSmpEvent.ToPointer();

				auto handleMsgEvent = gcnew DelegateHandleMsgEvent(this, &UIOperations::HookHandleMsgEvent);
				_gcHandleMsgEvent = GCHandle::Alloc(handleMsgEvent);
				IntPtr ipHandleMsgEvent = Marshal::GetFunctionPointerForDelegate(handleMsgEvent);
				_ops->handle_msg_event = (handlemsgeventhook)ipHandleMsgEvent.ToPointer();

				auto createInstag = gcnew DelegateCreateInstag(this, &UIOperations::HookCreateInstag);
				_gcCreateInstag = GCHandle::Alloc(createInstag);
				IntPtr ipCreateInstag = Marshal::GetFunctionPointerForDelegate(createInstag);
				_ops->create_instag = (createinstaghook)ipCreateInstag.ToPointer();

				auto convertMsg = gcnew DelegateConvertMsg(this, &UIOperations::HookConvertMsg);
				_gcConvertMsg = GCHandle::Alloc(convertMsg);
				IntPtr ipConvertMsg = Marshal::GetFunctionPointerForDelegate(convertMsg);
				_ops->convert_msg = (convertmsghook)ipConvertMsg.ToPointer();

				auto convertMsgFree = gcnew DelegateConvertMsgFree(this, &UIOperations::HookConvertFree);
				_gcConvertMsgFree = GCHandle::Alloc(convertMsgFree);
				IntPtr ipConvertMsgFree = Marshal::GetFunctionPointerForDelegate(convertMsgFree);
				_ops->convert_free = (convertmsgfreehook)ipConvertMsgFree.ToPointer();

				auto timer = gcnew DelegateTimer(this, &UIOperations::HookTimerControl);
				_gcTimer = GCHandle::Alloc(timer);
				IntPtr ipTimer = Marshal::GetFunctionPointerForDelegate(timer);
				_ops->timer_control = (timerhook)ipTimer.ToPointer();
			}

			UIOperations::~UIOperations()
			{
				_gcGetPolicy.Free();
				_gcCreatePrivKey.Free();
				_gcIsLoggedIn.Free();
				_gcInjectMessage.Free();
				_gcUpdateContextList.Free();
				_gcNewFingerPrint.Free();
				_gcWriteFingerprints.Free();
				_gcGoneSecure.Free();
				_gcGoneInsecure.Free();
				_gcStillSecure.Free();
				_gcMaxMessageSize.Free();
				_gcAccountName.Free();
				_gcAccountNameFree.Free();
				_gcReceivedSymKey.Free();
				_gcOtrErrorMessage.Free();
				_gcOtrErrorMessageFree.Free();
				_gcResentPrefixMessage.Free();
				_gcResentPrefixMessageFree.Free();
				_gcHandleSmpEvent.Free();
				_gcHandleMsgEvent.Free();
				_gcCreateInstag.Free();
				_gcConvertMsg.Free();
				_gcConvertMsgFree.Free();
				_gcTimer.Free();
				Marshal::FreeHGlobal((IntPtr)(void*)_ops);
			}

			OtrlPolicy UIOperations::HookGetPolicy(void *pOpdata, ConnContext *pContext)
			{
				switch(_provider->OnGetPolicy(gcnew OtrConnection(_manager,pContext, false)))
				{
				case OtrPolicy::Never :
					return OTRL_POLICY_NEVER;
				
				case OtrPolicy::Manual :
					return OTRL_POLICY_MANUAL;
				case OtrPolicy::Always :
					return OTRL_POLICY_ALWAYS;
				default:
					return OTRL_POLICY_DEFAULT;
				}
			}

			/* Create a private key for the given accountname/protocol if
			* desired. */
			void UIOperations::HookCreatePrivkey(void *pOpdata, const char *pAccountname,
				const char *pProtocol)
			{				
				_provider->OnCreatePrivKey(gcnew String(pAccountname));
			}

			/* Report whether you think the given user is online.  Return 1 if
			* you think he is, 0 if you think he isn't, -1 if you're not sure.
			*
			* If you return 1, messages such as heartbeats or other
			* notifications may be sent to the user, which could result in "not
			* logged in" errors if you're wrong. */
			int UIOperations::HookIsLoggedIn(void *pOpdata, const char *pAccountname,
				const char *pProtocol, const char *pRecipient)
			{
				
				return _provider->OnLoggedIn(gcnew OtrContact(gcnew String(pProtocol), gcnew String(pRecipient), gcnew String(pAccountname)));
			}

			/* Send the given IM to the given recipient from the given
			* accountname/protocol. */
			void UIOperations::HookInjectMessage(void *pOpdata, const char *pAccountname,const char *pProtocol, const char *pRecipient, const char *pMessage)
			{
				_provider->OnInjectMessage(gcnew OtrContact(gcnew String(pProtocol), gcnew String(pRecipient), gcnew String(pAccountname)), gcnew String(pMessage));
			}

			/* When the list of ConnContexts changes (including a change in
			* state), this is called so the UI can be updated. */
			void UIOperations::HookUpdateContextList(void *pOpdata)
			{
				_provider->OnUpdateContextList();
			}


			/* A new fingerprint for the given user has been received. */
		    void UIOperations::HookNewFingerprint(void *pOpdata, OtrlUserState us,
				const char *pAccountName, const char *pProtocol,
				const char *pUsername, unsigned char pFingerprint[20])
			{
				auto finger = gcnew array<System::Byte>(20);
				const unsigned char *fingerraw = pFingerprint;
				auto *context = otrl_context_find(us, pUsername,pAccountName, pProtocol, OTRL_INSTAG_BEST, TRUE, 0, nullptr, nullptr);
				auto *fingerPrint = otrl_context_find_fingerprint(context, pFingerprint, TRUE, 0);
				_provider->OnNewFingerPrint(gcnew OtrContact(gcnew String(pProtocol), gcnew String(pUsername), gcnew String(pAccountName)), gcnew OtrFingerprint(fingerPrint));
			
				
				
			}

			/* The list of known fingerprints has changed.  Write them to disk. */
			void UIOperations::HookWriteFingerprints(void *pOpdata)
			{
				_provider->OnWriteFingerPrints();
			}

			/* A ConnContext has entered a secure state. */
			void UIOperations::HookGoneSecure(void *pOpdata, ConnContext *pContext)
			{
				_provider->OnGoneSecure(gcnew OtrConnection(_manager,pContext, false));
			}

			/* A ConnContext has left a secure state. */
			void UIOperations::HookGoneInsecure(void *pOpdata, ConnContext *pContext)
			{				
				_provider->OnGoneInsecure(gcnew OtrConnection(_manager,pContext, false));
			}

			/* We have completed an authentication, using the D-H keys we
			* already knew.  is_reply indicates whether we initiated the AKE. */
			void UIOperations::HookStillSecure(void *pOpdata, ConnContext *pContext, int pIsReply)
			{
				_provider->OnStillSecure(gcnew OtrConnection(_manager,pContext, false), pIsReply==1);
			}

			/* Find the maximum message size supported by this protocol. */
			int UIOperations::HookMaxMessageSize(void *pOpdata, ConnContext *pContext)
			{
				return _provider->OnGetMaxMessageSize(gcnew OtrConnection(_manager,pContext, false));
			}

			/* Return a newly allocated string containing a human-friendly
			* representation for the given account */
			const char* UIOperations::HookGetAccountName(void *pOpdata, const char *pAccount,
				const char *pProtocol)
			{
				return (const char*)Marshal::StringToHGlobalAnsi(_provider->OnGetAccountName(gcnew String(pAccount))).ToPointer();
			}

			/* Deallocate a string returned by account_name */
			void UIOperations::HookAccountNameFree(void *opdata, const char *account_name)
			{
				IntPtr intPt((void*)account_name);
				Marshal::FreeHGlobal(intPt);
			}

			/* We received a request from the buddy to use the current "extra"
			* symmetric key.  The key will be passed in symkey, of length
			* OTRL_EXTRAKEY_BYTES.  The requested use, as well as use-specific
			* data will be passed so that the applications can communicate other
			* information (some id for the data transfer, for example). */
			void  UIOperations::HookReceivedSymKey(void *pOpdata, ConnContext *pContext,
				unsigned int pUse, const unsigned char *pUsedata,
				size_t pUsedatalen, const unsigned char *pSymkey)
			{
				auto useData = pUsedatalen > 0 && pUsedata ? gcnew array<System::Byte>  (pUsedatalen) : nullptr;
				if(useData!=nullptr)
					Marshal::Copy((IntPtr)(void *)pUsedata, useData, 0, pUsedatalen);
				auto symKey = gcnew array<System::Byte>(OTRL_EXTRAKEY_BYTES);
				Marshal::Copy((IntPtr)(void *)pSymkey, symKey, 0, OTRL_EXTRAKEY_BYTES);
				return _provider->OnReceivedSymKey(gcnew OtrConnection(_manager,pContext, false),pUse, useData, symKey);
			}

			/* Return a string according to the error event. This string will then
			* be concatenated to an OTR header to produce an OTR protocol error
			* message. The following are the possible error events:
			* - OTRL_ERRCODE_ENCRYPTION_ERROR
			* 		occured while encrypting a message
			* - OTRL_ERRCODE_MSG_NOT_IN_PRIVATE
			* 		sent encrypted message to somebody who is not in
			* 		a mutual OTR session
			* - OTRL_ERRCODE_MSG_UNREADABLE
			*		sent an unreadable encrypted message
			* - OTRL_ERRCODE_MSG_MALFORMED
			* 		message sent is malformed */
			const char* UIOperations::HookOtrErrorMessage(void *pOpdata, ConnContext *pContext,
				OtrlErrorCode pErrCode)
			{
				auto connection = gcnew OtrConnection(_manager,pContext, false);
				String^ errmsg;
				switch(pErrCode)
				{
				case OTRL_ERRCODE_ENCRYPTION_ERROR :
					errmsg= _provider->OnGetErrorMessage(connection, OtrErrorMessage::EncryptionError);
					break;
				case OTRL_ERRCODE_MSG_NOT_IN_PRIVATE:
					errmsg= _provider->OnGetErrorMessage(connection, OtrErrorMessage::MessageNotInPrivate);
					break;
				case OTRL_ERRCODE_MSG_UNREADABLE:
					errmsg= _provider->OnGetErrorMessage(connection, OtrErrorMessage::MessageUnreadable);
					break;
				case OTRL_ERRCODE_MSG_MALFORMED:
					errmsg= _provider->OnGetErrorMessage(connection, OtrErrorMessage::MessageMalformed);
					break;
				default:
					errmsg= _provider->OnGetErrorMessage(connection, OtrErrorMessage::Unknown);
					break;
				}
				return (const char*)Marshal::StringToHGlobalAnsi(errmsg).ToPointer();
			}

			/* Deallocate a string returned by otr_error_message */
			void  UIOperations::HookOtrErrorMessageFree(void *opdata, const char *err_msg) {
				Marshal::FreeHGlobal((IntPtr)(void*)err_msg);
			}

			/* Return a string that will be prefixed to any resent message. If this
			* function is not provided by the application then the default prefix,
			* "[resent]", will be used.
			* */
			const char* UIOperations::HookResentMsgPrefix(void *pOpdata, ConnContext *pContext)
			{
				return (const char*)Marshal::StringToHGlobalAnsi(_provider->OnGetResentMessagePrefix(gcnew OtrConnection(_manager,pContext,false))).ToPointer();
			}

			/* Deallocate a string returned by resent_msg_prefix */
			void  UIOperations::HookResentMsgPrefixFree(void *opdata, const char *prefix)
			{
				Marshal::FreeHGlobal((IntPtr)(void*)prefix);
			}

			/* Update the authentication UI with respect to SMP events
			* These are the possible events:
			* - OTRL_SMPEVENT_ASK_FOR_SECRET
			*      prompt the user to enter a shared secret. The sender application
			*      should call otrl_message_initiate_smp, passing NULL as the question.
			*      When the receiver application resumes the SM protocol by calling
			*      otrl_message_respond_smp with the secret answer.
			* - OTRL_SMPEVENT_ASK_FOR_ANSWER
			*      (same as OTRL_SMPEVENT_ASK_FOR_SECRET but sender calls
			*      otrl_message_initiate_smp_q instead)
			* - OTRL_SMPEVENT_CHEATED
			*      abort the current auth and update the auth progress dialog
			*      with progress_percent. otrl_message_abort_smp should be called to
			*      stop the SM protocol.
			* - OTRL_SMPEVENT_INPROGRESS 	and
			*   OTRL_SMPEVENT_SUCCESS 		and
			*   OTRL_SMPEVENT_FAILURE    	and
			*   OTRL_SMPEVENT_ABORT
			*      update the auth progress dialog with progress_percent
			* - OTRL_SMPEVENT_ERROR
			*      (same as OTRL_SMPEVENT_CHEATED)
			* */
			void UIOperations::HookHandleSmpEvent(void *opdata, OtrlSMPEvent smp_event,
			ConnContext *context, unsigned short progress_percent,
				char *pQuestion) {
				auto question = pQuestion == nullptr ? nullptr : gcnew String(pQuestion);
				switch(smp_event)
				{
				case OTRL_SMPEVENT_ASK_FOR_SECRET:
					_provider->OnSmpEvent(gcnew OtrConnection(_manager,context,false), OtrSmpEvent::AskForSecret, question, (int)progress_percent);
					break;
				case OTRL_SMPEVENT_ASK_FOR_ANSWER:
					_provider->OnSmpEvent(gcnew OtrConnection(_manager,context, false), OtrSmpEvent::AskForAnswer, question, (int)progress_percent);
					break;
				case  OTRL_SMPEVENT_ERROR:
					_provider->OnSmpEvent(gcnew OtrConnection(_manager, context, false), OtrSmpEvent::Error, question, (int)progress_percent);
					break;
				case  OTRL_SMPEVENT_CHEATED:
					_provider->OnSmpEvent(gcnew OtrConnection(_manager, context, false), OtrSmpEvent::Cheated, question, (int)progress_percent);
					break;
				case  OTRL_SMPEVENT_IN_PROGRESS:
					_provider->OnSmpEvent(gcnew OtrConnection(_manager, context, false), OtrSmpEvent::InProgress, question, (int)progress_percent);
					break;
				case  OTRL_SMPEVENT_SUCCESS:
					_provider->OnSmpEvent(gcnew OtrConnection(_manager, context, false), OtrSmpEvent::Success, question, (int)progress_percent);
					break;
				case  OTRL_SMPEVENT_FAILURE:
					_provider->OnSmpEvent(gcnew OtrConnection(_manager, context, false), OtrSmpEvent::Failure, question, (int)progress_percent);
					break;
				case  OTRL_SMPEVENT_ABORT:
					_provider->OnSmpEvent(gcnew OtrConnection(_manager, context, false), OtrSmpEvent::Abort, question, (int)progress_percent);
					break;
				}
			}

			/* Handle and send the appropriate message(s) to the sender/recipient
			* depending on the message events. All the events only require an opdata,
			* the event, and the context. The message and err will be NULL except for
			* some events (see below). The possible events are:
			* - OTRL_MSGEVENT_ENCRYPTION_REQUIRED
			*      Our policy requires encryption but we are trying to send
			*      an unencrypted message out.
			* - OTRL_MSGEVENT_ENCRYPTION_ERROR
			*      An error occured while encrypting a message and the message
			*      was not sent.
			* - OTRL_MSGEVENT_CONNECTION_ENDED
			*      Message has not been sent because our buddy has ended the
			*      private conversation. We should either close the connection,
			*      or refresh it.
			* - OTRL_MSGEVENT_SETUP_ERROR
			*      A private conversation could not be set up. A gcry_error_t
			*      will be passed.
			* - OTRL_MSGEVENT_MSG_REFLECTED
			*      Received our own OTR messages.
			* - OTRL_MSGEVENT_MSG_RESENT
			*      The previous message was resent.
			* - OTRL_MSGEVENT_RCVDMSG_NOT_IN_PRIVATE
			*      Received an encrypted message but cannot read
			*      it because no private connection is established yet.
			* - OTRL_MSGEVENT_RCVDMSG_UNREADABLE
			*      Cannot read the received message.
			* - OTRL_MSGEVENT_RCVDMSG_MALFORMED
			*      The message received contains malformed data.
			* - OTRL_MSGEVENT_LOG_HEARTBEAT_RCVD
			*      Received a heartbeat.
			* - OTRL_MSGEVENT_LOG_HEARTBEAT_SENT
			*      Sent a heartbeat.
			* - OTRL_MSGEVENT_RCVDMSG_GENERAL_ERR
			*      Received a general OTR error. The argument 'message' will
			*      also be passed and it will contain the OTR error message.
			* - OTRL_MSGEVENT_RCVDMSG_UNENCRYPTED
			*      Received an unencrypted message. The argument 'message' will
			*      also be passed and it will contain the plaintext message.
			* - OTRL_MSGEVENT_RCVDMSG_UNRECOGNIZED
			*      Cannot recognize the type of OTR message received.
			* - OTRL_MSGEVENT_RCVDMSG_FOR_OTHER_INSTANCE
			*      Received and discarded a message intended for another instance. */
			void UIOperations::HookHandleMsgEvent(void *pOpdata, OtrlMessageEvent pMsgEvent,
				ConnContext *pContext, const char *pMessage,
				gcry_error_t pErr)
			{
				auto message = pMessage == nullptr ? nullptr : gcnew String(pMessage);
				auto connection = gcnew OtrConnection(_manager, pContext,false);
				switch(pMsgEvent)
				{
				case OTRL_MSGEVENT_ENCRYPTION_REQUIRED :
					_provider->OnMessageEvent(connection, OtrMessageEvent::EncryptionRequired, message, pErr);
					break;
				case OTRL_MSGEVENT_ENCRYPTION_ERROR :
					_provider->OnMessageEvent(connection, OtrMessageEvent::EncryptionError, message, pErr);
					break;
				case OTRL_MSGEVENT_CONNECTION_ENDED :
					_provider->OnMessageEvent(connection, OtrMessageEvent::ConnectionEnded, message, pErr);
					break;
				case OTRL_MSGEVENT_SETUP_ERROR :
					_provider->OnMessageEvent(connection, OtrMessageEvent::SetupError, message, pErr);
					break;
				case OTRL_MSGEVENT_MSG_REFLECTED : 
					_provider->OnMessageEvent(connection, OtrMessageEvent::MessageReflected, message, pErr);
					break;
				case OTRL_MSGEVENT_MSG_RESENT :
					_provider->OnMessageEvent(connection, OtrMessageEvent::MessageResent, message, pErr);
					break;
				case OTRL_MSGEVENT_RCVDMSG_NOT_IN_PRIVATE :
					_provider->OnMessageEvent(connection, OtrMessageEvent::ReceivedNotInPrivate, message, pErr);
					break;
				case OTRL_MSGEVENT_RCVDMSG_UNREADABLE :
					_provider->OnMessageEvent(connection, OtrMessageEvent::ReceivedUnreadable, message, pErr);
					break;
				case OTRL_MSGEVENT_RCVDMSG_MALFORMED :
					_provider->OnMessageEvent(connection, OtrMessageEvent::ReceivedMalformed, message, pErr);
					break;
				case OTRL_MSGEVENT_LOG_HEARTBEAT_RCVD :	
					_provider->OnMessageEvent(connection, OtrMessageEvent::LogHeartBeatReceived, message, pErr);
					break;
				case OTRL_MSGEVENT_LOG_HEARTBEAT_SENT :		
					_provider->OnMessageEvent(connection, OtrMessageEvent::LogHeartBeatSent, message, pErr);
					break;
				case OTRL_MSGEVENT_RCVDMSG_GENERAL_ERR:
					_provider->OnMessageEvent(connection, OtrMessageEvent::ReceivedGeneralError, message, pErr);
					break;
				case OTRL_MSGEVENT_RCVDMSG_UNENCRYPTED	:	
					_provider->OnMessageEvent(connection, OtrMessageEvent::ReceivedUnencrypted, message, pErr);
					break;
				case OTRL_MSGEVENT_RCVDMSG_UNRECOGNIZED :	
					_provider->OnMessageEvent(connection, OtrMessageEvent::ReceivedUnrecognised, message, pErr);
					break;
				case OTRL_MSGEVENT_RCVDMSG_FOR_OTHER_INSTANCE :	
					_provider->OnMessageEvent(connection, OtrMessageEvent::OtherInstance, message, pErr);
					break;
				}
			}

			/* Create a instance tag for the given accountname/protocol if
			* desired. */
			void UIOperations::HookCreateInstag(void *opdata, const char *accountname,
				const char *protocol)
			{
				_provider->OnCreateInstag(gcnew String(accountname));
			}

			/* Called immediately before a data message is encrypted, and after a data
			* message is decrypted. The OtrlConvertType parameter has the value
			* OTRL_CONVERT_SENDING or OTRL_CONVERT_RECEIVING to differentiate these
			* cases. */
			void UIOperations::HookConvertMsg(void *opdata, ConnContext *context,
				OtrlConvertType convert_type, char ** dest, const char *src)
			{
				String^ coverted = nullptr;
				auto message = src == nullptr ? nullptr : gcnew String(src);
				auto connection = gcnew OtrConnection(_manager, context, false);
				switch(convert_type)
				{
				case OTRL_CONVERT_SENDING :
					coverted = _provider->OnConvertMessage(connection, OtrConvertType::Sending, message);
					break;
				case OTRL_CONVERT_RECEIVING :
					coverted = _provider->OnConvertMessage(connection, OtrConvertType::Recieving, message);
					break;
				}
				if(coverted!=nullptr)
					*dest = (char*)Marshal::StringToHGlobalAnsi(coverted).ToPointer();
			}

			/* Deallocate a string returned by convert_msg. */
			void UIOperations::HookConvertFree(void *opdata, ConnContext *context, char *dest)
			{
				Marshal::FreeHGlobal((IntPtr)(void*)dest);
			}

			/* When timer_control is called, turn off any existing periodic
			* timer.
			*
			* Additionally, if interval > 0, set a new periodic timer
			* to go off every interval seconds.  When that timer fires, you
			* must call otrl_message_poll(userstate, uiops, uiopdata); from the
			* main libotr thread.
			*
			* The timing does not have to be exact; this timer is used to
			* provide forward secrecy by cleaning up stale private state that
			* may otherwise stick around in memory.  Note that the
			* timer_control callback may be invoked from otrl_message_poll
			* itself, possibly to indicate that interval == 0 (that is, that
			* there's no more periodic work to be done at this time).
			*
			* If you set this callback to NULL, then you must ensure that your
			* application calls otrl_message_poll(userstate, uiops, uiopdata);
			* from the main libotr thread every definterval seconds (where
			* definterval can be obtained by calling
			* definterval = otrl_message_poll_get_default_interval(userstate);
			* right after creating the userstate).  The advantage of
			* implementing the timer_control callback is that the timer can be
			* turned on by libotr only when it's needed.
			*
			* It is not a problem (except for a minor performance hit) to call
			* otrl_message_poll more often than requested, whether
			* timer_control is implemented or not.
			*
			* If you fail to implement the timer_control callback, and also
			* fail to periodically call otrl_message_poll, then you open your
			* users to a possible forward secrecy violation: an attacker that
			* compromises the user's computer may be able to decrypt a handful
			* of long-past messages (the first messages of an OTR
			* conversation).
			*/
			void UIOperations::HookTimerControl(void *opdata, unsigned int interval)
			{
				_provider->OnTimerChange((IntPtr)(Void*)(opdata), interval);
			//	_provider->OnCreateInstag(gcnew String(accountname));
			}
		}
	}
}