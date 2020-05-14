using System.Collections;
using System.Collections.Generic;
using System;
using UnityEngine;
using UnityEngine.Assertions;
using UnityEngine.XR.MagicLeap;
using MagicLeapTools;

public class SceneControl : MonoBehaviour
{
    #region Variables
    [Header("Settings Object")]
    [SerializeField]
    private Settings gameSettings = null;

    //prefabs to instantiate or spawn
    [Header("Prefabs")]
    [SerializeField]
    private GameObject placementArenaPrefab = null;
    private const string arenaPrefabPath = "Prefabs/Arena";
    private const string ballPrefabPath = "Prefabs/Ball";
    private const string paddlePrefabPath = "Prefabs/Paddle";

    //references to scene instances of objects
    [Header("Scene References")]
    [SerializeField]
    private Transmission transmissionComponent = null;
    [SerializeField]
    private Pointer pointer = null;
    [SerializeField]
    private GameObject headposeMenus = null;

    private TransmissionObject paddle = null;
    private TransmissionObject ball = null;
    private TransmissionObject arena = null;
    private GameObject placementArena = null;

    private bool gamePaused = true;
    private bool gameOwner = false;
    #endregion //Variables

    //Called when change in connection status occurs, passing current connection status
    public event Action<bool> ConnectionEvent;

    #region Unity Methods
    //Reference checking
    private void Awake()
    {
        Assert.IsNotNull(gameSettings, "gameSettings not assigned on SceneControl");
        Assert.IsNotNull(placementArenaPrefab, "placementArena is not set on SceneControl");
        Assert.IsNotNull(transmissionComponent, "transmissionComponent is not set on SceneControl");
        Assert.IsNotNull(pointer, "pointer is not set on SceneControl");
        Assert.IsNotNull(headposeMenus, "headposeMenus is not set on SceneControl");
    }

    //Scene Initializations
    void Start()
    {
        transmissionComponent.gameObject.SetActive(false);
        pointer.gameObject.SetActive(true);
    }

    void Update()
    {
        if(!gamePaused && ball == null)
        {
            ball = Transmission.Spawn(ballPrefabPath, arena.transform.position + new Vector3(0, 1.5f, 0), Quaternion.identity, Vector3.one);
        }
    }
    #endregion //Unity Methods

    #region Public Functions
    //Move this to the ball script
    public void ApplySettings()
    {
        ball.transform.localScale = gameSettings.BallSize;
        ball.GetComponent<SphereCollider>().material = gameSettings.BallBounciness;
    }

    /// <summary>
    /// Called when the player selects "Join Game"
    /// </summary>
    public void TryJoinGame()
    {
        transmissionComponent.gameObject.SetActive(true);
        Transmission.Instance.OnPeerFound.AddListener(OnJoinGame);
    }

    /// <summary>
    /// Called when player cancels joining a game
    /// </summary>
    public void CancelJoinGame()
    {
        transmissionComponent.gameObject.SetActive(false);
        Transmission.Instance.OnPeerFound.RemoveListener(OnJoinGame);
    }

    /// <summary>
    /// Spawns stand-in arena to place, manages confirmation and cancellation
    /// </summary>
    public void StartPlacement()
    {
        //If the placement arena hasn't already been created, set it up
        if(!placementArena)
        {
            placementArena = Instantiate(placementArenaPrefab);
            placementArena.transform.SetParent(GameObject.Find("[_DYNAMIC]").transform);
        }
        //Cache references
        Placement arenaPlc = placementArena.GetComponent<Placement>();
        ControlInput cIScript = pointer.GetComponent<ControlInput>();
        //Start the placement session
        arenaPlc.Place(pointer.transform, new Vector3(3f, 2f, 2f), true, false, OnPlacementConfirm);
        //When the user pulls the trigger, attempt placement confirmation.
        cIScript.OnTriggerDown.AddListener(() => { 
            arenaPlc.Confirm();
            if(!arenaPlc.IsRunning)
            {
                cIScript.OnTriggerDown.RemoveAllListeners();
                cIScript.OnHomeButtonTap.RemoveAllListeners();
            }
        });
        //If home is pressed, remove arena and return to placement instructions
        cIScript.OnHomeButtonTap.AddListener(() => {
            GameObject.Destroy(placementArena);
            headposeMenus.SetActive(true);
            headposeMenus.transform.Find("PlacementInstructionsPage")?.gameObject.SetActive(true);
            cIScript.OnTriggerDown.RemoveAllListeners();
            cIScript.OnHomeButtonTap.RemoveAllListeners();
        });
    }

    public void StartGame()
    {
        Transmission.SetGlobalFloat("scoreRed", 0);
        Transmission.SetGlobalFloat("scoreBlue", 0);
        Transmission.Instance.OnGlobalFloatChanged.AddListener(HandleFloatChange);
    }

    /// <summary>
    /// Called when player exits from pause menu, despawns game and goes back to opening menu
    /// </summary>
    public void ExitGame()
    {
        //Reset menus, unregister hometap event
        headposeMenus.transform.GetChild(1).gameObject.SetActive(false);
        headposeMenus.transform.GetChild(0).gameObject.SetActive(true);
        headposeMenus.transform.GetChild(0).GetChild(0).gameObject.SetActive(true);
        pointer.GetComponent<ControlInput>().OnHomeButtonTap.RemoveListener(TogglePauseMenu);

        //Remove Paddle
        paddle.Despawn();

        if (gameOwner)
        {
            ball.Despawn();
            arena.Despawn();
            gameOwner = false;
        }

        Transmission.Instance.OnGlobalFloatChanged.RemoveListener(HandleFloatChange);
        transmissionComponent.gameObject.SetActive(false);
    }
    #endregion //Public Functions

    #region Event Handlers
    /// <summary>
    /// Called when a peer is succesfully found
    /// </summary>
    /// <param name="peerLabel"></param>
    public void OnJoinGame(string peerLabel)
    {
        //Notify other scripts of connection
        ConnectionEvent.Invoke(true);
        //Disable start menus
        headposeMenus.transform.GetChild(0).gameObject.SetActive(false);
        //Enable in game menus, show confirmation screen
        headposeMenus.transform.GetChild(1).gameObject.SetActive(true);

        ////Enable InGameMenuse and register togglePauseMenu to HomeTap event
        //headposeMenus.transform.GetChild(1).gameObject.SetActive(true);
        //pointer.GetComponent<ControlInput>().OnHomeButtonTap.AddListener(TogglePauseMenu);
        ////Spawn and initialize paddle
        //paddle = Transmission.Spawn(paddlePrefabPath, pointer.Origin, pointer.transform.rotation, Vector3.one);
        //paddle.ownershipLocked = true;
        //paddle.transform.SetParent(pointer.transform); //This is a non-physics way of achieveing movement. a physics joint may work better in the future
        //paddle.motionSource = pointer.transform;
    }

    private void OnPlacementConfirm(Vector3 pos, Quaternion rot)
    {
        headposeMenus.SetActive(true);
        GameObject confirmPage = headposeMenus.transform.Find("PlacementConfirmationPage")?.gameObject;
        if (!confirmPage)
            Debug.LogWarning("Could not find placement confirmation page");
        confirmPage.SetActive(true);
    }

    /// <summary>
    /// Called on home press, shows pause menu
    /// </summary>
    public void TogglePauseMenu()
    {
        GameObject pauseMenu = headposeMenus.transform.GetChild(1).GetChild(0).gameObject;
        if (gamePaused)
        {
            foreach (Transform t in pauseMenu.transform.parent)
            {
                t.gameObject.SetActive(false);
            }
            gamePaused = false;
        }
        else
        {
            pauseMenu.SetActive(true);
            gamePaused = true;
        }
    }

    private void HandleFloatChange(string key)
    {
        if(key == "scoreRed" && Transmission.GetGlobalFloat(key) == gameSettings.MaxScore)
        {
            //Red Wins
        }
        else if(key == "scoreBlue" && Transmission.GetGlobalFloat(key) == gameSettings.MaxScore)
        {
            //blue wins
        }

    }
    #endregion //Event Handlers
}
