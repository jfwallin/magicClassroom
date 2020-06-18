﻿using System.Collections.Generic;
using UnityEngine;
using MagicLeapTools;
using UnityEngine.UI;


/* This is based on the SpactialAlinmentExample.cs 
 * This is a test
 * Just want to see if I can get something working
 * 
 * Might try and rewrite this to use the globabl variables more carefully. Later. 
 *
 * Test
*/
public class SetUp : MonoBehaviour
{
    //private bool spawned = false;
    private const string GlobalSpawnedKey = "spawned";
    private const string GlobalTimeKey = "timeMultiplier";
    private const string GlobalHoldKey = "holdMultiplier";
    public ControlInput control;
    public GameObject endPoint; //will probably change out for a get component later


    private void Awake()
    {
        control.OnTriggerDown.AddListener(HandleTriggerDown);
        control.OnBumperDown.AddListener(HandleBumperDown);
    }

    
    private void Start()
    {
        //If the variable doesn't exist yet make it and set it to false
        if (!Transmission.HasGlobalBool(GlobalSpawnedKey))
        {
            Transmission.SetGlobalBool(GlobalSpawnedKey, false);
        }

        
        Transmission.SetGlobalFloat(GlobalTimeKey, 1);
        Transmission.SetGlobalFloat(GlobalHoldKey, 1);

        /*if (!Transmission.HasGlobalFloat("timeMultiplier"))
        {
            Transmission.SetGlobalFloat("timeMultiplier", 1f);
            Transmission.SetGlobalFloat("holdMultiplier", 1f);
        }
        */
        Debug.Log("In the start. spawned is "+ Transmission.GetGlobalBool(GlobalSpawnedKey)+". timeMultiplier is " + Transmission.GetGlobalFloat(GlobalTimeKey));
    }

    public void Update()
    {
   
    }

    private void HandleTriggerDown() //places object
    {
        if (!Transmission.GetGlobalBool(GlobalSpawnedKey)) {
            TransmissionObject solarSystem = Transmission.Spawn("Sun Earth Moon", endPoint.transform.position , Quaternion.Euler(0, 0, 0), new Vector3(0.25f, 0.25f, 0.25f));
    
            Transmission.SetGlobalBool(GlobalSpawnedKey, true);
            Debug.Log("Spawning system... spawned = " + Transmission.GetGlobalBool(GlobalSpawnedKey));
        }
        else
        {
            Debug.Log("You should already have a system. spawned = " + Transmission.GetGlobalBool(GlobalSpawnedKey));
        }
    }

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
        
}