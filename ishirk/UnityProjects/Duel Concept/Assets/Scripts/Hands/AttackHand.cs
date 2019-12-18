using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.XR.MagicLeap;

/// <summary>
/// manages magic attack spawning, launching, and states
/// </summary>
public class AttackHand : HandScript
{
    public enum AttackHandState { Idle, ChargingAttack, LaunchingAttack}

    #region Public Variables
    [Header("Attack Variables")]
    public float mPCost;                            //How many Magic Points are used to cast the attack
    public GameObject fireballPrefab;               //Prefab to be instantiated and launched
    public ParticleSystem chargingAttackParticles;  //Particle system activated when preparing to fire
    public LineRenderer aimDirectionLine;           //Responsible for displaying aim direction
    public GameObject aimLaserDot;
    public AudioSource chargingSoundSource;         //Toggled when in charging state
    #endregion

    private AttackHandState curState;                //Magic state of the hand
    private float directionIndicatorLength = 2.0f;   //The length the aim indicator

    #region Unity Methods
    protected override void Start()
    {
        //Make sure base start is called
        base.Start();

        //Make sure public references have been assigned
        if (fireballPrefab == null)
        {
            Debug.LogWarning("No fireball prefab assinged to attack hand, disabling script");
            enabled = false;
        }
        if(chargingAttackParticles == null)
        {
            Debug.LogWarning("No Particle SystemAssigned to attack hand, disabling script");
            enabled = false;
        }
        if(aimDirectionLine == null)
        {
            aimDirectionLine = GetComponent<LineRenderer>();
            if(aimDirectionLine == null)
            {
                Debug.LogWarning("No LineRenderer assigned to attack hand, disabling script");
                enabled = false;
            }
        }

        //Initializations
        curState = AttackHandState.Idle;
        chargingAttackParticles.Stop();
        aimDirectionLine.positionCount = 2;
        aimDirectionLine.enabled = false;
        aimLaserDot.SetActive(false);
    }

    protected override void OnDestroy()
    {
        //Make sure base ondestroy is called
        base.OnDestroy();

        //Clear visuals
        chargingAttackParticles.Stop();
        aimLaserDot.SetActive(false);
        aimDirectionLine.enabled = false;
    }

    protected override void Update()
    {
        //make sure base update is called
        base.Update();

        //Make sure nothing is updated if the hand tracking doesn't have a good view of the hand
        if (magicLeapHand.IsVisible == true && magicLeapHand.HandConfidence > 0.9)
        {
            //Get new current keypose only if it is registered with high confidence
            if (magicLeapHand.KeyPoseConfidence > 0.85f)
            {
                pose = magicLeapHand.KeyPose;
            }

            //State switcher
            if (curState == AttackHandState.Idle)
            {
                if (pose == MLHandKeyPose.OpenHand)
                    ChargeFireball();
            }
            else if (curState == AttackHandState.ChargingAttack)
            {
                if (pose == MLHandKeyPose.Finger)
                    FireFireball();
                else if (pose != MLHandKeyPose.OpenHand)
                    StopChargingFireball();
            }

            aimDirectionLine.enabled = true;
            aimLaserDot.SetActive(true);
            UpdateAimLine();
        }
        else //Hand is not visible
        {
            aimDirectionLine.enabled = false;
            aimLaserDot.SetActive(false);
            if(curState == AttackHandState.ChargingAttack)
            {
                StopChargingFireball();
            }
        }
    }
    #endregion

    #region Private Methods
    /// <summary>
    /// Points aim indicator in current hand forwardDirection
    /// </summary>
    private void UpdateAimLine()
    {
        Vector3[] setPoints = new Vector3[2];
        setPoints[0] = transform.position + forwardDirection * 0.1f;
        setPoints[1] = transform.position + forwardDirection * directionIndicatorLength;
        aimDirectionLine.SetPositions(setPoints);
        RaycastHit hitInfo;
        if(Physics.Raycast(transform.position, forwardDirection, out hitInfo))
        {
            aimLaserDot.transform.position = hitInfo.point + hitInfo.normal * 0.02f;
            aimLaserDot.transform.rotation = Quaternion.LookRotation(hitInfo.normal);
        }

        
    }

    /// <summary>
    /// If the player has enough MP and magic isn't already being used(shield): start charging particle system, play charging sound, set magicBeingUsed flag, and set state to ChargingAttack
    /// </summary>
    private void ChargeFireball()
    {
        //playerScript.UseMagicPoints() automatically uses the MP if it can and returns true, if not it just returns false
        if (playerScript.magicBeingUsed == false && playerScript.UseMagicPoints(mPCost))
        {
            chargingAttackParticles.Play();
            chargingSoundSource.Play();
            playerScript.magicBeingUsed = true;
            curState = AttackHandState.ChargingAttack;
        }
    }

    /// <summary>
    /// stop charging particle system, unset magicBeingUsed flag, restore MP, set state to Idle
    /// </summary>
    private void StopChargingFireball()
    {
        ParticleSystemStopBehavior behavior = ParticleSystemStopBehavior.StopEmittingAndClear;
        chargingAttackParticles.Stop(true, behavior);
        chargingSoundSource.Stop();
        playerScript.magicBeingUsed = false;
        playerScript.RestoreMagicPoints(mPCost);
        curState = AttackHandState.Idle;
    }

    /// <summary>
    /// Instantiate projectile and give it a velocity, disable charging particle system, unset magicBeingUSed flag, set state to Idle
    /// </summary>
    private void FireFireball()
    {
        //The LaunchingAttack state is more useful for attacks that take multiple frames to launch. Here it takes only one, 
        //but both states are included for clarity. Eventually attack calls will be split into a beginning and end where
        //state changes will occur seperated by an animation
        curState = AttackHandState.LaunchingAttack;
        GameObject go = GameObject.Instantiate(fireballPrefab, transform.position, Quaternion.identity);
        Rigidbody rb = go.GetComponent<Rigidbody>();
        //set the projectile in motion
        rb.velocity = forwardDirection * go.GetComponent<FireBallScript>().velocity;
        //Destroy it after 5 seconds
        Destroy(go, 5f);
        ParticleSystemStopBehavior behavior = ParticleSystemStopBehavior.StopEmittingAndClear;
        chargingAttackParticles.Stop(true, behavior);
        chargingSoundSource.Stop();
        playerScript.magicBeingUsed = false;
        curState = AttackHandState.Idle;
    }
    #endregion
}
