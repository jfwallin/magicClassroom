using MagicLeapTools;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Assertions;

public class AlignmentDebug : MonoBehaviour
{
    public SpatialAlignment sa;

    void Start()
    {
        if (!sa)
            sa = GetComponent<SpatialAlignment>();
        Assert.IsNotNull(sa, "SpatialAlignment was not assigned and could not be found in debug script");
        sa.OnLocalized.AddListener(handleOnLocalized);
        if(sa.enabled)
        {
            Debug.Log("Found Spatial Alignment component, and it is enabled.");
        }
    }

    private void OnDisable()
    {
        sa.OnLocalized.RemoveListener(handleOnLocalized);
    }

    private void handleOnLocalized()
    {
        Debug.Log("--------------------------\nSpatial Alignement has localized the app\n----------------------------");
    }
}
