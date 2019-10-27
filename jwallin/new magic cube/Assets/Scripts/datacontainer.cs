using System.Collections;
using System.Collections.Generic;
using UnityEngine;




//using System.Net;
//using System.Net.Sockets;



public class datacontainer : MonoBehaviour
{
    public Vector3 imageLocation1, imageLocation2;
    public Vector3 mylocation, myrotation;

    public Vector3 worldZero;
    public Vector3 worldVector;
    public float worldScale;
    public float worldTheta;



    
    // Start is called before the first frame update
    void Start()
    {

        imageLocation1 = new Vector3(0.0f, 0.0f, 0.0f);
        imageLocation2 = new Vector3(0.0f, 0.0f, 0.0f);
        mylocation = new Vector3(0.0f, 0.0f, 0.0f);
        myrotation = new Vector3(0.0f, 0.0f, 0.0f);

        
        worldZero = new Vector3(0.0f, 0.0f, 0.0f);
        worldVector = new Vector3(0.0f, 0.0f, 0.0f);
        worldScale = 0.0f;
        worldTheta = 0.0f;
        DontDestroyOnLoad(transform.gameObject);
    }



    public void calculateWorld()
    {
        worldZero = 0.5f * (imageLocation2 + imageLocation1);
        worldVector = imageLocation2 - imageLocation1;
        worldScale = worldVector.sqrMagnitude;
        worldTheta = Mathf.Atan2(worldVector[0], worldVector[2]) * Mathf.Rad2Deg + 90.0f;
        
    }





    // Update is called once per frame
    void Update()
    {
        
    }
}
