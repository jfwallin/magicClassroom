using System;
using System.Collections;
using System.Collections.Generic;
//using System.Diagnostics;
using System.IO;
using UnityEngine;

public class JSONTest0Logic : MonoBehaviour
{
    private JSONTest0 test = new JSONTest0();
    private string json;
    
    // Start is called before the first frame update
    void Start()
    {
        test.num = 13;
        test.name = "Laurel Koenig";

        json = JsonUtility.ToJson(test);
        Debug.Log(json+"In start");

        WriteString(json);
    }

    // Update is called once per frame
    void Update()
    {
        test.num++;
        json = JsonUtility.ToJson(test);

        Debug.Log(json);
    }


    /* Interesting things to noate about this meathod:
     * It doesn't overwrite the data already in the file.
     * It works.
     */
    private void WriteString(string toBeWritten)
    {
        string path = "Assets/_Local/Scripts/test.json";

        StreamWriter writer = new StreamWriter(path, true);
        writer.WriteLine(toBeWritten);
        writer.Close();
        Debug.Log("written string I hope");
    }
}
