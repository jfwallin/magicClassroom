using System;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Net.NetworkInformation;



class ipcheck {

static void Main() 
{

  //string myIP;
  //myIP = GetLocalIPAddress();
  //Console.WriteLine(" my ip = "+ myIP);



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
    


foreach(NetworkInterface ni in NetworkInterface.GetAllNetworkInterfaces())
{
   if(ni.NetworkInterfaceType == NetworkInterfaceType.Wireless80211 || ni.NetworkInterfaceType == NetworkInterfaceType.Ethernet)
   {
       Console.WriteLine(ni.Name);
       foreach (UnicastIPAddressInformation ip in ni.GetIPProperties().UnicastAddresses)
       {
           if (ip.Address.AddressFamily == System.Net.Sockets.AddressFamily.InterNetwork)
           {
               Console.WriteLine(ip.Address.ToString());
           }
       }
   }  
}




Console.WriteLine("----------------------");
Console.WriteLine("----------------------");
Console.WriteLine("----------------------");
Console.WriteLine("----------------------");






    foreach (NetworkInterface adapter in nics)
    {
        IPInterfaceProperties properties = adapter.GetIPProperties();
        
//IPInterfaceProperties adapterProperties = adapter.GetIPProperties();
        // Try to get the IPv4 interface properties.
  //      IPv4InterfaceProperties p = adapterProperties.GetIPv4Properties();
        


        Console.WriteLine();
        Console.WriteLine(adapter.Description);
        Console.WriteLine(String.Empty.PadLeft(adapter.Description.Length,'='));
        Console.WriteLine("  Interface type .......................... : {0}", adapter.NetworkInterfaceType);
        Console.WriteLine("  Physical Address ........................ : {0}", 
                   adapter.GetPhysicalAddress().ToString());
        Console.WriteLine("  Operational status ...................... : {0}", 
            adapter.OperationalStatus);
        string versions ="";






IPInterfaceProperties adapterProperties = adapter.GetIPProperties();
IPv4InterfaceProperties p = adapterProperties.GetIPv4Properties();
Console.WriteLine("  Index sdfsdfgw............................. : {0}", p.Index);

//ip = GetIPProperties();
//properties
string  IpAddress = properties.UnicastAddresses.Count > 0 ? properties.UnicastAddresses[0].Address.ToString() : "";
Console.WriteLine("IIIIII = : {0}",IpAddress);


UnicastIPAddressInformationCollection uniCast = properties.UnicastAddresses;
foreach (UnicastIPAddressInformation uni in uniCast)
{
    string address = uni.Address.ToString();
    Console.WriteLine("slj34lj = ",address);
}








        // Create a display string for the supported IP versions.
        if (adapter.Supports(NetworkInterfaceComponent.IPv4))
        {
             versions = "IPv4";
         }
        if (adapter.Supports(NetworkInterfaceComponent.IPv6))
        {
            if (versions.Length > 0)
            {
                versions += " ";
             }
            versions += "IPv6";
        }
        Console.WriteLine("  IP version .............................. : {0}", versions);
        //ShowIPAddresses(properties);
        
        // The following information is not useful for loopback adapters.
        if (adapter.NetworkInterfaceType == NetworkInterfaceType.Loopback)
        {
            continue;
        }
        Console.WriteLine("  DNS suffix .............................. : {0}", 
            properties.DnsSuffix);
        
        string label;
        if (adapter.Supports(NetworkInterfaceComponent.IPv4))
        {
            IPv4InterfaceProperties ipv4 = properties.GetIPv4Properties();
            Console.WriteLine("  MTU...................................... : {0}", ipv4.Mtu);
            if (ipv4.UsesWins)
            {
                
                IPAddressCollection winsServers = properties.WinsServersAddresses;
                if (winsServers.Count > 0)
                {
                    label = "  WINS Servers ............................ :";
                    //ShowIPAddresses(label, winsServers);
                }
            }
        }
                 
        Console.WriteLine("  DNS enabled ............................. : {0}", 
            properties.IsDnsEnabled);
        Console.WriteLine("  Dynamically configured DNS .............. : {0}", 
            properties.IsDynamicDnsEnabled);
        Console.WriteLine("  Receive Only ............................ : {0}", 
            adapter.IsReceiveOnly);
        Console.WriteLine("  Multicast ............................... : {0}", 
            adapter.SupportsMulticast);
        //ShowInterfaceStatistics(adapter);
        
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
