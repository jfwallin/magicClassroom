﻿using System.Collections;
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
    public string material;

    public int numOfChildren;
    public ObjectInfo[] children;

    public int numScriptsToAdd;
    public string[] scriptsToAdd;

    //Script variables follow

    /*A note: This is hardcoded for each script rn
     * That means that every time you add new scripts you have to manually add the appropriet variables here.
     * It aslo means you have to rewrite any respective JSONs.
     * 
     * Note: Check to see if JSONs can hold JSONs
     */

    public float PulseMin;
    public float PulseMax;

    //Orbit
    public string OrbitCenter;
    public float OrbitRotateDegree;

    //Rotate
    public Vector3 RotateRotationAngle;
}
