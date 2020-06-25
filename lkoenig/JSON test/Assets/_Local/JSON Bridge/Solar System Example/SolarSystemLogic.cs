using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using MagicLeapTools;

public class SolarSystemLogic : JsonObject
{

    private const string GlobalTimeKey = "timeMultiplier";
    private const string GlobalHoldKey = "holdMultiplier";
    private const string GlobalSpawnedKey = "spawned";

    public override void Construct(ObjectInfo info)
    {
        Transmission.SetGlobalFloat(GlobalTimeKey, 1);
        Transmission.SetGlobalFloat(GlobalHoldKey, 1);
        //If the variable doesn't exist yet make it and set it to false
        if (!Transmission.HasGlobalBool(GlobalSpawnedKey))
        {
            Transmission.SetGlobalBool(GlobalSpawnedKey, false);
        }
    }
    
    /*
     * Reminder to figure out the controller thing.
     * That might just have to already be a part of the scen though
     * Esspecially since this will always be deployed to the Leaps.
     * 
    public ControlInput control;
    public GameObject endPoint;

    private void Awake()
    {
        control.OnTriggerDown.AddListener(HandleTriggerDown);
        control.OnBumperDown.AddListener(HandleBumperDown);
    }
    */

    // Start is called before the first frame update
    void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {
        
    }

    /*
     * Again, this is a reminder to deal with it later
     * 
    private void HandleBumperDown()//Pauses and playes the time.
    {

        if (Transmission.GetGlobalFloat(GlobalTimeKey) == 0)
        {
            Transmission.SetGlobalFloat(GlobalTimeKey, Transmission.GetGlobalFloat(GlobalHoldKey));
            Debug.Log("The multiplier was already 0");
        }
        else
        {
            Transmission.SetGlobalFloat(GlobalHoldKey, Transmission.GetGlobalFloat(GlobalTimeKey));
            Transmission.SetGlobalFloat(GlobalTimeKey, 0);
        }
    }

    private void HandleTriggerDown()
    {
        if (!Transmission.GetGlobalBool(GlobalSpawnedKey))
        {
            GameObject solarSystem = MakeSystem("System");
            solarSystem.transform.position = endPoint.transform.position;
            //Transmission.Spawn("Sun Earth Moon", endPoint.transform.position, Quaternion.Euler(0, 0, 0), new Vector3(0.25f, 0.25f, 0.25f));

            Transmission.SetGlobalBool(GlobalSpawnedKey, true);
            Debug.Log("Spawning system... spawned = " + Transmission.GetGlobalBool(GlobalSpawnedKey));
        }
        else
        {
            Debug.Log("You should already have a system. spawned = " + Transmission.GetGlobalBool(GlobalSpawnedKey));
        }
    }
    */
}
