using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class ShieldHand : HandScript
{
    public GameObject shieldPrefab;

    protected override void Start()
    {
        //Make sure base start is called
        base.Start();

        if(shieldPrefab == null)
        {
            Debug.LogWarning("No Shield prefab assigned to shieldHand, disabling script.");
            enabled = false;
        }

        shieldPrefab.SetActive(false);
    }

    protected override void OnDestroy()
    {
        //Make sure base onDestroy is called
        base.OnDestroy();

        shieldPrefab.SetActive(false);
    }

    public void FormShield()
    {
        shieldPrefab.SetActive(true);
    }

    public void DispellShield()
    {
        shieldPrefab.SetActive(false);
    }
}
