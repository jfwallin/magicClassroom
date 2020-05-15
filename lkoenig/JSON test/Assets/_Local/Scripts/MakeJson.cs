using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.IO;

public class MakeJson : MonoBehaviour
{
    // Start is called before the first frame update
    void Start()
    {
        JSONTest1 test = new JSONTest1();
        string json;
        string path = "Assets/_Local/JSON files/CubeTest0.json";

        StreamWriter writer = new StreamWriter(path);

        test.id = "Cube";
        test.xPosition = 1f;
        test.yPosition = 1f;
        test.zPosition = 2f;
        test.color = "Blue";
        test.scale = 0.25f;
        test.script = "Pulse";
        test.min = 0.1f;
        test.max = 1.5f;

        json = JsonUtility.ToJson(test);
        Debug.Log("json file is: " + json);
        writer.WriteLine(json);

        writer.Close();
    }

    // Update is called once per frame
    void Update()
    {
        
    }
}
