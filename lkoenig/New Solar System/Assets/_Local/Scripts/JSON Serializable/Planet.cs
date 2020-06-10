using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[System.Serializable]
public class Planet
{
    public string name;

    public float xPosition;
    public float yPosition;
    public float zPosition;
    public float scale;
    public string material;

    public int numScriptsToAdd;
    public string []scriptName;

    //Variable related to the Orbit Script
    public string OrbitCenter;
    public float OrbitRotateDegree;

    //Variables related to the rotate script
    public Vector3 RotateRotationAngle;
}
