using System.Collections;
using System.Collections.Generic;
using System.Security.Policy;
using UnityEngine;

[System.Serializable]
public class Planets
{
    public int id;
    public string name;
    public float mass;
    public float diameter;
    public float density;
    public float gravity;
    public float escapeVelocity;
    public float rotationPeriod;
    public float lengthOfDay;
    public float distanceFromSun;
    public float perihelion;
    public float aphelion;
    public float orbitalPeriod;
    public float orbitalVelocity;
    public float orbitalInclination;
    public float orbitalEccentricity;
    public float obliquityToOrbit;
    public float meanTemperature;
    public float surfacePressure;
    public int numberOfMoons;
    public bool hasRingSystem;
    public bool hasGlobalMagneticField;
}
