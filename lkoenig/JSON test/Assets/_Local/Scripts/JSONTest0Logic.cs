using System;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using UnityEngine;

public class JSONTest0Logic : MonoBehaviour
{
    private JSONTest0 test = new JSONTest0();
    private Planets planets = new Planets();

    private string json;
    private string planet;
    private static string path = "Assets/_Local/Scripts/test.json";
    private StreamWriter writer; 


    // Start is called before the first frame update
    void Start()
    {
        test.num = 13;
        test.name = "Laurel Koenig";

        writer = new StreamWriter(path);

        json = JsonUtility.ToJson(test);
        Debug.Log(json+"In start");

        readFile();
        WriteString(json);
    }

    // Update is called once per frame
    void Update()
    {
        test.num++;
        json = JsonUtility.ToJson(test);

        WriteString(json);
        //Debug.Log(json);
    }


    private void readFile()
    {
        StreamReader reader = new StreamReader("Assets/_Local/JSON files/planets.json");
        string line;
        string jsonPlanetsString="";

        while ((line = reader.ReadLine()) != null)
        {
            Debug.Log(line);
            //if ((line = reader.ReadLine()) == "{")
            {
                jsonPlanetsString += "{";
                Debug.Log("This should be a }: " + jsonPlanetsString);
                while ((line = reader.ReadLine()) != "},")
                {
                    jsonPlanetsString += line;
                }
                jsonPlanetsString += "}";
            }
            Debug.Log(jsonPlanetsString);
        }
        Debug.Log("end of read function");
    }

    /* Interesting things to note about this meathod:
     * input an seccond option "true" flag causes the file to not overwrite when you create a new instance.
     * example of above: writer = new StreamWriter(path, true);
     * It works.
     */
    private void WriteString(string toBeWritten)
    {
        writer.WriteLine(toBeWritten);

        //Debug.Log("written string I hope");
    }

    private void OnDestroy()
    {
        writer.Close();
    }
}
