using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.XR.MagicLeap;

public class FollowHand : MonoBehaviour
{
    public MLHandType handType;
    public MeshRenderer rend;
    public ParticleSystem system;
    public GameObject projectile;
    private MLHand hand;
    private MLHandKeyPose curPose;

    void Start()
    {
        MLHands.Start();
        if(handType == MLHandType.Right)
        {
            hand = MLHands.Right;
        }
        else
        {
            hand = MLHands.Left;
        }
        system.Stop();
        curPose = hand.KeyPose;
    }

    private void OnDestroy()
    {
        MLHands.Stop();
    }

    // Update is called once per frame
    void Update()
    {
        if (hand.IsVisible)
        {
            rend.enabled = true;
            transform.position = hand.Center;
            if (hand.KeyPose == MLHandKeyPose.OpenHand && hand.KeyPoseConfidence > 0.9f)
            {
                system.Play();
                curPose = MLHandKeyPose.OpenHand;
            }
            else if(hand.KeyPose == MLHandKeyPose.Finger && hand.KeyPoseConfidence > 0.9f && curPose == MLHandKeyPose.OpenHand)
            {
                GameObject go = GameObject.Instantiate(projectile, transform.position, Quaternion.identity);
                Rigidbody rb = go.GetComponent<Rigidbody>();
                rb.velocity = Vector3.forward * 2;
                Destroy(go, 5f);
                curPose = hand.KeyPose;
            }
            else
                system.Stop();
        }
        else
        {
            rend.enabled = false;
            system.Stop();
        }
    }
}
