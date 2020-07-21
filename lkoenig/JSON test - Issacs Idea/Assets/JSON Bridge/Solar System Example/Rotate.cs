using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using MagicLeapTools;
using System.Diagnostics;

public class Rotate : JsonObject
{
    public Vector3 rotationAngle; //treat as a const

    //I would like this to assign it's own variables upon call.
    public override void Construct(ScriptVariables info)
    {
        rotationAngle = info.RotateRotationAngle;
    }

    //FixedUpdate is better for physics cause it's time sensitive 
    void FixedUpdate()
    {
        transform.Rotate(Transmission.GetGlobalFloat("timeMultiplier") *rotationAngle * Time.deltaTime);
    }
   
}
