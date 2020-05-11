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
    private static string path = "Assets/_Local/Scripts/test.json";
    private StreamWriter writer; 


    // Start is called before the first frame update
    void Start()
    {
        test.num = 13;
        test.name = "Laurel Koenig";

        writer = new StreamWriter(path);

        json = JsonUtility.ToJson(test);
        Debug.Log(json+"In start");

        WriteString(json);
    }

    // Update is called once per frame
    void Update()
    {
        test.num++;
        json = JsonUtility.ToJson(test);

        WriteString(json);
        Debug.Log(json);
    }


    /* Interesting things to noate about this meathod:
     * input an seccond option "true" flag causes the file to not overwrite when you create a new instance.
     * example of above: writer = new StreamWriter(path, true);
     * It works.
     */
    private void WriteString(string toBeWritten)
    {
        writer.WriteLine(toBeWritten);

        Debug.Log("written string I hope");
    }

    private void OnDestroy()
    {
        writer.Close();
    }
}
