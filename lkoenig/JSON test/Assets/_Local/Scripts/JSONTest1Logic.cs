using System;
using System.Collections;
using System.Collections.Generic;
using System.Collections.Specialized;
using System.IO;
using System.Runtime.Versioning;
using System.Security.Cryptography.X509Certificates;
using UnityEngine;


/* I want to test dynamically creating something from a JSON
 * 
 * This needs to read a .json file and then turn it into an object
 * following that it needs to then instantiate a GameObject using that info
 * 
 * The parsing of the JSON may eventually be it's own code but I'll probably leave it here for now.
 */
public class JSONTest1Logic : MonoBehaviour
{
    private JSONTest1 info = new JSONTest1(); //To be instantiated from
    private GameObject _dynamic; //This is grabbing this "folder" so I can place the objects after I make them
    private GameObject gameObject0;
    private GameObject gameObject1;


    // Start is called before the first frame update
    void Start()
    {
        _dynamic = GameObject.Find("[_DYNAMIC]");

        gameObject0 = MakeObject("Sphere");
        gameObject1 = MakeObject("Cube");
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
        info = JsonUtility.FromJson<JSONTest1>(line); //deserializes the line grabbed into a json file

        //note that this will be more omplicated if the file has more than one json in it or more than one line in general.
    }

    private GameObject MakeObject(string typeName)
    {
        GameObject myObject;
        Pulse pp; //variable for dealing with the pulse script that will be added
        getInfo("Assets/_Local/JSON files/" + typeName+"Test0.json"); //serializes the json data and makes it an obect that I can access. 

        myObject = (GameObject)Instantiate(Resources.Load(info.id), new Vector3(info.xPosition, info.yPosition, info.zPosition), Quaternion.identity);
        myObject.transform.parent = _dynamic.transform; //Puts the intantiated object in the proper location in the Hierarchy
        myObject.name = info.id; //renames the gameobject in the hierarchy so it should be easier to find by other scripts

        Renderer rend = myObject.GetComponent<Renderer>(); //This grabs the renderer to change the material
        rend.material = Resources.Load<Material>(info.color); //This assigns the material

        myObject.transform.localScale = new Vector3(info.scale, info.scale, info.scale); //Sets the scale based on in info given in the JSON

        myObject.AddComponent(Type.GetType(info.script)); //Add the script to the object
        //Type.GetType(info.script) pp = myObject.GetComponent(Type.GetType(info.script)); //get the script so you can assign variables
        pp = myObject.GetComponent<Pulse>(); //assigns the script to the pulse variable so that I can edit the public pulse variables
        //pp = myObject.GetComponent(info.script) as Pulse;
        //pp = myObject.AddComponent<Pulse>();
        pp.min = info.min; //assing the min
        pp.max = info.max; //assign the max
        //Note: The "Type.GetType("string")" is using a string to get find the proper script type and return it so that I can add it to the object

        InheritTest test = myObject.GetComponent("Pulse") as InheritTest;
        test.test();

        return (myObject);
    }

}
