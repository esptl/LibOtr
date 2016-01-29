using System;

namespace Esp.ThirdParty.LibOtr
{
    internal  static class NativeApi
    {

        static void Main()
        {
            Otr otr2 = null;
            var otr1 = new Otr("sip", "201", "201")
            {
                InjectMessage = (pContact, pMessage) =>
                {
                    Console.WriteLine("1->2 injected: "+pMessage);
                    otr2.ReceiveMessage(otr2.GetContact("201", "201"),pMessage);
                },
                GoneSecure = (pConnection) => Console.WriteLine("{0} gone secure", pConnection.Contact)
            };
            otr2 = new Otr("sip", "202","202")
            {
                InjectMessage = (pContact, pMessage) =>
                {
                    Console.WriteLine("2->1 injected: " + pMessage);
                    otr1.ReceiveMessage(otr1.GetContact("202", "202"), pMessage);
                },
                GoneSecure = (pConnection) => Console.WriteLine("{0} gone secure", pConnection.Contact)
            };
            //  var c = otr.FindConnection(otr.GetContact("kames", "kames"), 1);
            //     var d = otr.FindConnection(otr.GetContact("kames", "kames"), 2);
            //  var t = c.State;

            while (true)
            {
                Console.Write("> ");
                var s = Console.ReadLine();
                if (s == ".") break;
                if (s.StartsWith("1:"))
                {
                    var msg = otr1.SendMessage(otr1.GetContact("202", "202"),s.Substring(2));
                    Console.WriteLine("Sent: "+msg.Message);
                    var msg2 = otr2.ReceiveMessage(otr2.GetContact("201","201"),msg.Message);
                    Console.WriteLine("Rec: " + (msg2!=null?$"{msg2.Message} - {msg2.Connection.State}":"NULL"));
                }

                if (s.StartsWith("2:"))
                {
                    var msg = otr2.SendMessage(otr1.GetContact("102", "robin"), s.Substring(2));
                    Console.WriteLine("Sent: " + msg.Message);
                    var msg2 = otr1.ReceiveMessage(otr1.GetContact("202", "202"), msg.Message);
                    Console.WriteLine("Rec: " + (msg2 != null ? $"{msg2.Message} - {msg2.Connection.State}" : "NULL"));
                }
            }
         
         //var msg2=   otr2.ReceiveMessage(otr2.GetContact("201", "robin"), "Hello! \t  \t\t\t\t \t \t \t    \t\t  \t   \t\t  \t\t");
     
           // var msg2 = otr2.SendMessage(otr2.GetContact("203", "robin"), "Hello 2!");
        }
        
    }

}
