//ImageTrackingSimple.cs
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;
using UnityEngine.XR.MagicLeap;


// ebased on  code from https://forum.magicleap.com/hc/en-us/community/posts/360047911512--Simple-Sample-Image-Tracking?page=1#community_comment_360006415052

public class lockImages : MonoBehaviour
{



    private MLImageTarget[] _imageTarget = new MLImageTarget[2];
    public Texture2D[] _imageList = new Texture2D[2];
    private string[] _imageName = new string[2];
    private Vector3[] imagePosition = new Vector3[2];
    private int[] imageStatus = new int[2];
    private int imagesFound;

    public float _longerDimension;
   // private GameObject [] cubes = new GameObject[2];
    public GameObject dataTarget;
    private int nTargets;
    public GameObject[] cubes = new GameObject[2];
    private GameObject[] trackedCubes = new GameObject[2];
    private Vector3 myposition, myrotation;

    private Quaternion imageRotation = new Quaternion();

    public AudioClip targetFoundSound;

      private AudioSource source;
      //private float lowPitchRange = .75F;
      //private float highPitchRange = 1.5F;





    void Start()
    {

        MLResult result1 = MLImageTracker.Start();

        nTargets = _imageList.Length;
        imagesFound = 0;
        dataTarget = GameObject.Find("dataObject");

        for (int i = 0; i < nTargets; i++)
        {
            _imageName[i] = "image." + i.ToString();
            imagePosition[i] = new Vector3(0.0f, 0.0f, 0.0f);
            imageStatus[i] = 0;
            _imageTarget[i] = MLImageTracker.AddTarget(_imageName[i], _imageList[i], _longerDimension, ImageTracked);

        }
    }





    void Update()
    {
       /*  myposition = transform.position;
        dataTarget.GetComponent<datacontainer>().mylocation = myposition; 
        myrotation  = transform.rotation.eulerAngles;
        dataTarget.GetComponent<datacontainer>().myrotation= myrotation; 
        */        

    }


    private void OnDestroy()
    {
      //  MLImageTracker.RemoveTarget(_name1);
      //  MLImageTracker.RemoveTarget(_name2);
       // MLImageTracker.Stop();
    }


    private void ImageTracked(MLImageTarget imageTarget, MLImageTargetResult imageTargetResult)
    {
        if (imageTargetResult.Status.ToString() == "Tracked")
        {
  
            for (int j = 0; j < nTargets; j++)
            {
                if ( (imageTarget == _imageTarget[j]) && (imageStatus[j] == 0) )
                {
                    //Debug.Log("Found image " + j.ToString());
                    int itarget = j;

                    imageStatus[itarget] = 1;
                    imagesFound = imagesFound + 1;

                    imageRotation = imageTargetResult.Rotation;
                    imageRotation[0] = imageRotation[0] + 90.0f;
                    trackedCubes[j] = (GameObject)Instantiate(cubes[j], imageTargetResult.Position, imageRotation );
                    imagePosition[itarget] = imageTargetResult.Position;


                    if (imagesFound == 1)  {
                        source = GetComponent<AudioSource>();
                        source.pitch = 1.5f;
                    }  else   {
                        source.pitch = 0.5f;
                    }
                    float hitVol = 0.5f;

                    source.PlayOneShot(targetFoundSound, hitVol);

                }
            }
        }

        if (imagesFound == nTargets) this.trackingDone();
    }



    private void trackingDone()
    {
        
        dataTarget.GetComponent<datacontainer>().imageLocation1 = imagePosition[0];
        dataTarget.GetComponent<datacontainer>().imageLocation2 = imagePosition[1]; 
        dataTarget.GetComponent<datacontainer>().calculateWorld();

        myposition = transform.position;
        myrotation  = transform.rotation.eulerAngles;
        dataTarget.GetComponent<datacontainer>().myrotation= myrotation; 
        dataTarget.GetComponent<datacontainer>().mylocation = myposition; 
        imagesFound = 0;

        /* 
        for (int j = 0; j < nTargets; j++) {
            Destroy(trackedCubes[j]);
        }
        */

         //MLImageTracker.Stop();
       SceneManager.LoadScene("LoadData2");

  

    }



}