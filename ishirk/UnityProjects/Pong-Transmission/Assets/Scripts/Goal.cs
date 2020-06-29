using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using MagicLeapTools;
using UnityEngine.SocialPlatforms.Impl;

public class Goal : MonoBehaviour
{
    //What color goal is this:
    public enum GoalType {Red, Blue, Null}

    [SerializeField]
    private GoalType goalType = GoalType.Null;

    private void Awake()
    {
        if (goalType == GoalType.Null)
            Debug.LogError("goal Type is not set");
    }

    //Easy way to get string key of global bool stored in Transmission
    private string scoreKey
    {
        get
        {
            if (goalType == GoalType.Red)
                return "scoreBlue";
            else //goalType == GoalType.Blue
                return "scoreRed";
        }
    }

    //Called when a collider hits the goal, if it is the ball it increments the score and resets the ball
    private void OnTriggerEnter(Collider other)
    {
        if(other.gameObject.name == "Ball(Clone)")
        {
            if(goalType == GoalType.Red)
                other.GetComponent<Ball>().ResetBall(Ball.BallResetType.BlueSide);
            else if(goalType == GoalType.Blue)
                other.GetComponent<Ball>().ResetBall(Ball.BallResetType.RedSide);
            Transmission.SetGlobalFloat(scoreKey, Transmission.GetGlobalFloat(scoreKey) + 1f);
        }
    }
}
