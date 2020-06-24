using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.IO;


//Due to the nature of ObjectInfo at this moment this also has to be updated every time a new JsonObject script is added
public class MakeObjectInfo : MonoBehaviour
{
    
    public string path;
    public ObjectInfo info;

    // Start is called before the first frame update
    void Start()
    {
        //ObjectInfo info = new ObjectInfo();
        string json;
        path = path + info.name + ".json";

        StreamWriter writer = new StreamWriter(path);
        
        json = JsonUtility.ToJson(info);
        Debug.Log("json file is: " + json);

        writer.WriteLine(json);

        writer.Close();
    }

    
}
