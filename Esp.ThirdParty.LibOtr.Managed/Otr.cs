using System;
using System.Diagnostics;
using System.IO;
using System.Threading;

namespace Esp.ThirdParty.LibOtr
{    
    public class Otr : IUIOperationProvider  
    {
        private readonly OtrManager _manager;
        private readonly Timer _timer;
        private IntPtr _timerData;

        private void TimerTick(object pState)
        {
            _manager.Poll(_timerData);
        }

        public Otr(string pProtocol, string pAccountName, string pBaseFile)
        {
            if(!Directory.Exists(Path.GetDirectoryName(pBaseFile)))
                Directory.CreateDirectory(Path.GetDirectoryName(pBaseFile));
            _timer = new Timer(TimerTick);
            _manager = new OtrManager(this,pAccountName, pProtocol, pBaseFile);
        }

        #region Properties 

        public string ResentMessagePrefix { get; set; } = "Resent";
        public string ErrorUnknown { get; set; } = "An unknown OTR error occurred communicating with {0}.";
        public string ErrorEncryption { get; set; } = "An OTR encryption error occurred while communicating with {0}.";
        public string ErrorNotInPrivate { get; set; } = "An encrypted message was received outside of an encrypted session with {0}.";
        public string ErrorUnreadable { get; set; } = "Received an unreadable message from {0}.";
        public string ErrorMalformed { get; set; } = "Received a malformed message from {0}.";

        #endregion

        #region Lamdbas

        public Func<OtrConnection,OtrPolicy> GetPolicy { get; set; } = pConnection => OtrPolicy.Default;
        public Func<OtrContact, bool> IsLoggedIn { get; set; } = pContact => false;
        public Action<OtrContact, string> InjectMessage { get; set; } = (pContact, pMessage) => Debug.WriteLine($"{pMessage} -> {pContact}");
        public Action UpdateContextList { get; set; }
        public Action<OtrConnection> GoneSecure { get; set; } = (pConnection) => Debug.WriteLine($"{pConnection.Contact}: Gone Secure");
        public Action<OtrConnection> GoneInsecure { get; set; } = (pConnection) => Debug.WriteLine($"{pConnection.Contact}: Gone Insecure");
        public Action<OtrConnection,bool> StillSecure { get; set; } = (pConnection, pIsReply) => Debug.WriteLine($"{pConnection.Contact}: Still Secure (IsReply: {pIsReply})");

        public Action<OtrConnection,string> ReceivedUnencryptedMessage { get; set; } = (pContact, pMessage) => Debug.WriteLine($"Unencrypted <{pContact}> {pMessage}");

        public Func<OtrConnection, string, string> ConvertIncomingMessage { get; set; } = (pConnection, pMessage) => pMessage;
        public Func<OtrConnection, string, string> ConvertOutgoingMessage { get; set; } = (pConnection, pMessage) => pMessage;

        public Action<OtrConnection,int, string> SmpAskForAnswer { get; set; } = (pConnection,pPercent, pQuestion) => Debug.WriteLine($"{pConnection.Contact}: Has asked '{pQuestion}?' to authenticate this conversation ({pPercent}%).");
        public Action<OtrConnection,int> SmpAskForSecret { get; set; } = (pConnection, pPercent) => Debug.WriteLine($"{pConnection.Contact}: Has asked to authenticate using shared secret ({pPercent}%).");
        public Action<OtrConnection> SmpCheated { get; set; } = (pConnection) => Debug.WriteLine($"{pConnection.Contact} cheated in the SMP exchange.");
        public Action<OtrConnection,int> SmpProgress { get; set; } = (pConnection, pPercent) => Debug.WriteLine($"{pConnection.Contact} progressed to {pPercent}% in smp exchange.");
        public Action<OtrConnection> SmpSuccess { get; set; } = (pConnection) => Debug.WriteLine($"{pConnection.Contact} SMP exchange succeeded and contact is now trusted.");
        public Action<OtrConnection> SmpFailed { get; set; } = (pConnection) => Debug.WriteLine($"{pConnection.Contact} SMP exchange failed therefore contact is not trusted.");
        public Action<OtrConnection> SmpAborted { get; set; } = (pConnection) => Debug.WriteLine($"{pConnection.Contact} SMP exchange aborted therefore contact is not trusted.");

        public Action<OtrConnection> SmpError { get; set; } = (pConnection) => Debug.WriteLine($"{pConnection.Contact} SMP error occurred.");

        
        #endregion

        public OtrMessage SendMessage(OtrContact pContact, string pMessage)
        {
            return _manager.SendMessage(pContact, pMessage);
        }


        public OtrContact GetContact( string pUserName, string pAccountName)
        {
            return _manager.GetContact(pUserName, pAccountName);
        }

        public OtrConnection FindConnection(OtrContact pContact)
        {
            return _manager.FindConnection(pContact);
        }

        public OtrMessage ReceiveMessage(OtrContact pContact, string pMessage)
        {
            return _manager.ReceiveMessage(pContact, pMessage);
        }

        #region IUIOperationProvider
        OtrPolicy IUIOperationProvider.OnGetPolicy(OtrConnection pConnection)
        {
            return GetPolicy(pConnection);
        }

        void IUIOperationProvider.OnCreatePrivKey(string pAccountname)
        {
            _manager.GeneratePrivKey(pAccountname);
        }

        int IUIOperationProvider.OnLoggedIn(OtrContact pContact)
        {
            return IsLoggedIn(pContact) ? 1 : 0;
        }

        int IUIOperationProvider.OnInjectMessage(OtrContact pContact, string pMessage)
        {
            try
            {
                InjectMessage(pContact, pMessage);
            }
            catch (Exception)
            {
                return 0;
            }
            return 1;
        }

        void IUIOperationProvider.OnUpdateContextList()
        {
            UpdateContextList?.Invoke();
        }

        void IUIOperationProvider.OnNewFingerPrint(OtrContact pContact, OtrFingerprint pFingerprint)
        {
            pFingerprint.Trust = OtrFingerprintTrust.Untrusted;
            _manager.WriteFingerprints();

        }

        void IUIOperationProvider.OnWriteFingerPrints()
        {
            _manager.WriteFingerprints();
        }

        void IUIOperationProvider.OnGoneSecure(OtrConnection pConnection)
        {
            GoneSecure(pConnection);
        }

        void IUIOperationProvider.OnGoneInsecure(OtrConnection pConnection)
        {
            GoneInsecure(pConnection);
        }

        void IUIOperationProvider.OnStillSecure(OtrConnection pConnection, bool pIsReply)
        {
            StillSecure(pConnection, pIsReply);
        }

        int IUIOperationProvider.OnGetMaxMessageSize(OtrConnection pConnection)
        {
            Console.WriteLine("Max Message Size");
            return 0;
        }

        string IUIOperationProvider.OnGetAccountName(string pAccount)
        {
            return pAccount;            
        }

        void IUIOperationProvider.OnReceivedSymKey(OtrConnection pConnection, uint pUse, byte[] pUseData, byte[] pSymKey)
        {
            Console.WriteLine("Received Sym Key");
        }

        string IUIOperationProvider.OnGetErrorMessage(OtrConnection pConnection, OtrErrorMessage pErrorCode)
        {
            switch (pErrorCode)
            {
                case OtrErrorMessage.Unknown:
                    return string.Format(ErrorUnknown, pConnection.Contact);
                case OtrErrorMessage.EncryptionError:
                    return string.Format(ErrorEncryption, pConnection.Contact);
                case OtrErrorMessage.MessageNotInPrivate:
                    return string.Format(ErrorNotInPrivate, pConnection.Contact);
                case OtrErrorMessage.MessageUnreadable:
                    return string.Format(ErrorUnreadable, pConnection.Contact);
                case OtrErrorMessage.MessageMalformed:
                    return string.Format(ErrorMalformed, pConnection.Contact);
                default:
                    throw new ArgumentOutOfRangeException(nameof(pErrorCode), pErrorCode, null);
            }
        }

        string IUIOperationProvider.OnGetResentMessagePrefix(OtrConnection pConnection)
        {
            return ResentMessagePrefix;
        }

        void IUIOperationProvider.OnSmpEvent(OtrConnection pConnection, OtrSmpEvent pEvent, string pQuestion, int pPercent)
        {
            switch (pEvent)
            {
                case OtrSmpEvent.AskForSecret:
                    SmpAskForSecret(pConnection, pPercent);
                    break;
                case OtrSmpEvent.AskForAnswer:
                    SmpAskForAnswer(pConnection, pPercent, pQuestion);
                    break;
                case OtrSmpEvent.Error:
                    break;
                case OtrSmpEvent.Cheated:
                    SmpError(pConnection);
                    break;
                case OtrSmpEvent.InProgress:
                    SmpProgress(pConnection, pPercent);
                    break;
                case OtrSmpEvent.Success:
                    _manager.WriteFingerprints();
                    SmpSuccess(pConnection);
                    break;
                case OtrSmpEvent.Failure:
                    SmpFailed(pConnection);
                    break;
                case OtrSmpEvent.Abort:
                    SmpAborted(pConnection);
                    break;
                default:
                    throw new ArgumentOutOfRangeException(nameof(pEvent), pEvent, null);
            }
        }

        void IUIOperationProvider.OnMessageEvent(OtrConnection pConnection, OtrMessageEvent pEvent, string pMessage, int pGCryptErrorCode)
        {
            switch (pEvent)
            {
                case OtrMessageEvent.EncryptionRequired:
                    break;
                case OtrMessageEvent.EncryptionError:
                    break;
                case OtrMessageEvent.ConnectionEnded:
                    break;
                case OtrMessageEvent.SetupError:
                    break;
                case OtrMessageEvent.MessageReflected:
                    break;
                case OtrMessageEvent.MessageResent:
                    break;
                case OtrMessageEvent.ReceivedNotInPrivate:
                    break;
                case OtrMessageEvent.ReceivedUnreadable:
                    break;
                case OtrMessageEvent.ReceivedMalformed:
                    break;
                case OtrMessageEvent.LogHeartBeatReceived:
                    break;
                case OtrMessageEvent.LogHeartBeatSent:
                    break;
                case OtrMessageEvent.ReceivedGeneralError:
                    break;
                case OtrMessageEvent.ReceivedUnencrypted:
                    ReceivedUnencryptedMessage(pConnection, pMessage);
                    return;
                case OtrMessageEvent.ReceivedUnrecognised:
                    break;
                case OtrMessageEvent.OtherInstance:
                    break;
                default:
                    throw new ArgumentOutOfRangeException(nameof(pEvent), pEvent, null);
            }
            Console.WriteLine($"Msg Event: '{pMessage}', {pEvent}, {pGCryptErrorCode}");
        }

        void IUIOperationProvider.OnCreateInstag(string pAccountname)
        {
            _manager.GenerateInstag(pAccountname);
        }

        string IUIOperationProvider.OnConvertMessage(OtrConnection pConnection, OtrConvertType pCoversionType, string pMessage)
        {
            switch (pCoversionType)
            {
                case OtrConvertType.Sending:
                    return ConvertOutgoingMessage(pConnection, pMessage);
                case OtrConvertType.Recieving:
                    return ConvertIncomingMessage(pConnection, pMessage);
                default:
                    throw new ArgumentOutOfRangeException(nameof(pCoversionType), pCoversionType, null);
            }
        }

        void IUIOperationProvider.OnTimerChange(IntPtr pData, int pInternal)
        {
            _timerData = pData;
            _timer.Change(pInternal > 0 ? pInternal * 1000 : System.Threading.Timeout.Infinite, pInternal > 0 ? pInternal * 1000 : System.Threading.Timeout.Infinite);
        }

        #endregion
    }
}