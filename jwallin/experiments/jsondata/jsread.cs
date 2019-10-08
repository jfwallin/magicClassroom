using System;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Net.NetworkInformation;
using System.Web.Script.Serialization;
using System.IO;
using System.Collections.Generic;
//using Newtonsoft.Json;




public class planet 
{
  public int id;
  public string name;
  public float mass;
  public float diameter;
  public float density;
  public float gravity;
  public float escapeVelocity;
  public float rotationPeriod;
  public float lengthOfDay;
  public float distanceFromSun;
  public float perihelion;
  public float aphelion;
  public float orbitalPeriod; 
  public float orbitalVelocity;
  public float orbitalInclination;
  public float orbitalEccentricity;
  public float obliquityToOrbit;
  public float meanTemperature;
  public float surfacePressure;
  public int numberOfMoons;
  public string hasRingSystem;
  public string hasGlobalMagneticField;
}


public class satellite 
{
  public int id;
  public int planetId;
  public string name;
  public float gm;
  public float radius;
  public float density;
  public float magnitude;
  public float albedo;

}

public class orbit
{
  public string name;
  public string number;
  public string bodyItOrbits;
  public float distance; // thousands km
  public float orbitalPeriod; // days
  public float Incl;
  public float Eccen;
  public string Discoverer;
  public string date;
  public string altName;
}


public class moon 
{
  public int id;
  public int planetId;
  public string name;
  public float gm;
  public float radius;
  public float density;
  public float magnitude;
  public float albedo;

  public string bodyItOrbits;
  public float distance; // thousands km
  public float orbitalPeriod; // days
  public float Incl;
  public float Eccen;
  
}





class jstest {

  static void Main() 
  {

    var serializer = new JavaScriptSerializer();
    List<planet> ds = new List<planet>(); //serializer.Deserialize<List<planet>>(json);
    using (StreamReader r = new StreamReader("planets.json"))
    {
        string json = r.ReadToEnd();
        ds = serializer.Deserialize<List<planet>>(json);
    }


/*   foreach (planet aPart in ds)
          {
              Console.WriteLine(aPart.name);
          }
          */


    for(int i=0; i < 4; i++) {
        Console.WriteLine(ds[i].name);
    }


    List<satellite> dss = new List<satellite>(); //serializer.Deserialize<List<planet>>(json);
    using (StreamReader rr = new StreamReader("satellites.json"))
    {
        string json = rr.ReadToEnd();
        dss = serializer.Deserialize<List<satellite>>(json);
    }

    for(int i=0; i < 4; i++) {
        Console.WriteLine(dss[i].name);
    }



   Console.WriteLine("DDDDDDDDDDDD");
     List<orbit> olist = new List<orbit>(); 
    using (StreamReader rr = new StreamReader("orbitdata.json"))
    {
        string json = rr.ReadToEnd();
        olist = serializer.Deserialize<List<orbit>>(json);
    }

    foreach ( orbit oo in olist) {
      Console.WriteLine(oo.distance.ToString());
    }

    
    orbit o1;
    List<moon> moonList = new List<moon>(); 
    foreach(satellite ss in dss) {
      string name = ss.name;
      foreach (orbit oo in olist) {
        string nn = oo.name;
        if (nn == name) {
          o1 = oo;
          moon mm = new moon(); 
          mm.id = ss.id;
          mm.planetId = ss.planetId;
          mm.gm = ss.gm;
          mm.radius = ss.radius;
          mm.density = ss.density;
          mm.magnitude = ss.magnitude;
          mm.albedo = ss.albedo;

          mm.bodyItOrbits = o1.bodyItOrbits;
          mm.distance = o1.distance;
          mm.orbitalPeriod = o1.orbitalPeriod;
          mm.Incl = o1.Incl;
          mm.Eccen = o1.Eccen;

          moonList.Add(mm);
          //Console.WriteLine(name + " " + found.ToString());
        }
      }
    }

    


  }
}
