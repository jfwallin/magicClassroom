using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.IO;
using System.Collections.Specialized;
using System;
using MagicLeapTools;


public class JsonPlanetLogic : MonoBehaviour
{
    private Planet info = new Planet(); //To be instantiated from
    private GameObject _dynamic; //This is grabbing this "folder" so I can place the objects after I make them
    private GameObject[] gameObject = new GameObject[3];

    private const string GlobalTimeKey = "timeMultiplier";


    // Start is called before the first frame update
    void Start()
    {
        _dynamic = GameObject.Find("[_DYNAMIC]");
        Transmission.SetGlobalFloat(GlobalTimeKey, 1);

        gameObject[0] = MakeObject("Sun");
        gameObject[1] = MakeObject("Earth");
        gameObject[2] = MakeObject("Moon");
    }
    /*Notes:
     * Resources.Load(string) grabs things in the resources folder by name.
     * It can't be in a subfolder of resources
     * but you can have more than one resources file
     */


    // Update is called once per frame
    void Update()
    {

    }

    private void getInfo(string path)
    {
        StreamReader reader = new StreamReader(path);//Makes a reader for the file at path
        string line; //makes a variable to hold the lines when we grab them

        line = reader.ReadLine(); //grabs line and puts it in line variable
        info = JsonUtility.FromJson<Planet>(line); //deserializes the line grabbed into a json file

        //note that this will be more omplicated if the file has more than one json in it or more than one line in general.
    }
    
    private GameObject MakeObject(string typeName)
    {
        UnityEngine.Debug.Log("In the Make Object routine with " + typeName);
        GameObject myObject;
        //Pulse pp; //variable for dealing with the pulse script that will be added
        getInfo("Assets/_Local/JSON Files/" + typeName + ".json"); //serializes the json data and makes it an obect that I can access. 

        myObject = GameObject.CreatePrimitive(PrimitiveType.Sphere);
        myObject.transform.position = new Vector3(info.xPosition, info.yPosition, info.zPosition);
        myObject.transform.parent = _dynamic.transform; //Puts the intantiated object in the proper location in the Hierarchy
        myObject.name = info.name; //renames the gameobject in the hierarchy so it should be easier to find by other scripts
        
        Renderer rend = myObject.GetComponent<Renderer>(); //This grabs the renderer to change the material
        rend.material = Resources.Load<Material>(info.material); //This assigns the material

        myObject.transform.localScale = new Vector3(info.scale, info.scale, info.scale); //Sets the scale based on in info given in the JSON

        //myObject.AddComponent(GetType("Orbit"));
        
        /*
        for(int i = 0; i < info.numScriptsToAdd; i++)
        {
            myObject.AddComponent(Type.GetType(info.scriptName[i])); //Should add the scripts to the object
        }
        //Note: The "Type.GetType("string")" is using a string to get find the proper script type and return it so that I can add it to the object
        */

        //This is hardcodedd and will NEED a better solution at some point.
        myObject.AddComponent(Type.GetType(info.scriptName[0]));
        Orbit orbit = myObject.GetComponent<Orbit>();
        orbit.rotateDegree = info.OrbitRotateDegree;
        orbit.center = GameObject.Find(info.OrbitCenter);

        myObject.AddComponent(Type.GetType(info.scriptName[1]));
        Rotate rotate = myObject.GetComponent<Rotate>();
        rotate.rotationAngle = info.RotateRotationAngle;
        
        return (myObject);
        
    }
}
