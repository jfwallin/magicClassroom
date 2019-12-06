using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.XR.MagicLeap;

public class HandScript : MonoBehaviour
{
    #region Public Variables
    public MLHandType handType;              //Left/Right hand, Set in the inspector
    public MeshRenderer handMarkerRenderer;  //Renderer for the hand center marker
    public Player playerScript;              //Script reference to get shoulder positions
    #endregion

    #region Private Variables
    private MLHand magicLeapHand;
    private Vector3 handNormal;              //Vector perpendicular to the back of the hand
    protected Vector3 shoulderPosition;      //protected, because it is needed in some of the inheriting classes
    protected Vector3 firingDirection;
    [SerializeField]
    private float rotationLerpAmount = 0.5f;
    #endregion

    #region Unity Methods
    protected virtual void Start()
    {
        //Make sure public references have been assigned
        if(handMarkerRenderer == null)
        {
            handMarkerRenderer = GetComponent<MeshRenderer>();
            if(handMarkerRenderer == null)
            {
                Debug.LogWarning("No reference to a hand marker renderer on hand, disabling script");
                enabled = false;
            }
        }

        if(playerScript == null)
        {
            Debug.LogWarning("No playerScript assigned to hand, disabling script.");
            enabled = false;
        }
        
        //Initial assignements that depend on handedness
        MLHands.Start();
        if (handType == MLHandType.Right)
        {
            magicLeapHand = MLHands.Right;
            shoulderPosition = playerScript.RightShoulderPosition;
        }
        else
        {
            magicLeapHand = MLHands.Left;
            shoulderPosition = playerScript.LeftShoulderPosition;
        }
        firingDirection = (transform.position - shoulderPosition).normalized;

        handMarkerRenderer.enabled = false;
    }

    protected virtual void OnDestroy()
    {
        MLHands.Stop();
        handMarkerRenderer.enabled = false;
    }

    protected virtual void Update()
    {
        if (magicLeapHand.HandType == MLHandType.Right)
            shoulderPosition = playerScript.RightShoulderPosition;
        else
            shoulderPosition = playerScript.LeftShoulderPosition;
        firingDirection = (transform.position - shoulderPosition).normalized;

        if (magicLeapHand.IsVisible == true)
        {
            handMarkerRenderer.enabled = true;
            transform.position = magicLeapHand.Center;
            if(CalculateHandNormal(magicLeapHand, ref handNormal))
            {
                UpdateOrientation();
            }
        }
        else
        {
            handMarkerRenderer.enabled = false;
        }
    }
    #endregion//UnityMethods

    #region Private Methods
    /// <summary>
    /// This function aligns the object transform with the calculated orientation of the hand.
    /// </summary>
    private void UpdateOrientation()
    {
        Quaternion newRotation;
        //creates an orientation locking the transform to the handNormal.
        //in order to perfectly lock to the hand normal, It must be first vector in the LookRotation method call, but
        //this alligns the z-axis with it, which is 90deg off of what it should be, the y-axis. So it is rotated later.
        //Debug.Log(handNormal.ToString() + "," + firingDirection.ToString());
        newRotation = Quaternion.LookRotation(handNormal, -firingDirection);
     
        //This softens the motion, making it look less jagged
        newRotation = Quaternion.Lerp(transform.rotation, newRotation, rotationLerpAmount);
        transform.rotation = newRotation;
        transform.Rotate(new Vector3(0, 90, 0), Space.Self);
    }

    /// <summary>
    /// finds the hand normal
    /// </summary>
    /// <param name="hand">hand to get finger joint position data from</param>
    /// <param name="normal">perpendicular to the back of the hand</param>
    /// <returns>False if unable to find enough joints, updates normal if true</returns>
    private bool CalculateHandNormal(MLHand hand, ref Vector3 normal)
    {
        MLFinger[] fingerArray = new MLFinger[4];
        fingerArray[0] = hand.Index;
        fingerArray[1] = hand.Middle;
        fingerArray[2] = hand.Ring;
        fingerArray[3] = hand.Pinky;
        
        int lastFingerCheckedIndex = 0;
        bool foundValidPoint = false;
        Vector3 point1 = Vector3.zero;

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
        if (!foundValidPoint | lastFingerCheckedIndex == 3)
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

        Vector3 point1RelToHand;
        Vector3 point2RelToHand;

        if (hand.Wrist.Center.IsValid)
        {
            point1RelToHand = point1 - hand.Wrist.Center.Position;
            point2RelToHand = point2 - hand.Wrist.Center.Position;
        }
        else
        {
            point1RelToHand = point1 - hand.Center;
            point2RelToHand = point2 - hand.Center;
        }
        Vector3 tempNormal = Vector3.Cross(point1RelToHand, point2RelToHand).normalized;
        if(tempNormal == Vector3.zero)
        {
            Debug.Log(point1RelToHand.ToString() + point2RelToHand.ToString() + "tempNormal was zero after cross product");
            return false;
        }
        else
        {
            normal = hand.HandType == MLHandType.Right ? tempNormal : -tempNormal;
            return true;
        }
    }
    #endregion//Private Methods
}
