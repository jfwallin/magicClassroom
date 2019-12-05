using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.XR.MagicLeap;

public class FollowHand : MonoBehaviour
{
    public MLHandType handType;
    public MeshRenderer rend;
    public MeshRenderer shieldRenderer;
    public ParticleSystem system;
    public Player playerScript;
    public GameObject projectile;
    public GameObject shield;
    private MLHand hand;
    private MLHandKeyPose curPose;
    private Vector3 handNormal;

    void Start()
    {
        system.Stop();
        MLHands.Start();
        if (handType == MLHandType.Right)
        {
            hand = MLHands.Right;
        }
        else
        {
            hand = MLHands.Left;
        }

        curPose = hand.KeyPose;
        handNormal = Vector3.up;
        shieldRenderer.enabled = false;
    }

    private void OnDestroy()
    {
        shieldRenderer.enabled = false;
        MLHands.Stop();
        system.Stop();
    }

    // Update is called once per frame
    void Update()
    {
        if (hand.IsVisible)
        {
            rend.enabled = true;
            transform.position = hand.Center;
            if (CalculateHandNormal(hand, ref handNormal))
            {
                Quaternion newRotation;
                if (handType == MLHandType.Right)
                {
                    newRotation = Quaternion.LookRotation(handNormal, playerScript.rightHandForward);
                    transform.rotation = newRotation;
                    transform.Rotate(new Vector3(90, 0, 0), Space.Self);
                }
                else
                {
                    newRotation = Quaternion.LookRotation(-handNormal, playerScript.leftHandForward);
                    transform.rotation = newRotation;
                    transform.Rotate(new Vector3(-90, 0, 0), Space.Self);
                }
                
            }

            //if (hand.KeyPose == MLHandKeyPose.OpenHand && hand.KeyPoseConfidence > 0.9f)
            //{
            //    system.Play();
            //    curPose = MLHandKeyPose.OpenHand;
            //}
            //else if(hand.KeyPose == MLHandKeyPose.Finger && hand.KeyPoseConfidence > 0.9f && curPose == MLHandKeyPose.OpenHand)
            //{
            //    GameObject go = GameObject.Instantiate(projectile, transform.position, Quaternion.identity);
            //    Rigidbody rb = go.GetComponent<Rigidbody>();
            //    rb.velocity = Vector3.forward * 2;
            //    Destroy(go, 5f);
            //    curPose = hand.KeyPose;
            //}
            //else
            //    system.Stop();


            //Add code to calculate hand normals every frame the hand is visible.
        }
        else
        {
            rend.enabled = false;
            system.Stop();
        }
    }

    public void ChargeFireball()
    {
        system.Play();
    }

    public void StopChargingFireball()
    {
        ParticleSystemStopBehavior behavior = ParticleSystemStopBehavior.StopEmittingAndClear;
        system.Stop(true, behavior);
    }

    public void FireFireball(Vector3 direction)
    {
        GameObject go = GameObject.Instantiate(projectile, transform.position, Quaternion.identity);
        Rigidbody rb = go.GetComponent<Rigidbody>();
        rb.velocity = direction * 2;
        Destroy(go, 5f);
        ParticleSystemStopBehavior behavior = ParticleSystemStopBehavior.StopEmittingAndClear;
        system.Stop(true, behavior);
    }

    public void FormShield()
    {
        shieldRenderer.enabled = true;
    }

    public void DispellShield()
    {
        shieldRenderer.enabled = false;
    }


    /// <summary>
    /// Returns the vector normal of the hand, pointing perpendicular to the back of tha hand
    /// </summary>
    /// <param name="hand">what hand to calculate normals for</param>
    /// <returns></returns>
    private bool CalculateHandNormal(MLHand hand, ref Vector3 normal)
    {
        MLFinger[] fingerArray = new MLFinger[4];
        fingerArray[0] = hand.Index;
        fingerArray[1] = hand.Middle;
        fingerArray[2] = hand.Ring;
        fingerArray[3] = hand.Pinky;

        int lastFingerCheckedIndex = 0;

        Vector3 point1 = Vector3.zero;
        bool foundValidPoint = false;
        for(int i = 0; i<4; i++)
        {
            if(fingerArray[i].MCP.IsValid)
            {
                point1 = fingerArray[i].MCP.Position;
                lastFingerCheckedIndex = i;
                foundValidPoint = true;
                break;
            }
        }
        if(!foundValidPoint)
        {
            Debug.LogWarningFormat("Could not complete finding the normal of the {0} hand, using globa; up vector", hand.HandType.ToString());
            return false;
        }

        Vector3 point2 = Vector3.zero;
        foundValidPoint = false;
        for (int j = 3; j > lastFingerCheckedIndex; j--)
        {
            if(fingerArray[j].MCP.IsValid)
            {
                point2 = fingerArray[j].MCP.Position;
                foundValidPoint = true;
                break;
            }
        }
        if (!foundValidPoint)
        {
            Debug.LogWarningFormat("Could not complete finding the normal of the {0} hand, using globa; up vector", hand.HandType.ToString());
            return false;
        }

        Vector3 point1FromCenter = point1 - hand.Center;
        Vector3 point2FromCenter = point2 - hand.Center;
        if (hand.HandType == MLHandType.Right)
        {
            normal =  Vector3.Cross(point2FromCenter, point1FromCenter).normalized;
            return true;
        }
        else
        {
            normal =  Vector3.Cross(point1FromCenter, point2FromCenter).normalized;
            return true;
        }
    }
}
