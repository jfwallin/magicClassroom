using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using MagicLeapTools;

public class Rotate : MonoBehaviour
{
    public Vector3 rotationAngle; //treat as a const

    //FixedUpdate is better for physics cause it's time sensitive 
    void FixedUpdate()
    {
        transform.Rotate(Transmission.GetGlobalFloat("timeMultiplier") *rotationAngle * Time.deltaTime);
    }
   
}
