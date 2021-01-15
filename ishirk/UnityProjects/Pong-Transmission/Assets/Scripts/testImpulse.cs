using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UIElements;

public class testImpulse : MonoBehaviour
{
    public float strength = 2f;
    private float velocity = 0;
    private Vector3 posLastFrame = Vector3.zero;

    private void Start()
    {
        posLastFrame = transform.position;
    }

    private void Update()
    {
        velocity = (transform.position - posLastFrame).magnitude;
        posLastFrame = transform.position;
    }

    private void OnCollisionEnter(Collision collision)
    {
        ContactPoint contact = collision.GetContact(0);
        Vector3 force = contact.normal * velocity * strength;
        Debug.Log("velocity: " + velocity.ToString() + "and the force: " + force.ToString());
        collision.collider.attachedRigidbody.AddForce(force, ForceMode.Impulse);
    }
}
