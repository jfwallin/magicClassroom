using System.Collections;
using System.Collections.Generic;
using UnityEngine;


/*Time neeeds to keep track of and (eventually) controll the time 
 * (na) right now this is attached to the "system" 
 * if I deide to attach to the prefab planets instead it will require some reworking.
 * That said I think it could attach to a controller without much issue (though still some)
 * 
 * It has been determined that Singleton's are "bad coding practice" and will be avoided
 * I don't /need/ this to be global as I am willing to make a reference where I need it
 * I will probably still make a logic loop to disallow more than a single instace but without any of the other Singleton oddnessS
 */
public class TimeManager : MonoBehaviour
{

    private long frameCount; //used to universally keep track of time
    private float timeMultiplier; //used to speed up/slow down time
    private float holdMulitpier; //hold the last known value of the time mulitpier for pausing


    public float TimeMultiplier {
        get => timeMultiplier;

        set{ //used to set the timeMultiplier
            if(value == 0) //When toggling the timeMultiplier you just call it with "0" and it should toggle
            {
                if(timeMultiplier == 0)
                {
                    timeMultiplier = holdMulitpier;
                }
                else
                {
                    holdMulitpier = timeMultiplier;
                    timeMultiplier = 0;
                }
            }
            else
            {
                timeMultiplier = value;
                holdMulitpier = timeMultiplier;
            }
        }
    }


    private void Awake()
    {
        timeMultiplier = 1;
        holdMulitpier = timeMultiplier;
        //garunties that it starts at whatever the hardcoded proportion is
    }

    // Start is called before the first frame update
    void Start()
    {
        frameCount = 0;
    }

    /*How am I going to keep time?
     * First thought was to get from the earth's rotation and base it on that
     * But with the multipliers on time and tables to make sure all the bodies have proportionally the corrent rotations/orbits
     * Since they are useing fixed updat to controll them and the time mulitiplier
     * I think I can use fixed update and the time multiplier to just hardcode the time here
     * and then it will be selfcontained here.
     */

    private void FixedUpdate()
    {
        frameCount++;
        //Debug.Log("FrameCount = " + frameCount);
        //Debug.Log("timeMultiplier = " + timeMultiplier);
    }

    public string GetTimeString()
    {
        /*This will convert time to string and return it in some format tbd
         * I need to decide exactly how I'm keeping time for this to make sense
         * The more I think about it the more I think I want to be able to return in multiple formats (min/dates/hour/day/ect.)
         * So this will eventually actually need to be multiple functions
         */
        string timeString = "Error: Function in Developement";
        return timeString; 
    }

}
