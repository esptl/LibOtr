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
        static void Main(string[] args)
        {
            var fred = new Otr("sip", "fred","c:\\work\\fred");
            var bob = new Otr("sip","bob","c:\\work\\bob");
            fred.InjectMessage = (pContact, pS) =>
            {
                Console.WriteLine($"Fred->Bob: {pS}");
                bob.ReceiveMessage(pContact, pS);
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
                           pConnection => Console.WriteLine($"Bob: Gone secure with {pConnection.Contact.AccountName}");

            Console.ReadLine();
            Console.WriteLine("BEGIN");
            var msg = fred.SendMessage(bobContact, "Hi");
            bob.ReceiveMessage(fredContact, msg.Message);
            Console.WriteLine("END");
            Console.ReadLine();
            Console.WriteLine("BEGIN");
         
            fredCon.InitiateSmp("hi","you");
            Console.WriteLine("END");
            Console.ReadLine();
        }
    }
}
