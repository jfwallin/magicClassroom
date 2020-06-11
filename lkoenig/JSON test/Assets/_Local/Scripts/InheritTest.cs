using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class InheritTest : MonoBehaviour
{
    // Start is called before the first frame update
    void Start()
    {
        UnityEngine.Debug.Log("In Start of InheritTest.");
    }

    public virtual void test()
    {
        UnityEngine.Debug.Log("Testing 1...2...3");
    }
}
