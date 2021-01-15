using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using MagicLeapTools;

[RequireComponent(typeof(Collider))]
public class RequestOwnership : MonoBehaviour
{
    private void OnTriggerEnter(Collider other)
    {
        if(other.gameObject.name == "Ball")
        {
            other.GetComponent<TransmissionObject>().IsMine = true;
        }
    }
}
