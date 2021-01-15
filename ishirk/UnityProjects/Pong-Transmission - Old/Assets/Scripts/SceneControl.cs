using System.Collections;
using System.Collections.Generic;
using System;
using UnityEngine;
using UnityEngine.Animations;
using UnityEngine.Assertions;
using UnityEngine.XR.MagicLeap;
using MagicLeapTools;

public class SceneControl : MonoBehaviour
{
    #region Variables
    [Header("Settings Object")]
    [SerializeField]
    private Settings gameSettings = null;

    [Header("World Mesh Materials")]
    [SerializeField]
    private Material occlusionMat = null;
    [SerializeField]
    private Material colorfulMat = null;

    //prefabs to instantiate or spawn
    [Header("Prefabs")]
    [SerializeField]
    private GameObject placementArenaPrefab = null;
    private const string arenaPrefabPath = "Prefabs/Arena";
    private const string ballPrefabPath = "Prefabs/Ball";
    [SerializeField]
    private GameObject ballPrefab = null;
    private const string paddlePrefabPath = "Prefabs/Paddle";

    //references to scene objects
    [Header("Scene References")]
    [SerializeField]
    private MeshRenderer worldMeshOriginal = null; //Reference to world mesh renderer to allow for visibility toggling
    [SerializeField]
    private Pointer pointer = null;                //Used to access controller events and transform
    [SerializeField]
    private GameObject headposeMenus = null;       //Reference to headpose menus, allows for easy visibility toggling
    [SerializeField]
    private GameObject pauseMenu = null;           //Ingame pause menu, cached for its frequent calls
    [SerializeField]
    private GameObject gameOverMenu = null;        //Game Over page, reference to enable quickly after the game ends

    //references to scene objects after instantiation
    private TransmissionObject paddle = null;
    private TransmissionObject ball = null;
    private TransmissionObject arena = null;
    private GameObject placementArena = null;

    //state variables
    private bool gameOwner = false;
    #endregion //Variables

    public Transform ArenaTransform { get => arena?.transform; }

    //Called when change in connection status occurs, passing current connection status
    public event Action<bool> ConnectionEvent;

    #region Unity Methods
    //Reference checking
    private void Awake()
    {
        Assert.IsNotNull(gameSettings, "gameSettings not assigned on SceneControl");
        Assert.IsNotNull(occlusionMat, "occlusionMat not assigned on SceneControl");
        Assert.IsNotNull(colorfulMat, "colorfulMat not assigned on SceneControl");
        Assert.IsNotNull(placementArenaPrefab, "placementArena is not set on SceneControl");
        Assert.IsNotNull(ballPrefab, "ballPrefab is not assigned on SceneControl");
        Assert.IsNotNull(worldMeshOriginal, "worldMeshOriginal is not assigned on SceneControl");
        Assert.IsNotNull(pointer, "pointer is not set on SceneControl");
        Assert.IsNotNull(headposeMenus, "headposeMenus is not set on SceneControl");
    }

    //Scene Initializations
    void Start()
    {
        Transmission.Instance.gameObject.SetActive(false);
        pointer.gameObject.SetActive(true);
    }

    void Update()
    {
        ;
    }
    #endregion //Unity Methods

    #region Public Functions
    /// <summary>
    /// Called When starting to set up a new game, shows the world mesh
    /// </summary>
    public void StartMeshing()
    {
        //Make mesh visible
        worldMeshOriginal.material = colorfulMat;
        //Add responses to controller input
        ControlInput cIScript = pointer.GetComponent<ControlInput>();
        //When trigger pulled, move on to next setup step
        cIScript.OnTriggerDown.AddListener(() =>
        {
            //Switch mesh from visible to occluding
            worldMeshOriginal.material = occlusionMat;
            //Enable next menu
            headposeMenus.transform.GetChild(0).Find("MeshingConfirmationPage").gameObject.SetActive(true);
            //Unsubscribe events
            cIScript.OnTriggerDown.RemoveAllListeners();
            cIScript.OnHomeButtonTap.RemoveAllListeners();
        });
        //When home button tapped, go back
        cIScript.OnHomeButtonTap.AddListener(() =>
        {
            //Switch mesh from visible to occluding
            worldMeshOriginal.material = occlusionMat;
            //Enable previous menu
            headposeMenus.transform.GetChild(0).Find("MeshingInstructionsPage").gameObject.SetActive(true);
            //Unsubscribe events
            cIScript.OnTriggerDown.RemoveAllListeners();
            cIScript.OnHomeButtonTap.RemoveAllListeners();
        });
    }

    /// <summary>
    /// Spawns stand-in arena to place, manages confirmation and cancellation
    /// </summary>
    public void StartPlacement()
    {
        //If the placement arena hasn't already been created, set it up
        if (!placementArena)
        {
            placementArena = Instantiate(placementArenaPrefab);
            placementArena.transform.SetParent(GameObject.Find("[_DYNAMIC]").transform);
        }
        //Cache references
        Placement arenaPlc = placementArena.GetComponent<Placement>();
        ControlInput cIScript = pointer.GetComponent<ControlInput>();
        //Start the placement session, OnPlacementConfirm is called when succesful placement occurs
        arenaPlc.Place(pointer.transform, new Vector3(3f, 2f, 2f), true, false, OnPlacementConfirm);
        //When the user pulls the trigger, attempt placement confirmation.
        cIScript.OnTriggerDown.AddListener(() => {
            arenaPlc.Confirm();
            if (!arenaPlc.IsRunning)
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

    /// <summary>
    /// Called by the game owner after setting the rules. Spawns the Arena and Ball
    /// </summary>
    public void CreateGame()
    {
        //Update state
        gameOwner = true;
        //Enable Transmission
        Transmission.Instance.gameObject.SetActive(true);
        //Prepare to handle connection event
        Transmission.Instance.OnPeerFound.AddListener(OnJoinGame);
        //Spawn Transmission Arena
        Transform arenaTransform = placementArena.transform;
        GameObject.Destroy(placementArena);
        arena = Transmission.Spawn(arenaPrefabPath, arenaTransform.position, arenaTransform.rotation, arenaTransform.localScale);
        arena.transform.SetParent(GameObject.Find("[_DYNAMIC]").transform);
        //Spawn Transmission ball
        ball = Transmission.Spawn(ballPrefabPath, arenaTransform.position + new Vector3(0, 1.25f, 0), Quaternion.identity, Vector3.one);
        ball.transform.SetParent(GameObject.Find("[_DYNAMIC]").transform);
    }

    /// <summary>
    /// Undos create Game. Removes Transmission objects and disables transmission
    /// </summary>
    public void CancelCreateGame()
    {
        //Update state
        gameOwner = false;
        //Unsubscribe from connection event
        Transmission.Instance.OnPeerFound.RemoveListener(OnJoinGame);
        //Despawn transmission objects
        ball.Despawn();
        arena.Despawn();
        //Disable transmission
        Transmission.Instance.gameObject.SetActive(false);
    }

    /// <summary>
    /// Called when the player selects "Join Game"
    /// </summary>
    public void TryJoinGame()
    {
        Transmission.Instance.gameObject.SetActive(true);
        Transmission.Instance.OnPeerFound.AddListener(OnJoinGame);
    }

    /// <summary>
    /// Called when player cancels joining a game
    /// </summary>
    public void CancelJoinGame()
    {
        Transmission.Instance.OnPeerFound.RemoveListener(OnJoinGame);
        Transmission.Instance.gameObject.SetActive(false);
    }

    /// <summary>
    /// Called when player confirms readiness (that they can see the ball and arena) to play.
    /// Spawns the player's own paddle, and then gets ready to respond to the other player
    /// also confirming readiness.
    /// </summary>
    public void ConfirmReadiness()
    {
        //Spawn transmission object paddle
        paddle = Transmission.Spawn(paddlePrefabPath, pointer.transform.position, pointer.transform.rotation, Vector3.one);
        paddle.ownershipLocked = true;
        paddle.transform.SetParent(GameObject.Find("[_DYNAMIC]").transform);
        //Make it move with the controller
        ParentConstraint paddlePC = paddle.GetComponent<ParentConstraint>();
        ConstraintSource pointerSource = new ConstraintSource();
        pointerSource.sourceTransform = pointer.transform;
        pointerSource.weight = 1f;
        paddlePC.AddSource(pointerSource);
        for (int i = 0; i < 3; i++)
        {
            paddlePC.translationOffsets[i] = Vector3.zero;
            paddlePC.rotationOffsets[i] = Vector3.zero;
        }
        paddlePC.locked = true;
        paddlePC.constraintActive = true;

        //Start listening for bool changes to check if both players are ready
        Transmission.Instance.OnGlobalBoolChanged.AddListener(CheckReadiness);

        //Set applicable bool
        if (gameOwner)
            Transmission.SetGlobalBool("HostReady", true);
        else
            Transmission.SetGlobalBool("PeerReady", true);
    }

    /// <summary>
    /// Called when both players have confirmed their readiness. Closes the menu,
    /// and sets up Transmission Events and variables
    /// </summary>
    public void StartGame()
    {
        //Close the ConfirmReadinessPage
        headposeMenus.transform.GetChild(1).GetChild(0).gameObject.SetActive(false);

        //manage global variables and event subscriptions with Transmission
        Transmission.Instance.OnGlobalBoolChanged.RemoveListener(CheckReadiness);
        if(gameOwner)
        {
            Transmission.SetGlobalFloat("scoreRed", 0);
            Transmission.SetGlobalFloat("scoreBlue", 0);
            Transmission.SetGlobalBool("gamePaused", false);
        }
        Transmission.Instance.OnGlobalBoolChanged.AddListener(CheckPauseGame);
        Transmission.Instance.OnGlobalFloatChanged.AddListener(CheckScoreChange);
        Transmission.Instance.OnPeerLost.AddListener(HandlePeerLost);

        //Make the pause menu open on a home button press
        pointer.GetComponent<ControlInput>().OnHomeButtonTap.AddListener(TogglePauseGame);
    }

    public void ResetBall()
    {
        ball.GetComponent<Ball>().ResetBall(Ball.BallResetType.Neutral);
    }

    //Adjust values on prefab so all instantiated instances match, called after settings are edited
    public void ApplySettings()
    {
        ballPrefab.GetComponent<Collider>().material = gameSettings.BallBounciness;
        ballPrefab.transform.localScale = gameSettings.BallSize;
    }

    /// <summary>
    /// Called when player exits from pause menu; despawns transmission objects and goes back to opening menu
    /// </summary>
    public void ExitGame()
    {
        //Remove Paddle
        paddle.Despawn();

        //remove transmission objects specific to the owner
        if (gameOwner)
        {
            Transmission.SetGlobalBool("gamePaused", true);
            ball.Despawn();
            arena.Despawn();
            gameOwner = false;
        }

        //Remove event subscriptions and disable Transmission
        Transmission.Instance.OnGlobalFloatChanged.RemoveListener(CheckPauseGame);
        Transmission.Instance.OnGlobalFloatChanged.RemoveListener(CheckScoreChange);
        Transmission.Instance.OnPeerLost.RemoveListener(HandlePeerLost);
        Transmission.Instance.gameObject.SetActive(false);

        //Reset menus to welcome screen, unregister hometap event
        headposeMenus.transform.GetChild(1).GetChild(0).gameObject.SetActive(true);
        headposeMenus.transform.GetChild(1).gameObject.SetActive(false);
        headposeMenus.transform.GetChild(0).gameObject.SetActive(true);
        headposeMenus.transform.GetChild(0).GetChild(0).gameObject.SetActive(true);
        pointer.GetComponent<ControlInput>().OnHomeButtonTap.RemoveListener(TogglePauseGame);
    }

    public void ResetGame()
    {
        if (gameOwner)
        {
            ResetBall();
            Transmission.SetGlobalFloat("scoreRed", 0);
            Transmission.SetGlobalFloat("scoreBlue", 0);
            Transmission.SetGlobalBool("gamePaused", false);
        }
    }

#if UNITY_EDITOR
    /// <summary>
    /// Called by keyboard shortcut to fake connecting to a peer
    /// </summary>
    public void FakeConnection()
    {
        OnJoinGame("null");
    }
#endif
    #endregion //Public Functions



    #region Event Handlers
    /// <summary>
    /// Called by Placement when Confirm is called successfully.
    /// </summary>
    /// <param name="pos">final placement location</param>
    /// <param name="rot">final placement rotation</param>
    private void OnPlacementConfirm(Vector3 pos, Quaternion rot)
    {
        headposeMenus.SetActive(true);
        GameObject confirmPage = headposeMenus.transform.Find("PlacementConfirmationPage")?.gameObject;
        if (!confirmPage)
            Debug.LogWarning("Could not find placement confirmation page");
        confirmPage.SetActive(true);
    }

    /// <summary>
    /// Called when a peer is found
    /// </summary>
    /// <param name="peerLabel"></param>
    private void OnJoinGame(string peerLabel)
    {
        //Notify other scripts of connection
        ConnectionEvent.Invoke(true);
        //Reset and disable start menus
        foreach(Transform page in headposeMenus.transform.GetChild(0))
        {
            if(page.name == "WelcomePage")
            {
                page.gameObject.SetActive(true);
            }
            else
            {
                page.gameObject.SetActive(false);
            }
        }
        headposeMenus.transform.GetChild(0).gameObject.SetActive(false);
        //Enable in game menus, show confirmation screen
        headposeMenus.transform.GetChild(1).gameObject.SetActive(true);
    }

    /// <summary>
    /// When a global bool updates, it checks if it is both players signalling readiness to begin
    /// </summary>
    /// <param name="key">What bool was changed</param>
    private void CheckReadiness(string key)
    {
        if (Transmission.HasGlobalBool("HostReady") && Transmission.HasGlobalBool("PeerReady") &&
            Transmission.GetGlobalBool("HostReady") && Transmission.GetGlobalBool("PeerReady"))
            StartGame();
    }

    /// <summary>
    /// Called when a Transmission global float changes, if it was a score, then it checks if it meets the win condition
    /// </summary>
    /// <param name="key"></param>
    private void CheckScoreChange(string key)
    {
        gameOverMenu.SetActive(true);
        if (key == "scoreRed" && Transmission.GetGlobalFloat(key) == gameSettings.MaxScore)
        {
            foreach(Transform t in gameOverMenu.transform)
            {
                if (t.name == "RedText")
                    t.gameObject.SetActive(true);
            }
        }
        else if (key == "scoreBlue" && Transmission.GetGlobalFloat(key) == gameSettings.MaxScore)
        {
            foreach(Transform t in gameOverMenu.transform)
            {
                if (t.name == "BlueText")
                    t.gameObject.SetActive(true);
            }
        }
    }

    /// <summary>
    /// Sets global pause state
    /// </summary>
    public void TogglePauseGame()
    {
        if (Transmission.HasGlobalBool("gamePaused"))
        {
            Transmission.SetGlobalBool("gamePaused", !Transmission.GetGlobalBool("gamePaused"));
        }
    }

    /// <summary>
    /// Called when global bool changes, updates objects if it was the gamePaused state bool
    /// </summary>
    private void CheckPauseGame(string key)
    {
        if(key == "gamePaused")
        {
            //If the game became paused:
            if (Transmission.GetGlobalBool(key))
            {
                //Enable pause menus, freeze ball if the owner
                pauseMenu.SetActive(true);
                if (!gameOwner)
                    pauseMenu.GetComponent<PauseUI>().DisableOwnerButtons();
                else
                {
                    pauseMenu.GetComponent<PauseUI>().EnableOwnerButtons();
                    ball.GetComponent<Ball>().Freeze();
                }
                //disable paddle
                paddle.gameObject.SetActive(false);
            }
            //If game became unpaused
            else if(!Transmission.GetGlobalBool(key))
            {
                //Disable menus
                foreach (Transform t in pauseMenu.transform.parent)
                {
                    t.gameObject.SetActive(false);
                }
                //enable components
                paddle.gameObject.SetActive(true);
                if (gameOwner)
                    ball.GetComponent<Ball>().Unfreeze();
            }
        }
    }

    private void HandlePeerLost(string peerLabel)
    {
        if (Transmission.Peers.Length == 0)
            ExitGame();
    }
    #endregion //Event Handlers
}
