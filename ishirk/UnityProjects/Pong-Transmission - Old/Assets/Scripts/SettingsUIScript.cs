using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.Assertions;

public class SettingsUIScript : MonoBehaviour
{
    #region Private Variables
    [Header("Settings Reference")]
    [SerializeField]
    private Settings settingsObj = null;

    [Header("Settings Text Fields")]
    [SerializeField]
    private Text scoreText = null;
    [SerializeField]
    private Text bouncinessText = null;
    [SerializeField]
    private Text sizeText = null;
    #endregion

    #region Unity Methods
    private void OnEnable()
    {
        //Reference checking
        Assert.IsNotNull(settingsObj, "settingsObj was not assigned");
        Assert.IsNotNull(scoreText, "scoreText was not assigned");
        Assert.IsNotNull(bouncinessText, "bouncinessText was not assigned");
        Assert.IsNotNull(sizeText, "sizeText was not assigned");

        settingsObj.OnSettingsChanged += handleSettingsChange;
    }

    private void Start()
    {
        //Initialization
        scoreText.text = "10";
        bouncinessText.text = "Normal";
        sizeText.text = "Normal";
    }

    private void OnDisable()
    {
        settingsObj.OnSettingsChanged -= handleSettingsChange;
    }
    #endregion

    /// <summary>
    /// Reacts to settings having changed, updating menu text
    /// </summary>
    /// <param name="setting">name of setting changed</param>
    /// <param name="value">new value of changed setting</param>
    private void handleSettingsChange(string setting, string value)
    {
        switch(setting)
        {
            case "BallSize":
                sizeText.text = value;
                break;
            case "BallBounciness":
                bouncinessText.text = value;
                break;
            case "MaxScore":
                scoreText.text = value;
                break;
            default:
                break;
        }
    }
}
