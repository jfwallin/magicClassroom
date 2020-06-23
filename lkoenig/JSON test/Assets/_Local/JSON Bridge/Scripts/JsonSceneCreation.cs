using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.IO;
using MagicLeapTools;
using System.Runtime.CompilerServices;
using System.CodeDom;
using System;

//In progress!!!!
//I may rename this and change the way it's funtioning.


public class JsonSceneCreation : MonoBehaviour
{

    // Start is called before the first frame update
    void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {
        
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
        GameObject parent;
        JsonObject jsonObject;

        myObject = GameObject.CreatePrimitive(PrimitiveType.Sphere); //fix this later
        myObject.name = info.name;

        for(int i=0; i < info.numChildren; i++)
        {
            makeObject(info.children[i]);
        }

        for(int i = 0; i < info.numScriptsToAdd; i++)
        {
            myObject.AddComponent(Type.GetType(info.scriptsToAdd[i]));
            jsonObject = myObject.GetComponent(info.scriptsToAdd[i]) as JsonObject;
            jsonObject.Construct(info);
        }
    }

}
