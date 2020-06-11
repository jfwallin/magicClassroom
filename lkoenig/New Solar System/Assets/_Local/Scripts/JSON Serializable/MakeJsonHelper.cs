using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.IO;

public class MakeJsonHelper : MonoBehaviour
{
    public string name;

    public float xPosition;
    public float yPosition;
    public float zPosition;
    public float scale;

    public int numPlanetsToMake;
    public string[] planetName;

    public bool transmission;

    void Start()
    {
        Helper helper = new Helper();
        string json;
        string path = "Assets/_Local/JSON Files/" + name + ".json";

        StreamWriter writer = new StreamWriter(path);

        helper.name = name;
        helper.xPosition = xPosition;
        helper.yPosition = yPosition;
        helper.zPosition = zPosition;
        helper.scale = scale;

        helper.numPlanetsToMake = numPlanetsToMake;
        helper.planetName = planetName;

        helper.transmission = transmission;

        json = JsonUtility.ToJson(helper);
        Debug.Log("json file is: " + json);
        writer.WriteLine(json);

        writer.Close();
    }
}
