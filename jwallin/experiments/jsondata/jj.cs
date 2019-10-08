using System;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Net.NetworkInformation;
using System.Web.Script.Serialization;
using System.IO;
using System.Collections.Generic;
//using Newtonsoft.Json;

//using json.net;


class jj {

  static void Main() 
  {
    var jjj = new Json.JsonConvert();
    var t = "{\"x\":57,\"y\":57.0,\"z\":\"Yes\"}";
    //var obj = Json.JsonConvert.DeserializeObject(t); 
    var obj = jjj.DeserializeObject(t); 


  }

}
