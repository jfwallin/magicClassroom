using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.XR.MagicLeap;

/// <summary>
/// Manages hand object position updates, hand normal calculation, movement smoothing, and hand marker visibility
/// </summary>
public class HandScript : MonoBehaviour
{
    #region Public Variables
    [Header("General Variables")]
    public MLHandType handType;                //Left/Right hand, Set in the inspector
    public MeshRenderer handMarkerRenderer;    //Renderer for the hand center marker sphere
    public Player playerScript;                //Script reference to get shoulder positions
    public Camera mainCamera;                  //used to check if hitting clipping plane
    #endregion

    #region Private Variables
    protected MLHand magicLeapHand;
    protected MLHandKeyPose pose;              //This stores the most confident value of the pose for the hand
    protected Vector3 forwardDirection;        //The pointing direction of the hand, used to aligh transforms
    private Vector3 handNormal;                //Vector perpendicular to the back of the hand, used to align transform
    private Vector3 shoulderPosition;          //Used to define the hand forward direction every frame

    [Header("Position Smoothing Variables")]
    [Range(0.001f, 0.1f)]
    [SerializeField]
    private float totalPositionThreshold = 0.03f;        //amount of total movement required before the position is updated
    [Range(0.0005f, 0.02f)]
    [SerializeField]
    private float positionFrameDeltaThreshold = 0.0055f; //amount of movement in one frame that updates position
    private Vector3 previousHandPosition;      //stores Hand center last frame for comparison
    [SerializeField]
    private float rotationLerpAmount = 0.5f;   //Amount the transform moves to match calculated transform every frame
    [SerializeField]
    private float positionLerpAmount = 0.85f;
    [SerializeField]
    private float clippingPlaneOffset = 0.2f;  //How far hand objects should move in front of the camera clipping plane
    private Plane clippingPlane;               //Constructed to test if hand objects are too close
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
        
        //Initial assignements
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
        forwardDirection = (transform.position - shoulderPosition).normalized;
        mainCamera = Camera.main;
        clippingPlane = new Plane();
        pose = MLHandKeyPose.NoPose;
        handNormal = Vector3.up;
        handMarkerRenderer.enabled = false;
        previousHandPosition = magicLeapHand.Center;
    }

    protected virtual void OnDestroy()
    {
        MLHands.Stop();
        handMarkerRenderer.enabled = false;
    }

    protected virtual void Update()
    {
        //set current shoulder position
        if (magicLeapHand.HandType == MLHandType.Right)
            shoulderPosition = playerScript.RightShoulderPosition;
        else
            shoulderPosition = playerScript.LeftShoulderPosition;

        //Only update hand if it is clearly visible
        if (magicLeapHand.IsVisible == true & magicLeapHand.HandConfidence > 0.9)
        {
            handMarkerRenderer.enabled = true;
            SmoothPositionUpdate();
            forwardDirection = (transform.position - shoulderPosition).normalized;
            //Only update orientation if we can successfully find a hand normal
            if (CalculateHandNormal(magicLeapHand, ref handNormal))
            {
                SmoothOrientationUpdate();
            }
        }
        else //Hand not visible
        {
            handMarkerRenderer.enabled = false;
        }
    }
    #endregion//UnityMethods

    #region Private Methods
    /// <summary>
    /// mitigates MLHand position noise by only updating for large enough movements, also makes sure hand objects don't hit camera's near clip plane
    /// </summary>
    private void SmoothPositionUpdate()
    {
        //Define clipping plane based on camera settings
        clippingPlane.SetNormalAndPosition(mainCamera.transform.forward, mainCamera.transform.position + mainCamera.transform.forward * mainCamera.nearClipPlane);

        float distanceFromShoulderToPlane;
        //If we move far enough or quickly enough, it will update the position
        if ((magicLeapHand.Center - transform.position).magnitude > totalPositionThreshold |
            (magicLeapHand.Center - previousHandPosition).magnitude > positionFrameDeltaThreshold)
        {
            Ray handRay = new Ray(shoulderPosition, magicLeapHand.Center - shoulderPosition);
            Vector3 targetPosition;
            //if the hand is within the clipping plane, move the target transform forward until it is not
            if (clippingPlane.Raycast(handRay, out distanceFromShoulderToPlane) && distanceFromShoulderToPlane + clippingPlaneOffset > (magicLeapHand.Center - shoulderPosition).magnitude)
                targetPosition = handRay.GetPoint(distanceFromShoulderToPlane + clippingPlaneOffset);
            else
                targetPosition = magicLeapHand.Center;
            //Soften movement
            transform.position = Vector3.Lerp(transform.position, targetPosition, positionLerpAmount);
        }
        else //Not enough movement, but we still check for camera clipping
        {
            Ray handRay = new Ray(shoulderPosition, transform.position - shoulderPosition);
            if (clippingPlane.Raycast(handRay, out distanceFromShoulderToPlane) && distanceFromShoulderToPlane + clippingPlaneOffset > (transform.position - shoulderPosition).magnitude)
                transform.position = handRay.GetPoint(distanceFromShoulderToPlane + clippingPlaneOffset);
        }

        previousHandPosition = magicLeapHand.Center;
    }

    /// <summary>
    /// Aligns the object transform with the calculated orientation of the hand.
    /// </summary>
    private void SmoothOrientationUpdate()
    {
        Quaternion newRotation;
        //creates an orientation locking the transform to the handNormal.
        //in order to perfectly lock to the hand normal, It must be first vector in the LookRotation method call, but
        //this alligns the z-axis with it, which is 90deg off of what it should be, the y-axis. So it is then rotated.
        newRotation = Quaternion.LookRotation(handNormal, -forwardDirection) * Quaternion.Euler(90f, 0f, 0f);
     
        //This softens the motion, making it look less jagged
        newRotation = Quaternion.Slerp(transform.rotation, newRotation, rotationLerpAmount);
        transform.rotation = newRotation;
    }

    /// <summary>
    /// finds the hand normal
    /// </summary>
    /// <param name="hand">hand to get joint position data from</param>
    /// <param name="normal">perpendicular to the back of the hand</param>
    /// <returns>False if unable to find enough joints, updates normal if true</returns>
    private bool CalculateHandNormal(MLHand hand, ref Vector3 normal)
    {
        //fingers to search
        MLFinger[] fingerArray = new MLFinger[4];
        fingerArray[0] = hand.Index;
        fingerArray[1] = hand.Middle;
        fingerArray[2] = hand.Ring;
        fingerArray[3] = hand.Pinky;
        
        int lastFingerCheckedIndex = 0;   //used to make sure the second point isn't a finger already used
        bool foundValidPoint = false;
        Vector3 point1 = Vector3.zero;

        //Searches through finger array, stopping if it finds a valid joint or all fail
        for (int i = 0; i < 4; i++)
        {
            if (fingerArray[i].MCP.Position != magicLeapHand.Center) //When it fails to track the joint, it moves it to hand center
            {
                point1 = fingerArray[i].MCP.Position;
                lastFingerCheckedIndex = i;
                foundValidPoint = true;
                break;
            }
        }
        //If no valid joint was found, or if only one joint is valid out of all 4 fingers, the normal can't be found
        if (!foundValidPoint | lastFingerCheckedIndex == 3)
        {
            //Debug.LogWarningFormat("Could not complete finding the normal of the {0} hand, failed finding 1st finger", hand.HandType.ToString());
            return false;
        }

        Vector3 point2 = Vector3.zero;
        foundValidPoint = false;

        //Starts cheacking from the opposite side to make sure the joints are as far apart as possible
        for (int j = 3; j > lastFingerCheckedIndex; j--)
        {
            if (fingerArray[j].MCP.Position != magicLeapHand.Center)
            {
                point2 = fingerArray[j].MCP.Position;
                foundValidPoint = true;
                break;
            }
        }
        //No valid joints were found
        if (!foundValidPoint)
        {
            //Debug.LogWarningFormat("Could not complete finding the normal of the {0} hand, failed finding 2nd finger", hand.HandType.ToString());
            return false;
        }

        Vector3 point1RelToHand;          //used to get vectors in the plane of the hand
        Vector3 point2RelToHand;

        //Using the wrist gets us longer vectors, minimizing noise in MLHand measurements
        //Assuming it resets to hand center if not tracked
        if (hand.Wrist.Center.Position != magicLeapHand.Center)
        {
            point1RelToHand = point1 - hand.Wrist.Center.Position;
            point2RelToHand = point2 - hand.Wrist.Center.Position;
        }
        else //Can't use the wrist
        {
            point1RelToHand = point1 - hand.Center;
            point2RelToHand = point2 - hand.Center;
        }
        //perform the cross and check for errors
        Vector3 tempNormal = Vector3.Cross(point1RelToHand, point2RelToHand).normalized;
        if(tempNormal == Vector3.zero)
        {
            //Debug.Log(point1RelToHand.ToString() + point2RelToHand.ToString() + "tempNormal was zero after cross product");
            return false;
        }
        else
        {
            //normal would be backwards on the left hand, so flip it
            normal = hand.HandType == MLHandType.Right ? tempNormal : -tempNormal;
            return true;
        }
    }
    #endregion//Private Methods
}
