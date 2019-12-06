using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class AttackHand : HandScript
{
    public GameObject fireballPrefab;
    public ParticleSystem chargingAttackParticles;
    public LineRenderer aimDirectionIndicator;

    private float directionIndicatorLength = 2.0f;

    #region Unity Methods
    protected override void Start()
    {
        //Make sure base start is called
        base.Start();

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
        if(aimDirectionIndicator == null)
        {
            aimDirectionIndicator = GetComponent<LineRenderer>();
            if(aimDirectionIndicator == null)
            {
                Debug.LogWarning("No LineRenderer assigned to attack hand, disabling script");
                enabled = false;
            }
        }

        chargingAttackParticles.Stop();
        aimDirectionIndicator.enabled = false;
    }

    protected override void OnDestroy()
    {
        //Make sure base ondestroy is called
        base.OnDestroy();

        chargingAttackParticles.Stop();
        aimDirectionIndicator.enabled = false;
    }

    protected override void Update()
    {
        //make sure base update is called
        base.Update();
        if(aimDirectionIndicator.enabled)
            UpdateLineRenderer();
    }
    #endregion

    private void UpdateLineRenderer()
    {
        Vector3[] setPoints = new Vector3[2];
        setPoints[0] = transform.position + firingDirection * 0.1f;
        setPoints[1] = transform.position + firingDirection * directionIndicatorLength;
        aimDirectionIndicator.positionCount = 2;
        aimDirectionIndicator.SetPositions(setPoints);
    }

    #region Public Methods
    public void ChargeFireball()
    {
        chargingAttackParticles.Play();
        aimDirectionIndicator.enabled = true;
    }

    public void StopChargingFireball()
    {
        ParticleSystemStopBehavior behavior = ParticleSystemStopBehavior.StopEmittingAndClear;
        chargingAttackParticles.Stop(true, behavior);
        aimDirectionIndicator.enabled = false;
    }

    public void FireFireball()
    {
        GameObject go = GameObject.Instantiate(fireballPrefab, transform.position, Quaternion.identity);
        Rigidbody rb = go.GetComponent<Rigidbody>();
        rb.velocity = firingDirection * 2;
        Destroy(go, 5f);
        ParticleSystemStopBehavior behavior = ParticleSystemStopBehavior.StopEmittingAndClear;
        chargingAttackParticles.Stop(true, behavior);
    }
    #endregion
}
