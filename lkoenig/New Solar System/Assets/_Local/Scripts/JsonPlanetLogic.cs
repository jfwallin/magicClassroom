using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.IO;
using System.Collections.Specialized;
using System.Diagnostics;

public class JsonPlanetLogic : MonoBehaviour
{
    private Planet info = new Planet(); //To be instantiated from
    private GameObject _dynamic; //This is grabbing this "folder" so I can place the objects after I make them
    private GameObject[] gameObject = new GameObject[3];


    // Start is called before the first frame update
    void Start()
    {
        _dynamic = GameObject.Find("[_DYNAMIC]");

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
        UnityEngine.Debug.Log("In the Make Object routine");
        GameObject myObject;
        //Pulse pp; //variable for dealing with the pulse script that will be added
        getInfo("Assets/_Local/JSON Files/" + typeName + ".json"); //serializes the json data and makes it an obect that I can access. 

        myObject = GameObject.CreatePrimitive(PrimitiveType.Sphere);
        myObject.transform.position = new Vector3(info.xPosition, info.yPosition, info.zPosition);
        //myObject.transform.parent = _dynamic.transform; //Puts the intantiated object in the proper location in the Hierarchy
        myObject.name = info.name; //renames the gameobject in the hierarchy so it should be easier to find by other scripts
        
        Renderer rend = myObject.GetComponent<Renderer>(); //This grabs the renderer to change the material
        rend.material = Resources.Load<Material>(info.material); //This assigns the material

        myObject.transform.localScale = new Vector3(info.scale, info.scale, info.scale); //Sets the scale based on in info given in the JSON
        /*
        myObject.AddComponent(Type.GetType(info.script)); //Add the script to the object
        //Type.GetType(info.script) pp = myObject.GetComponent(Type.GetType(info.script)); //get the script so you can assign variables
        pp = myObject.GetComponent<Pulse>(); //assigns the script to the pulse variable so that I can edit the public pulse variables
        //pp = myObject.GetComponent(info.script) as Pulse;
        //pp = myObject.AddComponent<Pulse>();
        pp.min = info.min; //assing the min
        pp.max = info.max; //assign the max
        //Note: The "Type.GetType("string")" is using a string to get find the proper script type and return it so that I can add it to the object
        */
        return (myObject);
        
    }
}
