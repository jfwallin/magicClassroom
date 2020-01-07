using System.Collections.Generic;
using UnityEngine;
using MagicLeapTools;
using UnityEngine.UI;


/* This is based on the SpactialAlinmentExample.cs 
 * This is a test
 * Just want to see if I can get something working
*/
public class SetUp : MonoBehaviour
{
    //private bool spawned = false;
    private const string GlobalSpawnedKey = "spawned";
    public ControlInput control;
    private void Awake()
    {

        control.OnTriggerDown.AddListener(HandleTriggerDown);

        //I know this works
        //Debug.Log("In Awake. Spawning system.");
        //TransmissionObject system = Transmission.Spawn("EarthMoonSun", new Vector3(0, 0, 1), Quaternion.Euler(0, 0, 0), new Vector3(0.25f, 0.25f, 0.25f));

        /*trying to make this work
         if (Transmission.Peers.Length < 1)
         {
             Debug.Log("No Peer's found. Spawning system.");
             TransmissionObject system = Transmission.Spawn("EarthMoonSun", new Vector3(0, 0, 1), Quaternion.Euler(0, 0, 0), new Vector3(0.25f, 0.25f, 0.25f));
         }
         else
         {
             Debug.Log("Peer's found. You should have a system already.");
         }
         //If that works the next thing I need to work on is transfering ownership when aomeone leaves. 
         */
    }

    
    private void Start()
    {

        //If the variable doesn't exist yet make it and set it to false
        if (!Transmission.HasGlobalBool("spawned"))
        {
            Transmission.SetGlobalBool("spawned", false);
        }
    }
    public void Update()
    {
        //if (spawned)
        //{
            //send something telling other things to change flag to true
            //Transmission.SetGlobalBool()
        //}
        
    }

    private void HandleTriggerDown()
    {
        if (!Transmission.GetGlobalBool("spawned")) {
            TransmissionObject solarSystem = Transmission.Spawn("EarthMoonSun", control.Position, Quaternion.Euler(0, 0, 0), new Vector3(0.25f, 0.25f, 0.25f));
    
            Transmission.SetGlobalBool("spawned", true);
            Debug.Log("Spawning system... spawned = " + Transmission.GetGlobalBool("spawned"));
            //spawned = true;
        }
        else
        {
            Debug.Log("You should already have a system. spawned = " + Transmission.GetGlobalBool("spawned"));
        }
    }
        
}


/*Thinking longer term
 * 
 * I think the super dynamic from internet version will have to work in two phases
 * Phase one, instantiate the "celestial body" prefab at the right loccations/rotations/whatnot
 * Phase two, loop through and use a (JSON?) to set all the particulars like texture/orbit/whatnot
 * 
 * possibly hide this process with a "black curtain" by blocking this with a loading screen and that get's disabled at the end of phase two?
 * 
 * I also want to make a place phase that will be unique to each private key
 */
