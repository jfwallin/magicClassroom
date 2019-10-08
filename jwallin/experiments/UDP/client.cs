using System;
using System.Net;
using System.Net.Sockets;
using System.Text;

class client 
{
    static void Main(string[] args)
    {
        Socket s = new Socket(AddressFamily.InterNetwork, SocketType.Dgram, ProtocolType.Udp);
        

        string UDP_IP_ADDRESS = "127.0.0.1";
        int UDP_PORT_NO = 6789;
        IPAddress broadcast = IPAddress.Parse(UDP_IP_ADDRESS);
        //IPAddress broadcast = IPAddress.Parse("127.0.0.1");
        
        byte[] sendbuf = Encoding.ASCII.GetBytes(args[0]);
        IPEndPoint ep = new IPEndPoint(broadcast, UDP_PORT_NO);
        
        s.SendTo(sendbuf, ep);
        
        Console.WriteLine("Message sent to the broadcast address");
    }
}
