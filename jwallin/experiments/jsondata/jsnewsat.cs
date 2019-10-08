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
  


  /*
  Mass (1024kg)
Diameter (km)
Density (kg/m3)
Gravity (m/s2)
Escape Velocity (km/s)
Rotation Period (hours)
Length of Day (hours)
Distance from Sun (106 km)
Perihelion (106 km)
Aphelion (106 km)
Orbital Period (days)
Orbital Velocity (km/s)
Orbital Inclination (degrees)
Orbital Eccentricity
Obliquity to Orbit (degrees)
Mean Temperature (C)
Surface Pressure (bars)
Number of Moons (number)
Ring System? (Yes/No)
Global Magnetic Field? (Yes/No) 
     */

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
 
/*
 planet: owning planet of the satellite
name: name of the satellite
gm: GM (km3/sec2)
radius: Mean radius (km)
density: Mean density (g/cm3)
magnitude: Magnitude V0 or R
albedo: Geometric Albedo



   */



}


class JSON_PrettyPrinter
{
    public static string Process(string inputText)
    {
        bool escaped = false;
        bool inquotes = false;
        int column = 0;
        int indentation = 0;
        Stack<int> indentations = new Stack<int>();
        int TABBING = 8;
        StringBuilder sb = new StringBuilder();
        foreach (char x in inputText)
        {
            sb.Append(x);
            column++;
            if (escaped)
            {
                escaped = false;
            }
            else
            {
                if (x == '\\')
                {
                    escaped = true;
                }
                else if (x == '\"')
                {
                    inquotes = !inquotes;
                }
                else if ( !inquotes)
                {
                    if (x == ',')
                    {
                        // if we see a comma, go to next line, and indent to the same depth
                        sb.Append("\r\n");
                        column = 0;
                        for (int i = 0; i < indentation; i++)
                        {
                            sb.Append(" ");
                            column++;
                        }
                    } else if (x == '[' || x== '{') {
                        // if we open a bracket or brace, indent further (push on stack)
                        indentations.Push(indentation);
                        indentation = column;
                    }
                    else if (x == ']' || x == '}')
                    {
                        // if we close a bracket or brace, undo one level of indent (pop)
                        indentation = indentations.Pop();
                    }
                    else if (x == ':')
                    {
                        // if we see a colon, add spaces until we get to the next
                        // tab stop, but without using tab characters!
                        while ((column % TABBING) != 0)
                        {
                            sb.Append(' ');
                            column++;
                        }
                    }
                }
            }
        }
        return sb.ToString();
    }

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
          mm.name = ss.name;
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

    string json1 =  new JavaScriptSerializer().Serialize(moonList);
    string json_pretty = JSON_PrettyPrinter.Process(json1);  
    Console.WriteLine(json_pretty);
    File.WriteAllText("./moons.json", json_pretty );

   


  }
}
