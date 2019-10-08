using System;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Net.NetworkInformation;
using System.Web.Script.Serialization;
using System.IO;
using System.Collections.Generic;
//using Newtonsoft.Json;


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


   foreach (planet aPart in ds)
          {
              Console.WriteLine(aPart.name);
          }
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
   /*foreach (satellite aPart in dss)
          {
              Console.WriteLine(aPart.name);
          }
*/

   Console.WriteLine("DDDDDDDDDDDD");
 
    List<orbit> olist = new List<orbit>();

    using( StreamReader reader = new StreamReader("sorbits.csv"))
    {
      while(!reader.EndOfStream)
      {
        string line = reader.ReadLine();
        Console.WriteLine("LINE = "+line);
        Console.WriteLine("kkkkkkkk");
        string[] values = line.Split(",");
        orbit orb = new orbit();
        orb.name          = values[0];
        orb.number        = values[1];
        orb.bodyItOrbits  = values[2];
  
        try 
        {
          orb.distance      = (float) Convert.ToDouble(values[3]);
        }
        catch (FormatException ex)
        {
          orb.distance = 0.0f;
        }

        try 
        {
          orb.orbitalPeriod = (float) Convert.ToDouble(values[4]);
        }
        catch (FormatException ex)
        {
          orb.orbitalPeriod = 0.0f;
        }

        try 
        {
          orb.Incl  = (float) Convert.ToDouble(values[5]);
        }
        catch (FormatException ex)
        {
          orb.Incl = 0.0f;
        }

        try 
        {
          orb.Eccen  = (float) Convert.ToDouble(values[6]);
        }
        catch (FormatException ex)
        {
          orb.Eccen = 0.0f;
        }

        Console.WriteLine(orb.distance.ToString() + " " + orb.orbitalPeriod.ToString() + " " +  orb.Incl.ToString()+ "  " );
// orb.orbitalPeriod = (float) Convert.ToDouble(values[4]);
//        orb.Incl          = (float) Convert.ToDouble(values[5]);
//        orb.Eccen         = (float) Convert.ToDouble(values[6]);
        orb.Discoverer    = values[7];
        orb.date          = values[8];
        orb.altName       = values[9];
        olist.Add(orb);

        }
        Console.WriteLine("");
        
      }



    string json1 =  new JavaScriptSerializer().Serialize(olist);
    string json_pretty = JSON_PrettyPrinter.Process(json1);  
    Console.WriteLine(json_pretty);
    File.WriteAllText("./orbitdata.json", json_pretty );

    Console.WriteLine( olist.Count.ToString());
    foreach ( orbit oo in olist) {
      Console.WriteLine(oo.distance.ToString());
    }

  }
}
