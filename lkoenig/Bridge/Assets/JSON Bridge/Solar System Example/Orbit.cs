using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using MagicLeapTools;

public class Orbit : JsonObject
{
    //This will control the orbit of a celestial body
    

    //Public Variable
    public GameObject center; //This is what the orbit is centered on.
    public float rotateDegree; //Treat as const
    
    //Private Variable
    private Vector3 offset;

    //I'm fucking brilliant
    public override void Construct(string info)
    {
        OrbitInfo oInfo = getInfo<OrbitInfo>(info); //Calls Typed function to parse the JSON

        center = GameObject.Find(oInfo.center);
        rotateDegree = oInfo.rotateDegree;
    }

    // Start is called before the first frame update
    void Start()
    {
        if (center == null) //allows for no center
        {
            center = gameObject;
            rotateDegree = 0.0f;
        } 
    }

    private void FixedUpdate() //physics
    {
        offset = transform.position - center.transform.position; //radius of orbit

        Vector3 centerVector = center.transform.position; //get position relative to world 
        transform.position = offset + centerVector;

        transform.RotateAround(centerVector, Vector3.up, rotateDegree * Transmission.GetGlobalFloat("timeMultiplier") * Time.deltaTime);
    }
    
}
