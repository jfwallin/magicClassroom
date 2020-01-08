using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Orbit : MonoBehaviour
{
    //This will control the orbit of a celestial body


    //Public Variable
    public GameObject center; //Assigned in inspector. This is what the orbit is centered on.
    public float rotateDegree; //Treat as const
    
    //Private Variable
    private TimeManager timeManager; //Will allow speeding up/slowing down time
    private Vector3 offset;

    private void Awake()
    {
        if(center == null) //allows for no center
        {
            center = gameObject;
            rotateDegree = 0.0f;
        }
    }

    // Start is called before the first frame update
    void Start()
    {
        timeManager = GameObject.Find("Time Manager").GetComponent<TimeManager>(); //get s reference to the timeManager script
        //Debug.Log("In Start in Orbit. timeMultiplier = " + timeManager.TimeMultiplier);
        offset = transform.position - center.transform.position; //radius of orbit
    }

    private void FixedUpdate() //physics
    {
        float timeMultiplier = timeManager.TimeMultiplier;//get the current time multiplier

        Vector3 centerVector = center.transform.position; //get position relative to world 
        transform.position = offset + centerVector;

        transform.RotateAround(centerVector, Vector3.up, rotateDegree * timeMultiplier * Time.deltaTime);

        offset = transform.position - center.transform.position; //update offset
    }
}
