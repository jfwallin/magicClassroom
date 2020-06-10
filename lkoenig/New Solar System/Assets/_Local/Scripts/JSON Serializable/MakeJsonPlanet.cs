using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.IO;
using System.Collections.Specialized;

public class MakeJsonPlanet : MonoBehaviour
{
    // Start is called before the first frame update
    void Start()
    {
        Planet planet = new Planet();
        string json;
        string path = "Assets/_Local/JSON Files/Sun.json";

        StreamWriter writer = new StreamWriter(path);

        planet.name = "Sun";
        planet.xPosition = 0f;
        planet.yPosition = 0f;
        planet.zPosition = 0f;
        planet.scale = 1f;
        planet.material = "Sun";
        planet.numScriptsToAdd = 2;
        planet.scriptName = new string[planet.numScriptsToAdd];

        planet.scriptName[0] = "Orbit";
        //planet.OrbitCenter = "Earth";
        planet.OrbitRotateDegree = 0f;

        planet.scriptName[1] = "Rotate";
        planet.RotateRotationAngle = new Vector3(0,2.942379f,0);


        json = JsonUtility.ToJson(planet);
        Debug.Log("json file is: " + json);
        writer.WriteLine(json);

        writer.Close();
    }

}
