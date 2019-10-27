using System.Collections;
using System.Collections.Generic;
using UnityEngine;

using System;
using System.Text;



public class cubeme : MonoBehaviour
{
    public float xrate = 5.0f, yrate, zrate;


/* 
public static class JsonHelper
{
    public static T[] FromJson<T>(string json)
    {
        Wrapper<T> wrapper = JsonUtility.FromJson<Wrapper<T>>(json);
        return wrapper.Items;
    }

    public static string ToJson<T>(T[] array)
    {
        Wrapper<T> wrapper = new Wrapper<T>();
        wrapper.Items = array;
        return JsonUtility.ToJson(wrapper);
    }

    public static string ToJson<T>(T[] array, bool prettyPrint)
    {
        Wrapper<T> wrapper = new Wrapper<T>();
        wrapper.Items = array;
        return JsonUtility.ToJson(wrapper, prettyPrint);
    }

    [Serializable]
    private class Wrapper<T>
    {
        public T[] Items;
    }
}



      [Serializable]
        private class mlobject2
        {
        public Vector3 position;
        public string name;

        }
 
        [SerializeField]
        mlobject2[] mylist1 = new mlobject2[10];
         */


    // Start is called before the first frame update
    void Start()
    {


        /* 
        int nobj = 5;
        //[Serializable]
        //mylist = new List<mlobject>(nobj);
        
        Debug.Log("in cube me");
        mlobject2 bb = new mlobject2();
        bb.name = "ddddd";
        bb.position = new Vector3(2.0f, 0.3f, -1.2f);
    
        string sss = JsonUtility.ToJson(bb, true);
        Debug.Log("duck =====  "+sss);
        

        for (int i = 0; i< nobj; i++){
            Debug.Log("ddd" + i);
            mylist1[i] = new mlobject2 ();
            mylist1[i].name = "ddd"  + i.ToString ();
            mylist1[i].position = new Vector3(2.0f, 0.3f, -1.2f);
        }
        //mlobject mylist = new mlobject();
        //mylist.rotationRate = 1.0f;
        //mylist.rotationPhase = 0.3f;
        //mylist.objectOrbiting = "none";
        //mylist.objectShape = "cube";
    

        //string ss = JsonUtility.ToJson(mylist1, true);
        string ss = JsonHelper.ToJson(mylist1, true);
        Debug.Log(ss);

        mlobject2[] mylist2 = JsonHelper.FromJson<mlobject2 >(ss);
        for(int ii =0; ii<3; ii++ ){
        Debug.Log(ii + " " + mylist2[ii].name);
        }

        */


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
