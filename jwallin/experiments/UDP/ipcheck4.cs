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
//       Console.WriteLine("-------");
//       Console.WriteLine(ni.Name);
       foreach (UnicastIPAddressInformation ip in ni.GetIPProperties().UnicastAddresses)
       {



           if (ip.Address.AddressFamily == System.Net.Sockets.AddressFamily.InterNetwork)
           {

               Console.WriteLine("  name = "+ni.Name);
               Console.WriteLine("  ip = "+ip.Address.ToString());

                IPInterfaceProperties properties = ni.GetIPProperties();


                Console.WriteLine("  desc = "+ni.Description);
                //Console.WriteLine(String.Empty.PadLeft(ni.Description.Length,'='));
                Console.WriteLine("  Interface type .......................... : {0}", ni.NetworkInterfaceType);
                Console.WriteLine("  Physical Address ........................ : {0}", 
                           ni.GetPhysicalAddress().ToString());
                Console.WriteLine("  Operational status ...................... : {0}", 
                    ni.OperationalStatus);
                string versions ="";


           }
       }
   }  
}




Console.WriteLine("----------------------");
Console.WriteLine("----------------------");
Console.WriteLine("----------------------");
Console.WriteLine("----------------------");
Console.WriteLine("----------------------");
Console.WriteLine("----------------------");
Console.WriteLine("----------------------");
Console.WriteLine("----------------------");
Console.WriteLine("----------------------");
Console.WriteLine("----------------------");
Console.WriteLine("----------------------");


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
