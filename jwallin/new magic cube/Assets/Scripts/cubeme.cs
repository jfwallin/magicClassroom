using System.Collections;
using System.Collections.Generic;
using UnityEngine;





public class cubeme : MonoBehaviour
{
    public float xrate = 5.0f, yrate, zrate;


    // Start is called before the first frame update
    void Start()
    {
        xrate = 5.0f;
        yrate = 10.0f;
        xrate = 20.0f;

    }

    // Update is called once per frame
    void Update()
    {


    }


void LateUpdate()
{
    //transform.Rotate(Vector3.left * xrate * Time.deltaTime);
    //transform.Rotate(Vector3.up * yrate * Time.deltaTime);
    //transform.Rotate(Vector3.forward * zrate * Time.deltaTime);
    }

}
