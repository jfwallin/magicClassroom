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
            transform.LookAt(cameraObject.transform);
            if(triggered == false)
            {
                triggered = true;
                StartCoroutine(LaunchPrefab());
            }
        }
        else
        {
            triggered = false;
        }
    }

    private IEnumerator LaunchPrefab()
    {
        while(true)
        {

            yield return new WaitForSeconds(5f);
        }
    }

}
