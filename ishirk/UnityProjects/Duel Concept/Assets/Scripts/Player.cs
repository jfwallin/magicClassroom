using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Player : MonoBehaviour
{
    #region Events
    public delegate void DeathAction();
    public event DeathAction OnPlayerDeath;   //Called when player HP reaches 0
    #endregion

    #region Public Variables
    public float maxHP = 100f;                //Set in the inspector, sets upper bound
    public float maxMP = 100f;
    public float mPRegenRate = 5f;            //Rate at which player gains MP when idle
    [HideInInspector]
    public bool magicBeingUsed;               //Flag set when spells are being cast, so MP doesnt Regen
    #endregion

    #region Private Variables
    [SerializeField]
    private Vector3 rightShoulderPosition;    //position that determines hand forward direction
    [SerializeField]
    private Vector3 leftShoulderPosition;
    private float healthPoints;               //health and magic levels
    private float magicPoints;
    #endregion

    #region Properties
    //When another script gets a shoulder position, it gets transformed into world space first
    public Vector3 RightShoulderPosition
    {
        get
        {
            return transform.TransformPoint(rightShoulderPosition);
        }
        set => rightShoulderPosition = value;
    }
    public Vector3 LeftShoulderPosition
    {
        get
        {
            return transform.TransformPoint(leftShoulderPosition);
        }
        set => leftShoulderPosition = value;
    }
    //Custom setters are defined later
    public float HealthPoints { get => healthPoints; }
    public float MagicPoints { get => magicPoints; }
    #endregion

    void Start()
    {
        healthPoints = maxHP;
        magicPoints = maxMP;
        magicBeingUsed = false;
    }

    private void Update()
    {
        //Only regen MP if not using it for something
        if(magicBeingUsed == false)
        {
            RestoreMagicPoints(mPRegenRate * Time.deltaTime);
        }
    }

    /// <summary>
    /// Deals damage to the player, checks if it kills
    /// </summary>
    /// <param name="amount">how much damage to deal</param>
    public void ApplyDamage(float amount)
    {
        if (healthPoints > amount)
            healthPoints -= amount;
        else
        {
            healthPoints = 0;
            OnPlayerDeath();
        }
    }

    /// <summary>
    /// subtracts from  player magicPoints
    /// </summary>
    /// <param name="amount">how many MPs to use</param>
    /// <returns>true if succesful, false if not enough MP available</returns>
    public bool UseMagicPoints(float amount)
    {
        if (magicPoints > amount)
        {
            magicPoints -= amount;
            return true;
        }
        else
            return false;
    }

    /// <summary>
    /// Adds MP to player magicPoints
    /// </summary>
    /// <param name="amount">how many MPs to add</param>
    public void RestoreMagicPoints(float amount)
    {
        magicPoints = Mathf.Clamp(amount + magicPoints, 0f, maxMP);
    }
}
