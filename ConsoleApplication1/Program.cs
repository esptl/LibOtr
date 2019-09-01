using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Esp.ThirdParty.LibOtr;

namespace ConsoleApplication1
{
    class Program
    {
        private static bool _relayForFred = true;
        static void Main(string[] args)
        {
            var fred = new Otr("sip", "fred","c:\\work\\fred");
            var bob = new Otr("sip","bob","c:\\work\\bob");
         
            fred.InjectMessage = (pContact, pS) =>
            {
                if (_relayForFred)
                {
                    Console.WriteLine($"Fred->Bob: {pS}");
                    bob.ReceiveMessage(pContact, pS);
                }
            };
            var bobContact = fred.GetContact("sip", "bob");
            var fredContact = bob.GetContact("sip", "fred");
            bob.InjectMessage = (pContact, pS) =>
            {
                Console.WriteLine($"Bob->Fred: {pS}");
                fred.ReceiveMessage(pContact, pS);
            };
            OtrConnection fredCon = null;
            fred.GoneSecure =
                pConnection =>
                {
                    
                    fredCon = pConnection;
                  
               
                    Console.WriteLine($"Fred: Gone secure with {pConnection.Contact.AccountName}");
                };

            bob.GoneSecure =
                           pConnection => Console.WriteLine($"Bob: Gone secure with {pConnection.Contact.AccountName} - {pConnection.TheirFingerprint.ToString()}");
            bob.SmpAskForAnswer = (pConnection, pI, pQuestion) =>
            {
                Console.WriteLine($"Bob asked {pQuestion} by {pConnection.Contact.AccountName} ");
                pConnection.AnswerSmp("you");
            };

            fred.SmpSuccess = pConnection => Console.WriteLine($"Fred successful SMP with {pConnection.Contact.AccountName} ");
            fred.StillSecure = (pConnection, pB) => Console.WriteLine($"Fred still secure from {pConnection.Contact.AccountName} - {pConnection.TheirFingerprint.ToString()}");
            fred.SmpAborted = pConnection => Console.WriteLine($"Fred aborted SMP with {pConnection.Contact.AccountName} ");
            fred.SmpCheated = pConnection => Console.WriteLine($"Fred cheated SMP with {pConnection.Contact.AccountName} ");
            fred.SmpFailed = pConnection => Console.WriteLine($"Fred failed SMP with {pConnection.Contact.AccountName} ");
            fred.SmpProgress = (pConnection, pPercent) => Console.WriteLine($"Fred progress SMP with {pConnection.Contact.AccountName} {pPercent}%");
            fred.GoneInsecure = pConnection => Console.WriteLine($"Fred gone insecure with {pConnection.Contact.AccountName} ");
          
            bob.SmpSuccess = pConnection => Console.WriteLine($"Bob successful SMP with {pConnection.Contact.AccountName} ");
            bob.StillSecure = (pConnection, pB) => Console.WriteLine($"Bob still secure from {pConnection.Contact.AccountName} {pB}");
            bob.SmpAborted = pConnection => Console.WriteLine($"Bob aborted SMP with {pConnection.Contact.AccountName} ");
            bob.SmpCheated = pConnection => Console.WriteLine($"Bob cheated SMP with {pConnection.Contact.AccountName} ");
            bob.SmpFailed = pConnection => Console.WriteLine($"Bob failed SMP with {pConnection.Contact.AccountName} ");
            bob.SmpProgress = (pConnection,pPercent) => Console.WriteLine($"Bob progress SMP with {pConnection.Contact.AccountName} {pPercent}%");
            bob.GoneInsecure = pConnection => Console.WriteLine($"Bob gone insecure with {pConnection.Contact.AccountName} ");
            bob.GoneSecure = pConnection => Console.WriteLine($"Bob gone secure with {pConnection.Contact.AccountName} - {pConnection.TheirFingerprint.ToString()} ");
            Console.ReadLine();
            Console.WriteLine("BEGIN");
            var msg = fred.SendMessage(bobContact, "Hi");
            bob.ReceiveMessage(fredContact, msg.Message);
            Console.WriteLine();
            Console.WriteLine("END");
            Console.ReadLine();
            Console.WriteLine("BEGIN2");
            var bobCon = bob.FindConnection(fredContact);
            fredCon.Finish();

            msg = fred.SendMessage(bobContact, "Hi2");
            bob.ReceiveMessage(fredContact, msg.Message);
            Console.WriteLine();
            Console.WriteLine("END2");
            Console.ReadLine();
         //   _relayForFred = false;
          
            Console.ReadLine();
        }
    }
}
