using System;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Net.NetworkInformation;


public struct myInterface
{
  public String IP;
  public String Mac;
  public String Name;
  public String Interface;
  public String Status;
}


class ipcheck {

  static void Main() 
  {
    myInterface[] ilist = new myInterface[40];
    int icount = 0;
    (icount, ilist) = ShowNetworkInterfaces();

    for(int ii = 0; ii < icount; ii++) {
      Console.WriteLine(" name        = {0}", ilist[ii].Name);
      Console.WriteLine(" IP          = {0}", ilist[ii].IP);
      Console.WriteLine(" MAC         = {0}", ilist[ii].Mac);
      Console.WriteLine(" Interface   = {0}", ilist[ii].Interface);
      Console.WriteLine(" Status      = {0}", ilist[ii].Status);
    }
  }




  public static (int, myInterface [] ) ShowNetworkInterfaces()
  {

    myInterface[] ilist = new myInterface[40];
    int icount = 0;

    IPGlobalProperties computerProperties = IPGlobalProperties.GetIPGlobalProperties();
    NetworkInterface[] nics = NetworkInterface.GetAllNetworkInterfaces();
    if (nics == null || nics.Length < 1)
    {
      return (icount, ilist);
    }


    foreach(NetworkInterface ni in NetworkInterface.GetAllNetworkInterfaces())
    {
      ilist[icount].IP = "";
      ilist[icount].Mac = "";
      ilist[icount].Name = "";
      ilist[icount].Interface = "";
      ilist[icount].Status = "";

      if(ni.NetworkInterfaceType == NetworkInterfaceType.Wireless80211 || ni.NetworkInterfaceType == NetworkInterfaceType.Ethernet)
      {
        foreach (UnicastIPAddressInformation ip in ni.GetIPProperties().UnicastAddresses)
        {

          if (ip.Address.AddressFamily == System.Net.Sockets.AddressFamily.InterNetwork)
          {
            ilist[icount].IP = ip.Address.ToString();
            ilist[icount].Mac = ni.GetPhysicalAddress().ToString();
            ilist[icount].Name = ni.Name;
            ilist[icount].Interface = ni.NetworkInterfaceType.ToString();
            ilist[icount].Status = ni.OperationalStatus.ToString();
            icount = icount + 1;

          }
        }
      }  
    }

    return (icount, ilist);

  } 
}
