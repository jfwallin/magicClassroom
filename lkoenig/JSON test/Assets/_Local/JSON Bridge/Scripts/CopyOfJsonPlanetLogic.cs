using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.IO;
using System.Collections.Specialized;
using System;
using MagicLeapTools;
using System.Runtime.CompilerServices;

public class CopyOfJsonPlanetLogic : MonoBehaviour
{
    private Helper helper = new Helper(); //Hold the help file
    private Planet info = new Planet(); //To be instantiated from
    private GameObject _dynamic; //This is grabbing this "folder" so I can place the objects after I make them
    private GameObject[] gameObject;

    private const string GlobalTimeKey = "timeMultiplier";
    private const string GlobalHoldKey = "holdMultiplier";
    private const string GlobalSpawnedKey = "spawned";

    public ControlInput control;
    public GameObject endPoint;

    private void Awake()
    {
        control.OnTriggerDown.AddListener(HandleTriggerDown);
        control.OnBumperDown.AddListener(HandleBumperDown);
    }

    // Start is called before the first frame update
    void Start()
    {
        _dynamic = GameObject.Find("[_DYNAMIC]");

        Transmission.SetGlobalFloat(GlobalTimeKey, 1);
        Transmission.SetGlobalFloat(GlobalHoldKey, 1);
        //If the variable doesn't exist yet make it and set it to false
        if (!Transmission.HasGlobalBool(GlobalSpawnedKey))
        {
            Transmission.SetGlobalBool(GlobalSpawnedKey, false);
        }
    }

    private void HandleBumperDown()//Pauses and playes the time.
    {

        if (Transmission.GetGlobalFloat(GlobalTimeKey) == 0)
        {
            Transmission.SetGlobalFloat(GlobalTimeKey, Transmission.GetGlobalFloat(GlobalHoldKey));
            Debug.Log("The multiplier was already 0");
        }
        else
        {
            Transmission.SetGlobalFloat(GlobalHoldKey, Transmission.GetGlobalFloat(GlobalTimeKey));
            Transmission.SetGlobalFloat(GlobalTimeKey, 0);
        }
    }

    private void HandleTriggerDown()
    {
        if (!Transmission.GetGlobalBool(GlobalSpawnedKey))
        {
            GameObject solarSystem = MakeSystem("System");
            solarSystem.transform.position = endPoint.transform.position;
            //Transmission.Spawn("Sun Earth Moon", endPoint.transform.position, Quaternion.Euler(0, 0, 0), new Vector3(0.25f, 0.25f, 0.25f));

            Transmission.SetGlobalBool(GlobalSpawnedKey, true);
            Debug.Log("Spawning system... spawned = " + Transmission.GetGlobalBool(GlobalSpawnedKey));
        }
        else
        {
            Debug.Log("You should already have a system. spawned = " + Transmission.GetGlobalBool(GlobalSpawnedKey));
        }
    }


    /*Notes:
     * Resources.Load(string) grabs things in the resources folder by name.
     * It can't be in a subfolder of resources
     * but you can have more than one resources file
     */
    private void getHelp(string path)
    {
        StreamReader reader = new StreamReader(path);//Makes a reader for the file at path
        string line; //makes a variable to hold the lines when we grab them

        line = reader.ReadLine(); //grabs line and puts it in line variable
        helper = JsonUtility.FromJson<Helper>(line); //deserializes the line grabbed into a json file
    }


    private void getInfo(string path)
    {
        StreamReader reader = new StreamReader(path);//Makes a reader for the file at path
        string line; //makes a variable to hold the lines when we grab them

        line = reader.ReadLine(); //grabs line and puts it in line variable
        info = JsonUtility.FromJson<Planet>(line); //deserializes the line grabbed into a json file

        //note that this will be more omplicated if the file has more than one json in it or more than one line in general.
    }

    private GameObject MakeSystem(string name)
    {
        GameObject myObject;
        getHelp("Assets/_Local/JSON Files/" + name + ".json");

        myObject = new GameObject(helper.name);

        //Object.transform.parent = parent.transform; //Puts the intantiated object in the proper location in the Hierarchy

        //myObject.transform.localScale = new Vector3(helper.scale, helper.scale, helper.scale); //Sets the scale based on in info given in the JSON

        gameObject = new GameObject[helper.numPlanetsToMake];

        for (int i = 0; i < helper.numPlanetsToMake; i++)
        {
            gameObject[i] = MakePlanet(helper.planetName[i], myObject);
        }

        if (helper.transmission)
        {
            myObject.AddComponent<TransmissionObject>();
        }

        myObject.transform.position = new Vector3(helper.xPosition, helper.yPosition, helper.zPosition);
        myObject.transform.localScale = new Vector3(helper.scale, helper.scale, helper.scale); //Sets the scale based on in info given in the JSON
        myObject.transform.parent = _dynamic.transform;

        return myObject;
    }

    private GameObject MakePlanet(string typeName, GameObject parent)
    {
        UnityEngine.Debug.Log("In the Make Object routine with " + typeName);
        GameObject myObject;
        JsonAssignment jsonAssignment; //Variable for the inbetween which aalows us to send the info to the scripts to add themselves

        getInfo("Assets/_Local/JSON Files/" + typeName + ".json"); //serializes the json data and makes it an obect that I can access. 

        myObject = GameObject.CreatePrimitive(PrimitiveType.Sphere);
        myObject.transform.position = new Vector3(info.xPosition, info.yPosition, info.zPosition);
        myObject.transform.parent = parent.transform; //Puts the intantiated object in the proper location in the Hierarchy
        myObject.name = info.name; //renames the gameobject in the hierarchy so it should be easier to find by other scripts

        Renderer rend = myObject.GetComponent<Renderer>(); //This grabs the renderer to change the material
        rend.material = Resources.Load<Material>(info.material); //This assigns the material

        myObject.transform.localScale = new Vector3(info.scale, info.scale, info.scale); //Sets the scale based on in info given in the JSON

        for (int i = 0; i < info.numScriptsToAdd; i++)
        {
            myObject.AddComponent(Type.GetType(info.scriptName[i])); //Should add the scripts to the object
            jsonAssignment = myObject.GetComponent(info.scriptName[i]) as JsonAssignment; //Grabs the component but as the type it's inheriting from
            jsonAssignment.Construct(info); //Becasue Construct is a virtuual function in JsonAssignment I can override it to be person in each script. This lets me pass the entire JSON object and have each script figure out what to dowith it.
        }
        //Note: The "Type.GetType("string")" is using a string to get find the proper script type and return it so that I can add it to the object

        return (myObject);

    }
}
