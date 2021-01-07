using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class TargetSpawner : MonoBehaviour
{
    public GameObject targetClone;
    private bool spawning = false;

    // Update is called once per frame
    void Update()
    {
        if (transform.childCount == 0 && spawning == false)
        {
            Invoke("SpawnTarget", 1f);
            spawning = true;
        }
    }

    public void SpawnTarget()
    {
        GameObject go = Instantiate(targetClone, transform);
        go.transform.SetParent(transform);
        go.transform.localPosition = Vector3.zero;
        go.transform.localRotation = Quaternion.identity;
        spawning = false;
    }
}
