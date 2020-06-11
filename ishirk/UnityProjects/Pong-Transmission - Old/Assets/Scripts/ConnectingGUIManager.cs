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
    [SerializeField]
    private SceneControl sCon = null;
    
    private void Awake()
    {
        //Check references
        Assert.IsNotNull(sCon, "Scene Control is not assigned on ConnectionGUIManaget");
    }

    private void OnEnable()
    {
        sCon.ConnectionEvent += onConnectionEvent;
    }

    private void OnDisable()
    {
        sCon.ConnectionEvent -= onConnectionEvent;
    }

    public void onConnectionEvent(bool connected)
    {
        if(connected)
        {
            gameObject.SetActive(false);
        }
    }
}
