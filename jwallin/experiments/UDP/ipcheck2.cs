using System;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Net.NetworkInformation;

class ipcheck {

static void Main() 
{
  string myIP;
  myIP = GetLocalIPAddress();

  Console.WriteLine(" my ip = "+ myIP);



  ShowNetworkInterfaces();

}





public static void ShowNetworkInterfaces()
{
    IPGlobalProperties computerProperties = IPGlobalProperties.GetIPGlobalProperties();
    NetworkInterface[] nics = NetworkInterface.GetAllNetworkInterfaces();
    Console.WriteLine("Interface information for {0}.{1}     ",
            computerProperties.HostName, computerProperties.DomainName);
    if (nics == null || nics.Length < 1)
    {
        Console.WriteLine("  No network interfaces found.");
        return;
    }
                     
    Console.WriteLine("  Number of interfaces .................... : {0}", nics.Length);
    foreach (NetworkInterface adapter in nics)
    {
        IPInterfaceProperties properties = adapter.GetIPProperties(); //  .GetIPInterfaceProperties();
        Console.WriteLine();
        Console.WriteLine(adapter.Description);
        Console.WriteLine(String.Empty.PadLeft(adapter.Description.Length,'='));
        Console.WriteLine("  Interface type .......................... : {0}", adapter.NetworkInterfaceType);
        Console.Write("  Physical address ........................ : ");
        PhysicalAddress address = adapter.GetPhysicalAddress();
        byte[] bytes = address.GetAddressBytes();
        for(int i = 0; i< bytes.Length; i++)
        {
            // Display the physical address in hexadecimal.
            Console.Write("{0}", bytes[i].ToString("X2"));
            // Insert a hyphen after each byte, unless we are at the end of the 
            // address.
            if (i != bytes.Length -1)
            {
                 Console.Write("-");
            }
        }
        Console.WriteLine();
    }
}






public static string GetLocalIPAddress()
{
    var host = Dns.GetHostEntry(Dns.GetHostName());
    foreach (var ip in host.AddressList)
    {
        if (ip.AddressFamily == AddressFamily.InterNetwork)
        {
            return ip.ToString();
        }
    }
    throw new Exception("No network adapters with an IPv4 address in the system!");
}


}
