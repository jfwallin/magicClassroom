using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.XR.MagicLeap;

public class Player : MonoBehaviour
{
    public enum MagicState {Idle, ChargingFireball, FiringFireball}

    #region Public Variables
    public AttackHand rightHand;
    public ShieldHand leftHand;
    #endregion

    #region Private Variables
    private MagicState curMagicState = MagicState.Idle;
    [SerializeField]
    private Vector3 rightShoulderPosition;
    [SerializeField]
    private Vector3 leftShoulderPosition;
    private MLKeyPoseManager poseManager;
    private bool shieldActive = false;
    #endregion

    #region Properties
    public Vector3 RightShoulderPosition
    {
        get
        {
            return transform.TransformPoint(rightShoulderPosition);
        }
        set => rightShoulderPosition = value;
    }

    public Vector3 LeftShoulderPosition
    {
        get
        {
            return transform.TransformPoint(leftShoulderPosition);
        }
        set => leftShoulderPosition = value;
    }
    #endregion

    void Start()
    {
        if(rightHand == null | leftHand == null)
        {
            Debug.LogWarning("A hand is unassigned to the Player script, disabling");
            enabled = false;
        }

        MLHands.Start();
        poseManager = MLHands.KeyPoseManager;
        poseManager.OnHandKeyPoseBegin += OnHandKeyPoseBegin;
    }

    private void OnDestroy()
    {
        poseManager.OnHandKeyPoseBegin += OnHandKeyPoseBegin;
        MLHands.Stop();
    }

    /// <summary>
    /// Filters Keypose events into magic actions
    /// </summary>
    /// <param name="pose">recognized pose</param>
    /// <param name="hand">handedness</param>
    public void OnHandKeyPoseBegin(MLHandKeyPose pose, MLHandType hand)
    {
        Vector3 eventShoulderPosition;
        if(hand == MLHandType.Right)
        {
            eventShoulderPosition = rightShoulderPosition;

            if (curMagicState == MagicState.Idle)
            {
                if (pose == MLHandKeyPose.OpenHand)
                {
                    curMagicState = MagicState.ChargingFireball;
                    rightHand.ChargeFireball();
                }
            }
            else if (curMagicState == MagicState.ChargingFireball)
            {
                if (pose == MLHandKeyPose.NoPose)
                {
                    curMagicState = MagicState.Idle;
                    rightHand.StopChargingFireball();
                }
                else if (pose == MLHandKeyPose.Finger)
                {
                    curMagicState = MagicState.Idle;
                    rightHand.FireFireball();
                }
            }
        }
        else //hand = left
        {
            eventShoulderPosition = leftShoulderPosition;

            if(pose == MLHandKeyPose.Fist & shieldActive == false)
            {
                leftHand.FormShield();
                shieldActive = true;
            }
            else if(pose != MLHandKeyPose.Fist & shieldActive == true)
            {
                leftHand.DispellShield();
                shieldActive = false;
            }
        }
    }
}
