using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Assertions;

public class StartConfirmationUI : MonoBehaviour
{
    [SerializeField]
    private GameObject confirmButton = null;
    [SerializeField]
    private GameObject waitingText = null;

    private void Awake()
    {
        Assert.IsNotNull(confirmButton, "confirmButton not assigned on StartConfirmationPage");
        Assert.IsNotNull(waitingText, "waitingText not assigned on StartConfirmationPage");
    }

    void OnEnable()
    {
        waitingText.SetActive(false);
        confirmButton.SetActive(true);
    }
}