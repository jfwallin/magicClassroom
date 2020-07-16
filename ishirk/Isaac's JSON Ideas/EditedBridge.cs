using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.IO;
using MagicLeapTools;
using System.Runtime.CompilerServices;
using System.CodeDom;
using System;
using System.Collections.Specialized;
using System.Runtime.Versioning;
using System.Security.Policy;


public class Bridge 
{
    //ParseJson can be called from outside the class to trigger the methods included here
    public void ParseJson(string path)
    {
        makeObject(getInfo(path));
    }

    //getInfo serializes an ObjectInfo object from a json at a path
    private ObjectInfo getInfo(string path)
    {
        StreamReader reader = new StreamReader(path);  
        string line;
        ObjectInfoCollection info = new ObjectInfoCollection();

        line = reader.ReadToEnd();
        info = JsonUtility.FromJson<ObjectInfoCollection>(line);
        return info;
    }

    //makeeObject goes through the json and creates the scene and all conected scripts from it.
    //We are assuming that the scene is set up with the camera, default lighting, and controller already present.
    private void makeObjects(ObjectInfoCollection info)
    {
        foreach(ObjectInfo obj in info)
        {
            GameObject myObject;
            GameObject parent = GameObject.Find(obj.parentName);
            JsonObject jsonObject;

            myObject = dealWithType(obj.type); //possibly fixed
            myObject.name = obj.name;

            for (int i = 0; i < obj.componentsToAdd.Length; i++)
            {
                //Parse once to get the name of the component
                ComponentName cName = JsonUtility.FromJson<ComponentName>(componentsToAdd[i]);
                //Add the component and get its reference, then deserialize the JSON again to set the variables
                myObject.AddComponent(Type.GetType(cName)) = JsonUtility.FromJson(componentsToAdd[i], Type.GetType(cName));
            }

            myObject.transform.position = obj.position;
            myObject.transform.localScale = obj.scale;
            myObject.transform.parent = parent.transform;
        }
    }

    //dealWithType allows us to instantiate various objects.
    private GameObject dealWithType(string type)
    {
        GameObject myObject;

        switch (type){
            case "empty":
                myObject = new GameObject();
                break;
            case "plane":
                myObject = GameObject.CreatePrimitive(PrimitiveType.Plane);
                break;
            case "cube":
                myObject = GameObject.CreatePrimitive(PrimitiveType.Cube);
                break;
            case "sphere":
                myObject=GameObject.CreatePrimitive(PrimitiveType.Sphere);
                break;
            case "capsule":
                myObject = GameObject.CreatePrimitive(PrimitiveType.Capsule);
                break;
            case "cylinder":
                myObject = GameObject.CreatePrimitive(PrimitiveType.Cylinder);
                break;
            default:
                //myObject = new GameObject(); //This line is for if you want the default to be an empty game object

                myObject = GameObject.Instantiate(Resources.Load(type)) as GameObject; //This line is for if you want the default to be loading a prefab
                //Note that the above line requires your prefab to be located in a resources folder.
                break;
        }

        return myObject;
    }
    //This whole method will likely have to change slightly when we start dealing with Transmission.


}
