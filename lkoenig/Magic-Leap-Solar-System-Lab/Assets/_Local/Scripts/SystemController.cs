using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.XR.MagicLeap; 


//Will probably end up scrapping a lot of this

    /*In the meantime this need to size the simulation
     * This should be pretty easity acomplished as the entire system is child of an empty game object that this script is attached to
     * So I should be able to place and size the game object and as long as I change the scale equally it shouldn't mess anything up.
     * I think I need to project a circle and then place that circle and resize it. 
     * Then if it centers the system game object in the circle the sun will be in the center
     * Not sure how to scale/place the outer edges might just have to hardcode that with the math to the arbitrary proportion of my current object?
     * 
     * Alright. So I'm def going to redesign this later to make it better and more reusable.
     * right now though I just want to get something working.
     * Once I know what the code needs to look like I can better organize it.
     * 
     * Thinking about how I'm going to redesign this.
     * Will probably still have it do the raycaster/event handler thing that Isaac's does.
     * But instead of having the event handler only drag I want it to be diferent functionality based on object it's interacting with
     * Not sure how feasible that is. Or how useful for that matter. 
     * But I won't want it to drag the planets indiviually, but might want the drag feature for the "time" menu
     * Also will probably want to drag for sizing and placing
     * I'm going to have to look up info on templating in C#
     * Also need to think a bit more and plan farther along before I start coding.
     * 
     * I will probably get rid of this whole script.
     * 
     * I do have Isaac's package imported now. Haven't tested it yet and am going to have to think about how to impliment it.
     */
public class SystemController : MonoBehaviour
{

    private TimeManager timeManager; //Allows access to the time multiplier

    #region Unity Methods
    // Start is called before the first frame update
    void Start()
    {
        MLInput.Start(); //Start input
        
        timeManager = GameObject.Find("Time Manager").GetComponent<TimeManager>(); //get s reference to the timeManager script

        //Add button callbacks
        MLInput.OnControllerButtonDown += HandleOnButtonDown; //Adds HandleOnButtonDown to the event
    }

    private void OnDestroy()
    {
        MLInput.OnControllerButtonDown -= HandleOnButtonDown; //Removes button callback

        MLInput.Stop(); //Stops input
    }

    // Update is called once per frame
    void Update()
    {
        
    }
    #endregion

    #region Time Methods
    private void ChangeTimeMultiplier()
    {
        
    }

    public void ToggleTime() //This pauses/starts the simulation. Also might be somewhat unreasonable to have as it's own function.
    {
        timeManager.TimeMultiplier = 0f; //Should toggle the time multiplier through the set functionality. 
        Debug.Log("Toggle time multipler. Time multiplier now set to " + timeManager.TimeMultiplier);
    }

    public void HalfTime()
    {
        timeManager.TimeMultiplier *= .5f;
    }

    public void DoubleTime()
    {
        timeManager.TimeMultiplier *= 2f;
    }
    #endregion

    #region Physical Methods
    private void SizeAndPlace()
    {
        /*Not sure if this should be private or public yet actually
         * This will size and place the simulation
         */
        
    }
    #endregion

    #region Event Handlers
    //The callback functions must recieve the controller ID and button
    void HandleOnButtonDown(byte controller_id, MLInputControllerButton button)
    {
        if(button == MLInputControllerButton.Bumper) //If it's the bumper 
        {
            ToggleTime();
        }
    }
    #endregion
}
