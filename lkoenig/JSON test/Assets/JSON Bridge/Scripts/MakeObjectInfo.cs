using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.IO;


//This script allows you to test the Bridge without a outside source.
//You can create the JSON in the inspector and then test if it is working corrrectly in one run in the Unity Editor.
public class MakeObjectInfo : MonoBehaviour
{
    private Bridge bridge = new Bridge();

    public string path; //Where is/will your json be located?
    public ObjectInfo info;

    // Start is called before the first frame update
    void Start()
    {
        string json;
        path = path + info.name + ".json";

        StreamWriter writer = new StreamWriter(path);
        
        json = JsonUtility.ToJson(info, true); //Uses Unity's built in Json serializer
        Debug.Log("json file is: " + json);

        writer.WriteLine(json);

        writer.Close();

        bridge.ParseJson(path);
    }

    
}
