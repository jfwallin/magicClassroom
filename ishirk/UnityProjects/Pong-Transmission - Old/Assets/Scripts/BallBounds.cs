using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class BallBounds : MonoBehaviour
{
    //Called when the ball gets too far away from the arena, resets its position
    private void OnTriggerExit(Collider other)
    {
        if (other.name == "Ball")
        {
            other.GetComponent<Ball>().ResetBall(Ball.BallResetType.Neutral);
            other.attachedRigidbody.velocity = Vector3.zero;
        }
    }
}
