using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.XR.MagicLeap;

public class Player : MonoBehaviour
{
    public enum MagicState {Idle, ChargingFireball, FiringFireball}
    MagicState curMagicState = MagicState.Idle;

    private bool shieldActive = false;

    public FollowHand rightHand;
    public FollowHand leftHand;

    public Vector3 rightShoulderPosition;
    public Vector3 leftShoulderPosition;

    private MLKeyPoseManager poseManager;

    public Vector3 rightHandForward
    {
        get
        {
            return (rightHand.transform.position - transform.TransformPoint(rightShoulderPosition)).normalized;
        }
    }

    public Vector3 leftHandForward
    {
        get
        {
            return (leftHand.transform.position - transform.TransformPoint(leftShoulderPosition)).normalized;
        }
    }

    // Start is called before the first frame update
    void Start()
    {
        MLHands.Start();
        poseManager = MLHands.KeyPoseManager;
        //List<MLHandKeyPose> initialKeyPoses = new List<MLHandKeyPose>();
        //initialKeyPoses.Add(MLHandKeyPose.NoPose);
        //initialKeyPoses.Add(MLHandKeyPose.OpenHand);
        //MLHandKeyPose[] temp_Poses = initialKeyPoses.ToArray();
        //poseManager.EnableKeyPoses(temp_Poses, true, true);

        poseManager.OnHandKeyPoseBegin += OnHandKeyPoseBegin;
    }

    private void OnDestroy()
    {
        poseManager.OnHandKeyPoseBegin += OnHandKeyPoseBegin;
        MLHands.Stop();
    }

    // Update is called once per frame
    void Update()
    {
        
    }

    public void OnHandKeyPoseBegin(MLHandKeyPose pose, MLHandType hand)
    {
        FollowHand eventHand;
        Vector3 eventShoulderPosition;
        if(hand == MLHandType.Right)
        {
            eventHand = rightHand;
            eventShoulderPosition = rightShoulderPosition;

            if (curMagicState == MagicState.Idle)
            {
                if (pose == MLHandKeyPose.OpenHand)
                {
                    curMagicState = MagicState.ChargingFireball;
                    eventHand.ChargeFireball();
                }
            }
            else if (curMagicState == MagicState.ChargingFireball)
            {
                if (pose == MLHandKeyPose.NoPose)
                {
                    curMagicState = MagicState.Idle;
                    eventHand.StopChargingFireball();
                }
                else if (pose == MLHandKeyPose.Finger)
                {
                    curMagicState = MagicState.Idle;
                    Vector3 direction = (eventHand.transform.position - transform.TransformPoint(eventShoulderPosition)).normalized;
                    eventHand.FireFireball(direction);
                }
            }
        }
        else
        {
            eventHand = leftHand;
            eventShoulderPosition = leftShoulderPosition;

            if(pose == MLHandKeyPose.Fist & shieldActive == false)
            {
                eventHand.FormShield();
                shieldActive = true;
            }
            else if(pose != MLHandKeyPose.Fist & shieldActive == true)
            {
                eventHand.DispellShield();
                shieldActive = false;
            }
        }

        
    }
}
