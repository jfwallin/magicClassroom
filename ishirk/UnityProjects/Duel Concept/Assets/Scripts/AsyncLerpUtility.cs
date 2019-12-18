using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class AsyncLerpUtility : MonoBehaviour
{
    public void StartLerp(ref Vector3 valueToLerp, Vector3 startValue, Vector3 endValue, float timeToComplete)
    {
        float startTime = Time.time;
        StartCoroutine(Lerp(valueToLerp, startValue, endValue, startTime, timeToComplete));
    }

    private IEnumerator Lerp(Vector3 valueToLerp, Vector3 startValue, Vector3 endValue, float startTime, float timeToComplete)
    {
        float lerpTime = Time.time - startTime;
        while(lerpTime < timeToComplete)
        {
            lerpTime = Time.time - startTime;
            float percentComplete = lerpTime / timeToComplete;
            valueToLerp = Vector3.Lerp(startValue, endValue, percentComplete);
            yield return null;
        }
    }
}
