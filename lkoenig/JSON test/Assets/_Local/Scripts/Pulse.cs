﻿using System.CodeDom;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Pulse : InheritTest
{
    public float min, max;
    private bool grow = true;

    // Start is called before the first frame update
    void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {
        Vector3 scaleChange = new Vector3(0.01f, 0.01f, 0.01f);

        if (grow == true)
        {
            transform.localScale += scaleChange;
            if (transform.localScale.x >= max)
            {
                grow = false;
            }

        }
        else
        {
            transform.localScale -= scaleChange;
            if (transform.localScale.x <= min) grow = true;
        }
    }

    public override void test()
    {
        base.test();
        UnityEngine.Debug.Log("Override Test.");
    }
}