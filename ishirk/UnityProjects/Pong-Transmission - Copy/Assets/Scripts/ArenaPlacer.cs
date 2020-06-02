using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.XR.MagicLeap;
using UnityEngine.Assertions;


/// <summary>
/// Updates arena transform and highlight color to match position and fit in the placement session
/// </summary>
public class ArenaPlacer : MonoBehaviour
{
    [SerializeField]
    private Placement placeScript = null;      //Reference to the placement script on placement arena
    [SerializeField]
    private Material highlightMaterial = null; //Material for the arena area highlight showing correct fits (green = good fit, etc.)
    private bool stoppedRunning = false;       //Flag for the first frame after Placement stops running

    private void Awake()
    {
        Assert.IsNotNull(placeScript, "Placement script is not assigned");
        Assert.IsNotNull(highlightMaterial, "highlightMaterial is not assigned");
    }

    void Start()
    {
        placeScript.OnPlacementEvent += handleOnFitChange;
    }

    private void OnDestroy()
    {
        placeScript.OnPlacementEvent -= handleOnFitChange;
    }

    void Update()
    {
        if (placeScript.IsRunning)
        {
            transform.position = placeScript.Position;
            transform.rotation = placeScript.Rotation;
        }
        else
        {
            stoppedRunning = true;
        }
            
        if(stoppedRunning)
        {
            highlightMaterial.color = Color.clear;
            stoppedRunning = false;
        }
    }

    /// <summary>
    /// Updates color of arena highlight when placement fit changes
    /// </summary>
    /// <param name="fit">Current fit type</param>
    private void handleOnFitChange(FitType fit)
    {
        Color tmpColor;
        switch(fit)
        {
            case FitType.Fits:
                tmpColor = new Color(0, 1, 0, 0.2f); //green
                break;
            case FitType.Overhang:
            case FitType.Uneven:
                tmpColor = new Color(1, 1, 0, 0.2f); //yellow
                break;
            default:
                tmpColor = new Color(1, 0, 0, 0.2f);
                break;
        }
        highlightMaterial.color = tmpColor;
    }
}
