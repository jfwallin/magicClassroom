using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.XR.MagicLeap;

public class SetupPhase : MonoBehaviour
{
    enum SetupState { Meshing, PlacingTargets, PlacingLaunchers, FightInstructions, Done}

    #region Public Variables
    public MLHandType controlHandedness;
    public GameObject HUD;
    public GameObject[] handObjects = new GameObject[2];
    [Header("Meshing Phase Variables")]
    public Material visibleMeshMaterial;
    public Material occlusionMeshMaterial;
    public MeshRenderer environmentMeshPrefabRenderer;
    [Header("Target Placement Phase Variables")]
    public GameObject targetPrefab;
    [Header("Launcher Placement Phase Variables")]
    public GameObject launcherPrefab;
    #endregion

    #region Private Variables
    private SetupState curState;
    private MLKeyPoseManager keyPoseManager;
    private bool confirming = false;
    private float timer = 0f;
    private List<GameObject> targets = new List<GameObject>();
    private List<GameObject> launchers = new List<GameObject>();
    #endregion

    #region Private Properties
    private MLHand Hand
    {
        get
        {
            if (controlHandedness == MLHandType.Right)
                return MLHands.Right;
            else
                return MLHands.Left;
        }
    }
    #endregion

    #region Unity Methods
    void Start()
    {
        environmentMeshPrefabRenderer.material = visibleMeshMaterial;
        //Initialize keypose tracking
        MLHands.Start();
        keyPoseManager = MLHands.KeyPoseManager;
        MLHandKeyPose[] initialPoses = { MLHandKeyPose.NoPose, MLHandKeyPose.Thumb };
        keyPoseManager.EnableKeyPoses(initialPoses, true, true);
        keyPoseManager.SetKeyPointsFilterLevel(MLKeyPointFilterLevel.Raw);
        keyPoseManager.SetPoseFilterLevel(MLPoseFilterLevel.ExtraRobust);
        curState = SetupState.Meshing;
    }

    private void OnDestroy()
    {
        MLHands.Stop();
    }

    void Update()
    {
        switch(curState)
        {
            case SetupState.Meshing:
                {
                    if(Hand.KeyPose == MLHandKeyPose.Thumb && Hand.KeyPoseConfidence > 0.8f)
                    {
                        if(confirming == false)
                        {
                            confirming = true;
                            timer = Time.time;
                        }
                        else if (Time.time - timer > 3f) //Start next phase if Thumbs up for 3 seconds
                        {
                            //Make world mesh invisible and occluding
                            environmentMeshPrefabRenderer.material = occlusionMeshMaterial;
                            foreach(MeshRenderer mr in GameObject.Find("Environment Mesh Parent").GetComponentsInChildren<MeshRenderer>())
                            {
                                mr.material = occlusionMeshMaterial;
                            }
                            //Add new poses
                            MLHandKeyPose[] newPoses = { MLHandKeyPose.Pinch, MLHandKeyPose.Ok };
                            keyPoseManager.EnableKeyPoses(newPoses, true);
                            //Enable next menu
                            Transform SetupUI = HUD.transform.GetChild(0);
                            SetupUI.GetChild(0).gameObject.SetActive(false); //Meshing menu
                            SetupUI.GetChild(1).gameObject.SetActive(true);  //Target placement menu
                            //reset confirmation variables
                            confirming = false;
                            timer = 0f;
                            //Set state
                            curState = SetupState.PlacingTargets;
                        }
                    }
                    else if(confirming == true) //Not Thumbs up hand pose
                    {
                        confirming = false;
                        timer = 0f;
                    }
                    break;
                }
            case SetupState.PlacingTargets:
                {
                    if(Hand.KeyPoseConfidence > 0.8f)
                    {
                        if (Hand.KeyPose == MLHandKeyPose.Pinch) //Place Shield
                        {
                            //If no shield, make one
                            if (transform.childCount == 0)
                            {
                                GameObject go = Instantiate(targetPrefab, transform);
                                go.transform.localScale = new Vector3(0.2f, 0.2f, 0.2f);
                                go.transform.GetChild(0).GetComponent<TargetScript>().enabled = false;
                            }
                            //place shield facing player
                            else if (transform.childCount == 1)
                            {
                                Transform targetToPlace = transform.GetChild(0);
                                targetToPlace.position = Hand.Index.KeyPoints[2].Position;
                                targetToPlace.rotation = Quaternion.LookRotation(Vector3.ProjectOnPlane(GameObject.Find("Player Body").transform.position - targetToPlace.position, Vector3.up)) * Quaternion.Euler(0f,-90f,0f);
                            }
                        }
                        //If a shield has been placed, lock it into position
                        else if(Hand.KeyPose == MLHandKeyPose.Ok && transform.childCount == 1)
                        {
                            StartCoroutine(LerpObjectSize(transform.GetChild(0).gameObject, new Vector3(0.5f,0.5f,0.5f), 1.5f));
                        }
                        else if(Hand.KeyPose == MLHandKeyPose.Thumb) 
                        {
                            if (confirming == false)
                            {
                                confirming = true;
                                timer = Time.time;
                            }
                            else if (Time.time - timer > 3f) //confirming == true
                            {
                                Transform SetupUI = HUD.transform.GetChild(0);
                                SetupUI.GetChild(1).gameObject.SetActive(false);  //Target placement menu
                                SetupUI.GetChild(2).gameObject.SetActive(true);   //Launcher placement menu
                                confirming = false;
                                timer = 0f;
                                curState = SetupState.PlacingLaunchers;
                            }
                        }
                    }
                    if (Hand.KeyPose != MLHandKeyPose.Thumb && confirming == true) //Try starting next phase
                    {
                        confirming = false;
                        timer = 0f;
                    }
                    break;
                }
            case SetupState.PlacingLaunchers:
                {
                    if (Hand.KeyPoseConfidence > 0.8f)
                    {
                        if (Hand.KeyPose == MLHandKeyPose.Pinch) //Place Launcher
                        {
                            //If no shield, make one
                            if (transform.childCount == 0)
                            {
                                GameObject go = Instantiate(launcherPrefab, transform);
                                go.transform.localScale = new Vector3(0.2f, 0.2f, 0.2f);
                                go.GetComponent<LauncherScript>().enabled = false;
                                launchers.Add(go);
                            }
                            //place launcher facing player
                            else if (transform.childCount == 1)
                            {
                                Transform targetToPlace = transform.GetChild(0);
                                targetToPlace.position = Hand.Index.KeyPoints[2].Position;
                                targetToPlace.rotation = Quaternion.LookRotation(Vector3.ProjectOnPlane(GameObject.Find("Player Body").transform.position - targetToPlace.position, Vector3.up)) * Quaternion.Euler(0f, -90f, 0f);
                            }
                        }
                        //If a shield has been placed, lock it into position
                        else if (Hand.KeyPose == MLHandKeyPose.Ok && transform.childCount == 1)
                        {
                            StartCoroutine(LerpObjectSize(transform.GetChild(0).gameObject, new Vector3(1f, 1f, 1f), 1.5f));
                        }
                        else if (Hand.KeyPose == MLHandKeyPose.Thumb)
                        {
                            if (confirming == false)
                            {
                                confirming = true;
                                timer = Time.time;
                            }
                            else if (Time.time - timer > 3f) //confirming == true
                            {
                                MLHandKeyPose[] newPoses = { MLHandKeyPose.Thumb, MLHandKeyPose.NoPose };
                                keyPoseManager.EnableKeyPoses(newPoses, true, true);
                                HUD.transform.GetChild(0).gameObject.SetActive(false);  //Set up instructions
                                HUD.transform.GetChild(1).gameObject.SetActive(true);   //fighting instructions
                                confirming = false;
                                timer = 0f;
                                curState = SetupState.FightInstructions;
                            }
                        }
                    }
                    if (Hand.KeyPose != MLHandKeyPose.Thumb && confirming == true) //Try starting next phase
                    {
                        confirming = false;
                        timer = 0f;
                    }
                    break;
                }
            case SetupState.FightInstructions:
                {
                    if (Hand.KeyPose == MLHandKeyPose.Thumb && Hand.KeyPoseConfidence > 0.8f)
                    {
                        if (confirming == false)
                        {
                            confirming = true;
                            timer = Time.time;
                        }
                        else if (Time.time - timer > 3f) //Start next phase if Thumbs up for 3 seconds
                        {
                            MLHandKeyPose[] newPoses = { MLHandKeyPose.Fist, MLHandKeyPose.Finger, MLHandKeyPose.OpenHand, MLHandKeyPose.NoPose };
                            keyPoseManager.EnableKeyPoses(newPoses, true, true);
                            handObjects[0].SetActive(true);
                            handObjects[1].SetActive(true);
                            HUD.transform.GetChild(1).gameObject.SetActive(false); //Fight instructions
                            HUD.transform.GetChild(2).gameObject.SetActive(true);  //Fight HUD
                            //reset confirmation variables
                            confirming = false;
                            timer = 0f;
                            //Set state
                            curState = SetupState.Done;
                        }
                    }
                    else if (confirming == true) //Not Thumbs up hand pose
                    {
                        confirming = false;
                        timer = 0f;
                    }
                    break;
                }
            case SetupState.Done:
                {
                    foreach(GameObject go in targets)
                    {
                        go.transform.GetChild(0).GetComponent<TargetScript>().enabled = true;
                    }
                    foreach(GameObject go in launchers)
                    {
                        go.GetComponent<LauncherScript>().enabled = true;
                    }
                    enabled = false;
                    break;
                }
        }
    }
    #endregion

    private IEnumerator LerpObjectSize(GameObject objectToResize, Vector3 targetSize, float timeTocomplete)
    {
        float startTime = Time.time;
        float percentComplete = 0f;
        while (percentComplete < 1)
        {
            objectToResize.transform.localScale = Vector3.Lerp(objectToResize.transform.localScale,targetSize, percentComplete);
            percentComplete = (Time.time - startTime) / timeTocomplete;
            yield return null;
        }
        transform.DetachChildren();
    }
}
