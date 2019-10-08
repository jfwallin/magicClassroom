using System;
using System.Net;
using System.Net.Sockets;
using System.Text;
using NetworkInterface;

class ipcheck {

static void Main() 
{
  string myIP;
  myIP = GetLocalIPAddress();

  Console.WriteLine(" my ip = "+ myIP);

  string myMac;
  myMac = GetMacAddress();
  Console.WriteLine("my MAC = "+ myMac);


}


/// <summary>
/// Finds the MAC address of the NIC with maximum speed.
/// </summary>
/// <returns>The MAC address.</returns>
private string GetMacAddress()
{
    const int MIN_MAC_ADDR_LENGTH = 12;
    string macAddress = string.Empty;
    long maxSpeed = -1;

    foreach (NetworkInterface nic in NetworkInterface.GetAllNetworkInterfaces())
    {
        log.Debug(
            "Found MAC Address: " + nic.GetPhysicalAddress() +
            " Type: " + nic.NetworkInterfaceType);

        string tempMac = nic.GetPhysicalAddress().ToString();
        if (nic.Speed > maxSpeed &&
            !string.IsNullOrEmpty(tempMac) &&
            tempMac.Length >= MIN_MAC_ADDR_LENGTH)
        {
            log.Debug("New Max Speed = " + nic.Speed + ", MAC: " + tempMac);
            maxSpeed = nic.Speed;
            macAddress = tempMac;
        }
    }

    return macAddress;
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
