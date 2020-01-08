using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class eXCodeStorage : MonoBehaviour
{
    //So this doesn't actually do anything. It just stores code blips that I wrote and might want later

    //This was a part of timemanager.
    //It iterated though the child gameobjects of the object it was attached to and changed the timeMultiplier component

    /*This will change the multiplier on all the game objects so that the proportions stays the same but you can mess with time (speed up/slow down/pause/reverse)
     * 
        Specifically note the way it loops through the child gameobjects and access thier components

        foreach(Transform child in transform)
        {
            Rotate rotate = child.gameObject.GetComponent<Rotate>();
            Orbit orbit = child.gameObject.GetComponent<Orbit>();

            Debug.Log("Before changing the multipier\n" + "orbit time is = " + orbit.timeMultiplier.ToString() + " and the rotate vector is = " + rotate.rotationAngle.ToString());

            rotate.timeMultiplier = newMultiplier;
            orbit.timeMultiplier = newMultiplier;

            Debug.Log("After changing the multiplier\n" + "orbit time is = " + orbit.timeMultiplier.ToString() + " and the rotate vector is = " + rotate.rotationAngle.ToString());
        }
    */

    /*This is just an example of finding a gameobject
     * private GameObject tMan; 
     * tMan = GameObject.Find("Time Manager"); //Finds the time manager and sets it's reference
     */
}
