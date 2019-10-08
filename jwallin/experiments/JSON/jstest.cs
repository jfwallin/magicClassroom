using System;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Net.NetworkInformation;
using System.Web.Script.Serialization;
//using Json.NET;



public class myInterface
{
  public string IP;
  public string Mac;
  public string Name;
  public string Interface;
  public string Status;
  public int Count;
}


class jstest {

  static void Main() 
  {

    myInterface mi = new myInterface();

    mi.IP = "hello";
    mi.Mac = "mac";
    mi.Name ="name";
    mi.Interface = "kkk";
    mi.Status = "stat";
    mi.Count = 3;

    string json =  new JavaScriptSerializer().Serialize(mi);

    Console.WriteLine(json);

    var serializer = new JavaScriptSerializer();

    myInterface ds = serializer.Deserialize<myInterface>(json);
    Console.WriteLine(ds.Status);

  }



/*
  public static (int, myInterface [] ) ShowNetworkInterfaces()
  {
    return (icount, ilist);

  } 
*/


}
