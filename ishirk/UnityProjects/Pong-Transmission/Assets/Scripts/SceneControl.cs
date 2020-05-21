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

    //prefabs to instantiate or spawn
    [Header("Prefabs")]
    [SerializeField]
    private GameObject placementArenaPrefab = null;
    private const string arenaPrefabPath = "Prefabs/Arena";
    private const string ballPrefabPath = "Prefabs/Ball";
    private const string paddlePrefabPath = "Prefabs/Paddle";

    //references to scene objects
    [Header("Scene References")]
    [SerializeField]
    private Transmission transmissionComponent = null;
    [SerializeField]
    private Pointer pointer = null;
    [SerializeField]
    private GameObject headposeMenus = null;
    [SerializeField]
    private GameObject pauseMenu = null;

    //references to scene objects after instantiation
    private TransmissionObject paddle = null;
    private TransmissionObject ball = null;
    private TransmissionObject arena = null;
    private GameObject placementArena = null;

    //state variables
    private bool gamePaused = true;
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
        ;
    }
    #endregion //Unity Methods

    #region Public Functions
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
        //Start the placement session
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
    /// Called by the person setting up and simulating the game. Spawns the Arena and Ball
    /// </summary>
    public void CreateGame()
    {
        //Update state
        gameOwner = true;
        //Enable Transmission
        transmissionComponent.gameObject.SetActive(true);
        //Spawn Transmission Arena
        Transform arenaTransform = placementArena.transform;
        arena = Transmission.Spawn(arenaPrefabPath, arenaTransform.position, arenaTransform.rotation, arenaTransform.localScale);
        arena.transform.SetParent(GameObject.Find("[_DYNAMIC]").transform);
        //Spawn Transmission ball
        ball = Transmission.Spawn(ballPrefabPath, arenaTransform.position + new Vector3(0, 1.25f, 0), Quaternion.identity, Vector3.one);
        ball.transform.SetParent(GameObject.Find("[_DYNAMIC]").transform);
        //Prepare to handle connection event
        if(Transmission.Peers.Length > 0)
            OnJoinGame(Transmission.Peers[0]);
        else //no connection has occurred yet
            transmissionComponent.OnPeerFound.AddListener(OnJoinGame);

    }

    /// <summary>
    /// Undos create Game. Removes Transmission objects and disables transmission
    /// </summary>
    public void CancelCreateGame()
    {
        //Update state
        gameOwner = false;
        //Unsubscribe from connection event
        transmissionComponent.OnPeerFound.RemoveListener(OnJoinGame);
        //Despawn transmission objects
        ball.Despawn();
        arena.Despawn();
        //Disable transmission
        transmissionComponent.gameObject.SetActive(false);
    }

    /// <summary>
    /// Called when the player selects "Join Game"
    /// </summary>
    public void TryJoinGame()
    {
        transmissionComponent.gameObject.SetActive(true);
        if (Transmission.Peers.Length > 0)
            OnJoinGame(Transmission.Peers[0]);
        else //no connection event has occurred yet
            Transmission.Instance.OnPeerFound.AddListener(OnJoinGame);
    }

    /// <summary>
    /// Called when player cancels joining a game
    /// </summary>
    public void CancelJoinGame()
    {
        Transmission.Instance.OnPeerFound.RemoveListener(OnJoinGame);
        transmissionComponent.gameObject.SetActive(false);
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
        //Make it move with the pointer
        ParentConstraint paddlePC = paddle.GetComponent<ParentConstraint>();
        ConstraintSource pointerSource = new ConstraintSource();
        pointerSource.sourceTransform = pointer.transform;
        pointerSource.weight = 1f;
        paddlePC.AddSource(pointerSource);
        for (int i = 0; i < paddlePC.translationOffsets.Length; i++)
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
        gamePaused = true;

        //Close the ConfirmReadinessPage
        headposeMenus.transform.GetChild(1).GetChild(0).gameObject.SetActive(false);

        //manage global variables and event subscriptions with Transmission
        Transmission.Instance.OnGlobalBoolChanged.RemoveListener(CheckReadiness);
        if(gameOwner)
        {
            Transmission.SetGlobalFloat("scoreRed", 0);
            Transmission.SetGlobalFloat("scoreBlue", 0);
        }
        Transmission.Instance.OnGlobalFloatChanged.AddListener(CheckScoreChange);
        Transmission.Instance.OnPeerLost.AddListener(HandlePeerLost);

        //Make the pause menu open on a home button press
        pointer.GetComponent<ControlInput>().OnHomeButtonTap.AddListener(TogglePauseMenu);
    }

    /// <summary>
    /// Called when player exits from pause menu; despawns transmission objects and goes back to opening menu
    /// </summary>
    public void ExitGame()
    {
        gamePaused = true;

        //Remove Paddle
        paddle.Despawn();

        //remove transmission objects specific to the owner
        if (gameOwner)
        {
            ball.Despawn();
            arena.Despawn();
            gameOwner = false;
        }

        //Remove event subscriptions and disable Transmission
        Transmission.Instance.OnGlobalFloatChanged.RemoveListener(CheckScoreChange);
        Transmission.Instance.OnPeerLost.RemoveListener(HandlePeerLost);
        transmissionComponent.gameObject.SetActive(false);

        //Reset menus to welcome screen, unregister hometap event
        headposeMenus.transform.GetChild(1).gameObject.SetActive(false);
        headposeMenus.transform.GetChild(0).gameObject.SetActive(true);
        headposeMenus.transform.GetChild(0).GetChild(0).gameObject.SetActive(true);
        pointer.GetComponent<ControlInput>().OnHomeButtonTap.RemoveListener(TogglePauseMenu);
    }

    //Move this to the ball script
    public void ApplySettings()
    {
        ball.transform.localScale = gameSettings.BallSize;
        ball.GetComponent<SphereCollider>().material = gameSettings.BallBounciness;
    }
    #endregion //Public Functions



    #region Event Handlers
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
    public void OnJoinGame(string peerLabel)
    {
        //Notify other scripts of connection
        ConnectionEvent.Invoke(true);
        //Disable start menus
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
        if (Transmission.GetGlobalBool("HostReady") && Transmission.GetGlobalBool("PeerReady"))
            StartGame();
    }

    /// <summary>
    /// Called when a Transmission global float changes, if it was a score, then it checks if it meets the win condition
    /// </summary>
    /// <param name="key"></param>
    private void CheckScoreChange(string key)
    {
        if (key == "scoreRed" && Transmission.GetGlobalFloat(key) == gameSettings.MaxScore)
        {
            //Red Wins
        }
        else if (key == "scoreBlue" && Transmission.GetGlobalFloat(key) == gameSettings.MaxScore)
        {
            //blue wins
        }
    }

    /// <summary>
    /// Called on home press, shows pause menu
    /// </summary>
    public void TogglePauseMenu()
    {
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
            if (!gameOwner)
                pauseMenu.GetComponent<PauseUI>().DisableOwnerButtons();
            else
                pauseMenu.GetComponent<PauseUI>().EnableOwnerButtons();
            gamePaused = true;
        }
    }

    public void HandlePeerLost(string peerLabel)
    {
        if (Transmission.Peers.Length == 0)
            ExitGame();
    }
    #endregion //Event Handlers
}
