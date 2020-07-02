using System.Collections;
using System.Collections.Generic;
using System;
using UnityEngine;
using UnityEngine.Assertions;

public class Ball : MonoBehaviour
{
    //Where should the ball be reset too:
    public enum BallResetType { RedSide, BlueSide, Neutral }

    private SceneControl sc = null;               //Reference to sceneControl to get the arena position
    private Rigidbody rb = null;                  //Ball rigidbody, used to acces velocity for storage
    private Vector3 storedVel = Vector3.zero;     //Stores ball movement when frozen for a game pause
    private Vector3 storedAngVel = Vector3.zero;

    private Vector3 size;                         //values controlled by the settings object
    private PhysicMaterial bounciness = null;

    public event Action onBallReset;
    void Awake()
    {
        //Initialize and check references
        sc = GameObject.Find("SceneControl").GetComponent<SceneControl>();
        Assert.IsNotNull(sc, "Could not find SceneControl script in Ball Script");
        rb = GetComponent<Rigidbody>();
        Assert.IsNotNull(rb, "Could not find Rigidbody on the Ball");
        bounciness = GetComponent<Collider>().material;
        Assert.IsNotNull(bounciness, "Could not find physics material on the ball");
        size = transform.localScale;
    }

    /// <summary>
    /// moves the ball to a starting position
    /// </summary>
    /// <param name="type">which starting position is it moving to</param>
    public void ResetBall(BallResetType type)
    {
        onBallReset.Invoke();
        //Reset ball values
        transform.rotation = Quaternion.identity;
        GetComponent<Rigidbody>().velocity = Vector3.zero;
        GetComponent<Rigidbody>().angularVelocity = Vector3.zero;
        //move ball back to correct side of the arena
        if (type == BallResetType.RedSide)
            transform.position = ((Vector3)sc.ArenaTransform?.position) + new Vector3(0.3f, 1.25f, 0);
        else if (type == BallResetType.BlueSide)
            transform.position = ((Vector3)sc.ArenaTransform?.position) + new Vector3(-0.3f, 1.25f, 0);
        else //reset type is neutral
            transform.position = ((Vector3)sc.ArenaTransform?.position) + new Vector3(0, 1.25f, 0);
    }
}
