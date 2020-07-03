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
        ObjectInfo info = new ObjectInfo();

        line = reader.ReadToEnd();
        info = JsonUtility.FromJson<ObjectInfo>(line);
        return info;
    }

    //makeeObject goes through the json and creates the scene and all conected scripts from it.
    //We are assuming that the scene is set up with the camera, default lighting, and controller already present.
    private void makeObject(ObjectInfo info)
    {
        GameObject myObject;
        GameObject parent = GameObject.Find(info.parentName);
        JsonObject jsonObject;

        myObject = dealWithType(info.type); //possibly fixed
        myObject.name = info.name;

        for(int i = 0; i < info.numScriptsToAdd; i++)
        {
            myObject.AddComponent(Type.GetType(info.scriptsToAdd[i]));
            jsonObject = myObject.GetComponent(info.scriptsToAdd[i]) as JsonObject;
            jsonObject.Construct(info);
        }

        for (int i = 0; i < info.numOfChildren; i++)
        {
            makeObject(info.children[i]);
        }

        myObject.transform.position = info.position;
        myObject.transform.localScale = info.scale;
        myObject.transform.parent = parent.transform;

        if (info.material != "")
        {
            Renderer rend = myObject.GetComponent<Renderer>();
            rend.material = Resources.Load<Material>(info.material); //material must be in a recources folder.
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
