using System;
using System.Collections;
using System.Collections.Generic;
//using System.Diagnostics; //This just confused the system
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
    }

    // Update is called once per frame
    void Update()
    {
        test.num++;
        json = JsonUtility.ToJson(test);

        Debug.Log(json);
    }
}
