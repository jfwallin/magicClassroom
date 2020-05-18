using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Assertions;

public class Ball : MonoBehaviour
{
    //Where should the ball be reset too:
    public enum BallResetType { RedSide, BlueSide, Neutral }

    //Reference to sceneControl to get the arena position
    private SceneControl sc = null;
    
    void Awake()
    {
        //Get sceneControl reference
        sc = GameObject.Find("SceneControl").GetComponent<SceneControl>();
        Assert.IsNotNull(sc, "Could not find SceneControl script in Ball Script");
    }

    public void ResetBall(BallResetType type)
    {
        //Reset ball values
        transform.rotation = Quaternion.identity;
        GetComponent<Rigidbody>().velocity = Vector3.zero;
        //move ball back to correct side of the arena
        if (type == BallResetType.RedSide)
            transform.position = ((Vector3)sc.ArenaTransform?.position) + sc.ArenaTransform.TransformPoint(new Vector3(0.3f, 1.25f, 0));
        else if (type == BallResetType.BlueSide)
            transform.position = ((Vector3)sc.ArenaTransform?.position) + sc.ArenaTransform.TransformPoint(new Vector3(-0.3f, 1.25f, 0));
        else //reset type is neutral
            transform.position = ((Vector3)sc.ArenaTransform?.position) + new Vector3(0, 1.25f, 0);
    }
}
