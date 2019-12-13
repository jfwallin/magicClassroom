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

    private void Awake()
    {
        //I know this works
        //Debug.Log("In Awake. Spawning system.");
        //TransmissionObject system = Transmission.Spawn("EarthMoonSun", new Vector3(0, 0, 1), Quaternion.Euler(0, 0, 0), new Vector3(0.25f, 0.25f, 0.25f));

        //trying to make this work
         if (Transmission.Peers.Length >= 1)
         {
             Debug.Log("No Peer's found. Spawning system.");
             TransmissionObject system = Transmission.Spawn("EarthMoonSun", new Vector3(0, 0, 1), Quaternion.Euler(0, 0, 0), new Vector3(0.25f, 0.25f, 0.25f));
         }
         else
         {
             Debug.Log("Peer's found. You should have a system already.");
         }
         
    }


}
