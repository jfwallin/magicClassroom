//ImageTrackingSimple.cs
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.XR.MagicLeap;

// example code from https://forum.magicleap.com/hc/en-us/community/posts/360047911512--Simple-Sample-Image-Tracking?page=1#community_comment_360006415052

public class imageTracker : MonoBehaviour
{
    public MLImageTarget _imageTarget;

    /*
         private MLImageTarget _imageTarget;   
    [SerializeField] private string _name;
    [SerializeField] private Texture2D _image;
    [SerializeField] private float _longerDimension;
    */

    public string _name;
    public Texture2D _image;
    public float _longerDimension;

    public GameObject cube;

    void Start()
    {
        MLResult result = MLImageTracker.Start();
        _imageTarget = MLImageTracker.AddTarget(_name, _image, _longerDimension, ImageTrackingCallback);
    }

    private void OnDestroy()
    {
        MLImageTracker.RemoveTarget(_name);
        MLImageTracker.Stop();
    }

    private void ImageTrackingCallback(MLImageTarget imageTarget, MLImageTargetResult imageTargetResult)
    {
        Debug.Log("Position: " + imageTargetResult.Position);
        Debug.Log("Rotation: " + imageTargetResult.Rotation);


        cube.transform.position = imageTargetResult.Position + Vector3.up*0.3f;
        cube.transform.rotation = imageTargetResult.Rotation;
    }
}