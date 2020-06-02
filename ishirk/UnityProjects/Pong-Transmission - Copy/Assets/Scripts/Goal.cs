using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using MagicLeapTools;

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
                return "scoreRed";
            else //goalType == GoalType.Blue
                return "scoreBlue";
        }
    }

    //Called when a collider hits the goal, if it is the ball it increments the score and resets the ball
    private void OnTriggerEnter(Collider other)
    {
        if(other.gameObject.name == "Ball")
        {
            if(scoreKey == "scoreRed")
                other.GetComponent<Ball>().ResetBall(Ball.BallResetType.BlueSide);
            else //scoreKey == "scoreBlue"
                other.GetComponent<Ball>().ResetBall(Ball.BallResetType.RedSide);
            Transmission.SetGlobalFloat(scoreKey, Transmission.GetGlobalFloat(scoreKey) + 1f);
        }
    }
}
