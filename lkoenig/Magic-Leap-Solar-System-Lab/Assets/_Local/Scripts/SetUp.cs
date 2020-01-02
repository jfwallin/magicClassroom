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
    private bool spawned = false;
    private void Awake()
    {
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
        
    }
    public void LateUpdate()
    {

        //trying to make this work
        if (Transmission.Peers.Length < 1 && !spawned)
        {
            Debug.Log("No Peer's found. Spawning system.");
            TransmissionObject system = Transmission.Spawn("EarthMoonSun", new Vector3(0, 0, 1), Quaternion.Euler(0, 0, 0), new Vector3(0.25f, 0.25f, 0.25f));
            system.motionSource = system.transform;
            spawned = true;
        }
        else
        {
            Debug.Log("You should have a system already.");
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
