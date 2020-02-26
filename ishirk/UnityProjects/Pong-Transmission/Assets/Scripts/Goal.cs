using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using MagicLeapTools;

public class Goal : MonoBehaviour
{
    public enum GoalType {Red, Blue}

    [SerializeField]
    private GoalType goalType = GoalType.Red;
    private string scoreKey
    {
        get
        {
            if (goalType == GoalType.Red)
                return "scoreRed";
            else
                return "scoreBlue";
        }
    }

    private void OnTriggerEnter(Collider other)
    {
        if(other.gameObject.name == "Ball")
        {
            other.GetComponent<TransmissionObject>().Despawn();
            if (Transmission.HasGlobalFloat(scoreKey))
                Transmission.SetGlobalFloat(scoreKey, Transmission.GetGlobalFloat(scoreKey) + 1f);
        }
    }
}
