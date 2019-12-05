using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.XR.MagicLeap;

public class HandScript : MonoBehaviour
{
    public MLHandType handType;
    public MeshRenderer handMarkerRenderer;

    private MLHand magicLeapHand;

    void Start()
    {
        MLHands.Start();
        if (handType == MLHandType.Right)
        {
            magicLeapHand = MLHands.Right;
        }
        else
        {
            magicLeapHand = MLHands.Left;
        }

        handMarkerRenderer.enabled = false;
    }

    private void OnDestroy()
    {
        MLHands.Stop();
        handMarkerRenderer.enabled = false;
    }

    void Update()
    {
        if(magicLeapHand.IsVisible == true)
        {
            handMarkerRenderer.enabled = true;
            transform.position = magicLeapHand.Center;
        }
        else
        {
            handMarkerRenderer.enabled = false;
        }
    }

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
        for (int i = 0; i < 4; i++)
        {
            if (fingerArray[i].MCP.IsValid)
            {
                point1 = fingerArray[i].MCP.Position;
                lastFingerCheckedIndex = i;
                foundValidPoint = true;
                break;
            }
        }
        if (!foundValidPoint)
        {
            Debug.LogWarningFormat("Could not complete finding the normal of the {0} hand, using globa; up vector", hand.HandType.ToString());
            return false;
        }

        Vector3 point2 = Vector3.zero;
        foundValidPoint = false;
        for (int j = 3; j > lastFingerCheckedIndex; j--)
        {
            if (fingerArray[j].MCP.IsValid)
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
            normal = Vector3.Cross(point2FromCenter, point1FromCenter).normalized;
            return true;
        }
        else
        {
            normal = Vector3.Cross(point1FromCenter, point2FromCenter).normalized;
            return true;
        }
    }
}
