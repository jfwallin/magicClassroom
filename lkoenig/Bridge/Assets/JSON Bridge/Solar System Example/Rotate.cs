using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using MagicLeapTools;
using System.Diagnostics;

public class Rotate : MonoBehaviour
{
    public Vector3 rotationAngle; //treat as a const

    /*I would like this to assign it's own variables upon call.
    public override void Construct(string info)
    {
        RotateInfo rInfo = getInfo<RotateInfo>(info); //Parses JSON with typed function in JsonObject class

        rotationAngle = rInfo.rotationAngle;
    }
    */

    //FixedUpdate is better for physics cause it's time sensitive 
    void FixedUpdate()
    {
        transform.Rotate(Transmission.GetGlobalFloat("timeMultiplier") *rotationAngle * Time.deltaTime);
    }
   
}
