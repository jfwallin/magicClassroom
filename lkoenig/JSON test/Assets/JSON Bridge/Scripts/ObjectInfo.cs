using System.Collections;
using System.Collections.Generic;
using System.Collections.Specialized;
using UnityEngine;

/*
 * This class creates a JSON object that can be serialized/deserialized easily with Unities built in capabilities
 * One of the things about JSONs that is nice is that not every field/variable has to be filled. We will take advantage of that.
 */

[System.Serializable]
public class ObjectInfo
{
    public string name; 
    public string parentName;
    public string type; 

    public Vector3 position;
    public Vector3 scale;
    public string material; //Leaving material blank won't cause any problems and just won't render a material. 


    public int numScriptsToAdd;
    public string[] scriptsToAdd;

    public int numOfChildren;
    public ObjectInfo[] children;


    //Script variables follow

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
