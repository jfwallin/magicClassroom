using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class LauncherScript : MonoBehaviour
{
    public Vector3 LaunchLocation;
    public GameObject projectilePrefab;
    public float triggerDistance;

    private GameObject cameraObject;
    private bool triggered;

    void Start()
    {
        cameraObject = Camera.main.gameObject;
    }

    void Update()
    {
        if ((cameraObject.transform.position - transform.position).sqrMagnitude < (triggerDistance*triggerDistance))
        {
            Vector3 lookDirection = (cameraObject.transform.position - new Vector3(0, 0.5f, 0)) - transform.position;
            transform.rotation = Quaternion.LookRotation(lookDirection);
            if(triggered == false)
            {
                triggered = true;
                StartCoroutine(LaunchPrefab());
            }
        }
        else if(triggered == true)
        {
            triggered = false;
            StopAllCoroutines();
        }
    }

    private IEnumerator LaunchPrefab()
    {
        while(true)
        {
            GameObject go = Instantiate(projectilePrefab, transform.TransformPoint(LaunchLocation), Quaternion.identity);
            Rigidbody rb = go.GetComponent<Rigidbody>();
            rb.velocity = transform.forward * go.GetComponent<FireBallScript>().velocity;
            Destroy(go, 5f);
            yield return new WaitForSeconds(5f);
        }
    }

}
