using System.Collections.Generic;
using UnityEngine;
using MagicLeapTools;
using UnityEngine.UI;


/* This is based on the SpactialAlinmentExample.cs 
 * This is a test
 * Just want to see if I can get something working
*/
public class SetUp : MonoBehaviour
{

    

    private void Awake()
    {
        TransmissionObject system = Transmission.Spawn("EarthMoonSun", new Vector3(0,2,-2), Quaternion.Euler(0,0,0), new Vector3(0.25f, 0.25f, 0.25f));

    }


    // Start is called before the first frame update
    void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {
        
    }
}
