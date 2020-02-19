using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.Assertions;
using MagicLeapTools;

/// <summary>
/// Manages the display of multiplayer connection screen
/// </summary>
public class ConnectingGUIManager : MonoBehaviour
{
    //UI Objects to manipulate while connecting
    [Header("UI Objects")]
    [SerializeField]
    private Text waitingText = null;
    [SerializeField]
    private Text connectedText = null;

    private void Awake()
    {
        //Check references
        Assert.IsNotNull(waitingText, "waitingText not assigned");
        Assert.IsNotNull(connectedText, "connectedText not assigned");
    }

    private void OnEnable()
    {
        //Set default state, register event handler
        waitingText.gameObject.SetActive(true);
        connectedText.gameObject.SetActive(false);
        Transmission.Instance.OnPeerFound.AddListener(handleOnPeerFound);

        //check if the connection already occurred
        if (Transmission.Peers.Length > 0)
            handleOnPeerFound(Transmission.Peers[0]);
    }

    private void OnDisable()
    {
        //unregister event handler
        Transmission.Instance.OnPeerFound.RemoveListener(handleOnPeerFound);
    }

    /// <summary>
    /// Responds to transmissionconnecting to another app
    /// </summary>
    /// <param name="peerLabel">name of peer</param>
    public void handleOnPeerFound(string peerLabel)
    {
        StartCoroutine(connectedTransition());
    }

    /// <summary>
    /// Briefly displays connection confirmation
    /// </summary>
    private IEnumerator connectedTransition()
    {
        //display connectedText
        waitingText.gameObject.SetActive(false);
        connectedText.gameObject.SetActive(true);
        yield return new WaitForSeconds(1.5f);

        //Turn off. ADD CALLS TO SCENE MANAGER TO CHANGE STATE
        transform.parent.GetChild(0).gameObject.SetActive(false);
        gameObject.SetActive(false);
        yield return null;
    }
}
