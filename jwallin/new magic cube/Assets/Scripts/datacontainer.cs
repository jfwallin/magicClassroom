using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class datacontainer : MonoBehaviour
{
    public Vector3 imageLocation1, imageLocation2;
    public Vector3 mylocation, myrotation;
    
    // Start is called before the first frame update
    void Start()
    {


        imageLocation1 = new Vector3(0.0f, 0.0f, 0.0f);
        imageLocation2 = new Vector3(0.0f, 0.0f, 0.0f);
        mylocation = new Vector3(0.0f, 0.0f, 0.0f);
        myrotation = new Vector3(0.0f, 0.0f, 0.0f);
        DontDestroyOnLoad(transform.gameObject);
    }

    // Update is called once per frame
    void Update()
    {
        
    }
}
