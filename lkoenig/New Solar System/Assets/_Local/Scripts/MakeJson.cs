using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.IO;

public class MakeJson : MonoBehaviour
{
    // Start is called before the first frame update
    void Start()
    {
        Planet planet = new Planet();
        string json;
        string path = "Assets/_Local/JSON Files/Earth.json";

        StreamWriter writer = new StreamWriter(path);

        planet.name = "Earth";
        planet.xPosition = 0f;
        planet.yPosition = 0f;
        planet.zPosition = 0f;
        planet.numScriptsToAdd = 3;
        //planet.scriptName[0] = "Orbit";
        //planet.scriptName[1] = "Rotate";
        //planet.scriptName[2] = "Transmission Object";

        json = JsonUtility.ToJson(planet);
        Debug.Log("json file is: " + json);
        writer.WriteLine(json);

        writer.Close();
    }

    // Update is called once per frame
    void Update()
    {
        
    }
}
