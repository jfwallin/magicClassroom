using System.Collections;
using System.Collections.Generic;
using UnityEngine;


[System.Serializable]
public class ScriptVariables
{

    /*A note: This is hardcoded for each script rn
    * That means that every time you add new scripts you have to manually add the appropriet variables here.
    * You then need to make sure that the "construct" method is properly overridded and that you are assigning all the variables you should.
    * 
    * Naming convenion: when adding variables to this part of the JSON they should be named as ScriptNameVariableName. 
    * Where ScriptName is the actual name of the script, and VariableName perfectly matches the name of the corresponding variable in said script.
    */

    //Pulse: this is from the "Example"
    public float PulseMin;
    public float PulseMax;

    //Orbit: this is from the "Solar System Example"
    public string OrbitCenter;
    public float OrbitRotateDegree;

    //Rotate: this is from the "Solar System Example"
    public Vector3 RotateRotationAngle;
}
