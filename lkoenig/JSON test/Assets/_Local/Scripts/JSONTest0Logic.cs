using System;
using System.Collections;
using System.Collections.Generic;
//using System.Diagnostics;
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
    }

    // Update is called once per frame
    void Update()
    {
        Debug.Log(json);
    }
}
