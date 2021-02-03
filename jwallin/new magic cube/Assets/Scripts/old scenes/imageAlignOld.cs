//ImageTrackingSimple.cs
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;
using UnityEngine.XR.MagicLeap;

// example code from https://forum.magicleap.com/hc/en-us/community/posts/360047911512--Simple-Sample-Image-Tracking?page=1#community_comment_360006415052

public class imageAlignOld
    : MonoBehaviour
{
    public MLImageTarget _imageTarget;

    /*
         private MLImageTarget _imageTarget;   
    [SerializeField] private string _name;
    [SerializeField] private Texture2D _image;
    [SerializeField] private float _longerDimension;
    */

    public string _name1, _name2;
    public Texture2D _image1;
    public Texture2D _image2;
    public float _longerDimension;

    public GameObject cube1, cube2;

    public GameObject dataTarget;
    //private MLImageTargetTrackingStatus statlist;

    int s1, s2;
    Vector3 p1, p2;

    void Start()
    {

        MLResult result1 = MLImageTracker.Start();
        _imageTarget = MLImageTracker.AddTarget(_name1, _image1, _longerDimension, ImageTrackingCallback);
        _imageTarget = MLImageTracker.AddTarget(_name2, _image2, _longerDimension, ImageTrackingCallback2);


        s1 = 0;
        s2 = 0;

        p1 = new Vector3(0.0f, 0.0f, 0.0f);
        p2 = new Vector3(0.0f, 0.0f, 0.0f);
    }

    void Update()
    {

        //Debug.Log("positions = " + p1 + p2);
    }
    private void OnDestroy()
    {
        MLImageTracker.RemoveTarget(_name1);
        MLImageTracker.RemoveTarget(_name2);
        MLImageTracker.Stop();
    }

    private void ImageTrackingCallback(MLImageTarget imageTarget, MLImageTargetResult imageTargetResult)
    {
        if (imageTargetResult.Status.ToString() == "Tracked")
        {
            Debug.Log("Target 1 found!");
            /*
            Debug.Log("Target 1 found!");
            Debug.Log("xPosition: " + imageTargetResult.Position);
            Debug.Log("xRotation: " + imageTargetResult.Rotation);
            Debug.Log("xstatus: " + imageTargetResult.Status);
            */

            cube1.transform.position = imageTargetResult.Position + Vector3.up * 0.05f; ;
            cube1.transform.rotation = imageTargetResult.Rotation;
            s1 = 1;
            p1 = imageTargetResult.Position;
            //MLImageTracker.RemoveTarget(_name1);
        }
        if (s1 == 1 && s2 == 1) this.trackingDone();
    }



    private void ImageTrackingCallback2(MLImageTarget imageTarget, MLImageTargetResult imageTargetResult)
    {
        if ( imageTargetResult.Status.ToString() == "Tracked")
            {
            Debug.Log("Target 2 found!");
            /*
             * Debug.Log("Target 2 found!");
              Debug.Log("yPosition: " + imageTargetResult.Position);
              Debug.Log("yRotation: " + imageTargetResult.Rotation);
              Debug.Log("ystatus: " + imageTargetResult.Status.ToString());
              */
            cube2.transform.position = imageTargetResult.Position + Vector3.up * 0.05f;
            cube2.transform.rotation = imageTargetResult.Rotation;

            p2 = imageTargetResult.Position;
            //MLImageTracker.RemoveTarget(_name2);
            s2 = 1;
        }
        if (s1 == 1 && s2 == 1) this.trackingDone();

    }

    private void trackingDone()
    {


        //Debug.Log("about to end tracking ");

        //yield WaitForSeconds(5);

        dataTarget = GameObject.Find("dataObject");
        //dataTarget.GetComponent<datacontainer>().cat = -5;
        dataTarget.GetComponent<datacontainer>().imageLocation1 = p1;
        dataTarget.GetComponent<datacontainer>().imageLocation2 = p2;

        cube1.transform.localScale = new Vector3(0.001f, 0.001f, 0.001f);
        cube2.transform.localScale = new Vector3(0.001f, 0.001f, 0.01f);

        MLImageTracker.RemoveTarget(_name1);
        MLImageTracker.RemoveTarget(_name2);
        MLImageTracker.Stop();

        //StartCoroutine(waiter());
        //MLImageTracker.Stop();
        SceneManager.LoadScene("new scene");
        //Debug.Log("tracking disabled");


    }



}