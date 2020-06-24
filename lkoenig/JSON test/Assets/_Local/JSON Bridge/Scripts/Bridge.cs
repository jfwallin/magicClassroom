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

//In progress!!!!
//I may rename this and change the way it's funtioning.


public class Bridge : MonoBehaviour
{
    public void ParseJson(string path)
    {
        makeObject(getInfo(path));
    }

    private ObjectInfo getInfo(string path)
    {
        StreamReader reader = new StreamReader(path);
        string line;
        ObjectInfo info = new ObjectInfo();

        line = reader.ReadLine();
        info = JsonUtility.FromJson<ObjectInfo>(line);
        return info;
    }

    private void makeObject(ObjectInfo info)
    {
        GameObject myObject;
        GameObject parent = GameObject.Find(info.parentName);
        JsonObject jsonObject;

        myObject = dealWithType(info.type); //fix this later
        myObject.name = info.name;

        for(int i=0; i < info.numOfChildren; i++)
        {
            makeObject(info.children[i]);
        }

        for(int i = 0; i < info.numScriptsToAdd; i++)
        {
            myObject.AddComponent(Type.GetType(info.scriptsToAdd[i]));
            jsonObject = myObject.GetComponent(info.scriptsToAdd[i]) as JsonObject;
            jsonObject.Construct(info);
        }

        myObject.transform.position = info.position;
        myObject.transform.localScale = info.scale;
        myObject.transform.parent = parent.transform;

        Renderer rend = myObject.GetComponent<Renderer>();
        rend.material = Resources.Load<Material>(info.material); //material must be in a recources folder.
    }

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
                myObject = new GameObject();
                break;
        }

        return myObject;
    }

}
