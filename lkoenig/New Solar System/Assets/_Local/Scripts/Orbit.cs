using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using MagicLeapTools;

public class Orbit : MonoBehaviour
{
    //This will control the orbit of a celestial body
    

    //Public Variable
    public GameObject center; //Assigned in inspector. This is what the orbit is centered on.
    public float rotateDegree; //Treat as const
    
    //Private Variable
    private Vector3 offset;

    // Start is called before the first frame update
    void Start()
    {
        if (center == null) //allows for no center
        {
            center = gameObject;
            rotateDegree = 0.0f;
        }
        
        offset = transform.position - center.transform.position; //radius of orbit
    }

    private void FixedUpdate() //physics
    {
        Vector3 centerVector = center.transform.position; //get position relative to world 
        transform.position = offset + centerVector;

        transform.RotateAround(centerVector, Vector3.up, rotateDegree * Transmission.GetGlobalFloat("timeMultiplier") * Time.deltaTime);

        offset = transform.position - center.transform.position; //update offset
    }
    
}
