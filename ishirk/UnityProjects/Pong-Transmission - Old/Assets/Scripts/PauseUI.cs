using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.Assertions;

public class PauseUI : MonoBehaviour
{
    [Header("Menu References")]
    [SerializeField]
    private Button exitButton = null;
    [SerializeField]
    private Button resetBallButton = null;
    [SerializeField]
    private Button replaceFieldButton = null;
    [SerializeField]
    private Button settingsButton = null;

    private void Awake()
    {
        Assert.IsNotNull(exitButton, "exitButton is not set on PauseUI");
        Assert.IsNotNull(resetBallButton, "resetBallButton is not set on PauseUI");
        Assert.IsNotNull(replaceFieldButton, "replaceFieldButton is not set on PauseUI");
        Assert.IsNotNull(settingsButton, "settingsButton is not set on PauseUI");
    }

    public void DisableOwnerButtons()
    {
        exitButton.interactable = false;
        resetBallButton.interactable = false;
        replaceFieldButton.interactable = false;
        settingsButton.interactable = false;
    }

    public void EnableOwnerButtons()
    {
        exitButton.interactable = true;
        resetBallButton.interactable = true;
        replaceFieldButton.interactable = true;
        settingsButton.interactable = true;
    }
}
