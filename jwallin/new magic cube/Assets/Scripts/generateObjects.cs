using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class generateObjects : MonoBehaviour
{
    // Start is called before the first frame update
    public GameObject dataTarget;
    public Vector3 mydata = new Vector3();
    void Start()
    {
        dataTarget = GameObject.Find("dataObject");
        mydata = dataTarget.GetComponent<datacontainer>().imageLocation1 ;
        Debug.Log("hello from lock and load");
        Debug.Log("image 1 location in new script " );
        Debug.Log( mydata);
        mydata = dataTarget.GetComponent<datacontainer>().imageLocation2 ;
        Debug.Log("hello from lock and load");
        Debug.Log("image 2 location in new script "+mydata );
        Debug.Log( mydata);
        //Debug.Log(dataContainer.ImageLocation2);
    }

    // Update is called once per frame
    void Update()
    {
        
    }
}
