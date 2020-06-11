using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.Assertions;
using MagicLeapTools;

public class ScoreManager : MonoBehaviour
{
    [SerializeField]
    private Text redScore = null;
    [SerializeField]
    private Text blueScore = null;

    private void Awake()
    {
        Assert.IsNotNull(redScore, "redScore is not set");
        Assert.IsNotNull(blueScore, "blueScore is not set");
        Transmission.Instance.OnGlobalFloatChanged.AddListener(HandleFloatChanged);
    }

    private void OnDisable()
    {
        Transmission.Instance.OnGlobalFloatChanged.RemoveListener(HandleFloatChanged);
    }

    private void HandleFloatChanged(string key)
    {
        if (key == "scoreRed")
            redScore.text = Transmission.GetGlobalFloat(key).ToString();
        else if (key == "scoreBlue")
            blueScore.text = Transmission.GetGlobalFloat(key).ToString();
    }
}
