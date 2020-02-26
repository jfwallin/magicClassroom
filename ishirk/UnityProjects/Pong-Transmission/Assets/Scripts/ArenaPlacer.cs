using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.XR.MagicLeap;
using UnityEngine.Assertions;

public class ArenaPlacer : MonoBehaviour
{
    [SerializeField]
    private Placement placeScript = null;
    [SerializeField]
    private Material highlightMaterial = null;

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
        if(placeScript.IsRunning)
        {
            transform.position = placeScript.Position;
            transform.rotation = placeScript.Rotation;
        }
    }

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
