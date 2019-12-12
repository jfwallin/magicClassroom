using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.XR.MagicLeap;


/// <summary>
/// manages shield spawning
/// </summary>
public class ShieldHand : HandScript
{
    //Having the shield transition states allows for transition animations in the future, they aren't used right now
    public enum ShieldHandState { Idle, SummoningShield, ShieldActive, UnsummoningShield}

    [Header("Shield Variables")]
    public GameObject shieldPrefab;
    public float mPDrainRate;
    [Tooltip("must have MPDrainRate * minMPMultiplier magic points to summon shield")]
    public float minMPMultiplier;

    private ShieldHandState curState;

    #region Unity Methods
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
        curState = ShieldHandState.Idle;
    }

    protected override void OnDestroy()
    {
        //Make sure base onDestroy is called
        base.OnDestroy();

        shieldPrefab.SetActive(false);
    }

    protected override void Update()
    {
        base.Update();

        //Only update if hand is clearly visible
        if (magicLeapHand.IsVisible == true & magicLeapHand.HandConfidence > 0.9)
        {
            //Get new keypose only if it registered with high confidence
            if (magicLeapHand.KeyPoseConfidence > 0.85f)
            {
                pose = magicLeapHand.KeyPose;
            }

            if (curState == ShieldHandState.Idle)
            {
                if(pose == MLHandKeyPose.Fist)
                    FormShield();
            }
            else if (curState == ShieldHandState.ShieldActive)
            {
                if (pose != MLHandKeyPose.Fist || !playerScript.UseMagicPoints(mPDrainRate * Time.deltaTime)) //FIX
                    DispellShield();
            }
        }
        else //hand is not visible
        {
            //turn off shield when we can't follow the hand
            if (curState == ShieldHandState.ShieldActive)
                DispellShield();
        }
    }
    #endregion

    #region Private Methods
    /// <summary>
    /// If player has enough magic points and magic isn't already being used, set magicBeingUsed flag, activate shield, set state to ShieldActive
    /// </summary>
    private void FormShield()
    {
        if (playerScript.magicBeingUsed == false && playerScript.MagicPoints > mPDrainRate * minMPMultiplier)
        {
            shieldPrefab.SetActive(true);
            playerScript.magicBeingUsed = true;
            curState = ShieldHandState.ShieldActive;
            playerScript.ApplyDamage(11); //Temporary, for testing purposes
        }
    }

    /// <summary>
    /// Deactivate shield, unset magicBeingUsed flag, set state to Idle
    /// </summary>
    private void DispellShield()
    {
        shieldPrefab.SetActive(false);
        playerScript.magicBeingUsed = false;
        curState = ShieldHandState.Idle;
    }
    #endregion
}
