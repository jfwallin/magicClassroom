using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.IO;


//Due to the nature of ObjectInfo at this moment this also has to be updated every time a new JsonObject script is added
public class MakeObjectInfo : MonoBehaviour
{
    public string path;
    public string name;

    public Vector3 position;
    public Vector3 scale;
    public string material;

    public int numScriptsToAdd;
    public string[] scriptsToAdd;

    // Start is called before the first frame update
    void Start()
    {
        ObjectInfo info = new ObjectInfo();
        string json;
        path = path + name + ".json";

        StreamWriter writer = new StreamWriter(path);

        //Varriables
        info.name = name;

        info.position = position;
        info.scale = scale;
        info.material = material;

        info.numScriptsToAdd = numScriptsToAdd;
        info.scriptsToAdd = scriptsToAdd;
        //end variables

        json = JsonUtility.ToJson(info);
        Debug.Log("json file is: " + json);
        writer.WriteLine(json);

        writer.Close();
    }

    
}
