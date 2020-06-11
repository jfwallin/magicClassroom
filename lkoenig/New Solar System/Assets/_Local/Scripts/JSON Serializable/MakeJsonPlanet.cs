using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.IO;
using System.Collections.Specialized;

public class MakeJsonPlanet : MonoBehaviour
{
    public string name;

    public float xPosition;
    public float yPosition;
    public float zPosition;
    public float scale;
    public string material;

    public int numScriptsToAdd;
    public string[] scriptName;

    //Variable related to the Orbit Script
    public string OrbitCenter;
    public float OrbitRotateDegree;

    //Variables related to the rotate script
    public Vector3 RotateRotationAngle;

    // Start is called before the first frame update
    void Start()
    {
        Planet planet = new Planet();
        string json;
        string path = "Assets/_Local/JSON Files/" + name + ".json";

        StreamWriter writer = new StreamWriter(path);

        planet.name = name;
        planet.xPosition = xPosition;
        planet.yPosition = yPosition;
        planet.zPosition = zPosition;
        planet.scale = scale;
        planet.material = material;
        planet.numScriptsToAdd = 2;

        planet.scriptName = scriptName;
        
        planet.OrbitCenter = OrbitCenter;
        planet.OrbitRotateDegree = OrbitRotateDegree;

        planet.RotateRotationAngle = RotateRotationAngle;


        json = JsonUtility.ToJson(planet);
        Debug.Log("json file is: " + json);
        writer.WriteLine(json);

        writer.Close();
    }

}
