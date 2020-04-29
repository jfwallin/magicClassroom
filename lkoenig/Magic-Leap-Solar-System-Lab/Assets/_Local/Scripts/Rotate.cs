using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Rotate : MonoBehaviour
{
    

    public Vector3 rotationAngle; //treat as a const

    //private TimeManager timeManager; //Will allow speeding up/slowing down time

    private void Awake()
    {
    
    }

    private void Start()
    {
        //timeManager = GameObject.Find("Time Manager").GetComponent<TimeManager>(); //get s reference to the timeManager script
    }

    // Update is called once per frame
    void FixedUpdate()
    {
        //float timeMultiplier = timeManager.TimeMultiplier; //get the current time multiplier
        transform.Rotate(Transmission.GetGlobalFloat("timeMultiplier") *rotationAngle * Time.deltaTime);
    }
   
}
