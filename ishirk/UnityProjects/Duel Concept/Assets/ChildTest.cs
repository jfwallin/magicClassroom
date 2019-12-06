using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class ChildTest : InheritanceTesting
{
    void Start()
    {
        Debug.Log("Starting Child");
    }

    protected override void Update()
    {
        base.Update();
        Debug.Log("Update - Child");
    }
}
