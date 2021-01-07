using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class TargetScript : MonoBehaviour
{
    private void OnCollisionEnter(Collision collision)
    {
        GameObject.Destroy(gameObject, 3f);
        gameObject.GetComponent<Rigidbody>().useGravity = true;
    }
}
