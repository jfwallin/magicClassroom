using System;
using System.Collections;
using System.Collections.Generic;
using System.Net;
using System.Net.Sockets;
using System.Text;
using UnityEngine;
using UnityEngine.Networking;



public class generateObjects : MonoBehaviour
{
    // Start is called before the first frame update
    public GameObject dataTarget;


    public Vector3 worldZero;
    public Vector3 worldVector;
    public float worldScale;
    public float worldTheta;
    public Vector3 imageLocation1, imageLocation2;

public static class JsonHelper
{
    public static T[] FromJson<T>(string json)
    {
        Debug.Log("fromJson - wrapper-----");
        Debug.Log(json);
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
        private class sceneElement
        {
        public Vector3 position ;
        public Quaternion rotation ;
        public Vector3 scale ;

        public bool rigidbody ;
        public string name ;
        public string textureName;
        public string objectShape ;

        public float rotationRate;
        public float rotationPhase;
        public string objectOrbiting;
        public float orbitalRadius;
        public float orbitalPeriod;
        public float orbitalPhase;

        /* 
        public string objectOrbiting { get; set; }
        public float orbitalRadius { get; set; }
        public float orbitalPeriod { get; set; }
        public float orbitalPhase { get; set; }
        public float orbitalInclination { get; set; }
        public float orbitalArgumentOfPeriapse { get; set; }
        public float orbitalEllipticity { get; set; }
        public float rotationRate { get; set; }
        public float rotationPhase { get; set; }
        */

    }
 
    [SerializeField]
    sceneElement [] sceneElements = new sceneElement[10];

    private string jsonStuff;

    GameObject[] sceneObjects = new GameObject[20];

    Dictionary<string, PrimitiveType> shapeDictionary =   
        new Dictionary<string, PrimitiveType >(){ 
            {"Cube", PrimitiveType.Cube}, 
            {"Sphere", PrimitiveType.Sphere},
            {"Capsule", PrimitiveType.Capsule},
            {"Cylinder", PrimitiveType.Cylinder},
            {"Quad", PrimitiveType.Quad},
            {"Plane", PrimitiveType.Plane}
        }; 
       

       public int sceneID;

       public string zzz ;

       private int myupdate;

    void Start()
    {

        
        dataTarget = GameObject.Find("dataObject");
        worldZero = dataTarget.GetComponent<datacontainer>().worldZero ;
        worldVector = dataTarget.GetComponent<datacontainer>().worldVector ;
        worldTheta = dataTarget.GetComponent<datacontainer>().worldTheta ;
        worldScale = dataTarget.GetComponent<datacontainer>().worldScale ;
        
        imageLocation1 = dataTarget.GetComponent<datacontainer>().imageLocation1;
        imageLocation2 = dataTarget.GetComponent<datacontainer>().imageLocation2;

        sceneID = 0;
        

        //StartCoroutine(PollForUpdates());
        Debug.Log("prototyping");

        //prototypeObjects() ;
        //string z1 = objectsToJson();

        string zz1 = "{\"Items\": [{ \"rigidbody\": false, \"objectOrbiting\": \"None\", \"orbitalPhase\": 0.0, \"name\": \"Jupiter\", \"textureName\": \"Jupiter\", \"rotationRate\": 2.071129707112971, \"orbitalPeriod\": 1.0, \"rotationPhase\": 0.0, \"scale\": { \"y\": 0.7846409532768894, \"x\": 0.7846409532768894, \"z\": 0.7846409532768894 }, \"position\": { \"y\": 0.3, \"x\": 0.0, \"z\": 0.0 }, \"rotation\": { \"y\": 0.0, \"x\": 0.0, \"z\": 0.0, \"w\": 0.0 }, \"objectShape\": \"Cube\", \"orbitalRadius\": 1.0 }, { \"rigidbody\": false, \"objectOrbiting\": \"None\", \"orbitalPhase\": 0.0, \"name\": \"Io\", \"textureName\": \"Io\", \"rotationRate\": 0.37029288702928875, \"orbitalPeriod\": 1.0, \"rotationPhase\": 0.0, \"scale\": { \"y\": 0.1599397930385701, \"x\": 0.1599397930385701, \"z\": 0.1599397930385701 }, \"position\": { \"y\": 0.3, \"x\": 0.6616494198808404, \"z\": 0.0 }, \"rotation\": { \"y\": 0.0, \"x\": 0.0, \"z\": 0.0, \"w\": 0.0 }, \"objectShape\": \"Cube\", \"orbitalRadius\": 1.0 }, { \"rigidbody\": false, \"objectOrbiting\": \"None\", \"orbitalPhase\": 0.0, \"name\": \"Europa\", \"textureName\": \"Europa\", \"rotationRate\": 0.7426778242677825, \"orbitalPeriod\": 1.0, \"rotationPhase\": 0.0, \"scale\": { \"y\": 0.13704107870805896, \"x\": 0.13704107870805896, \"z\": 0.13704107870805896 }, \"position\": { \"y\": 0.3, \"x\": 1.0520539354029477, \"z\": 0.0 }, \"rotation\": { \"y\": 0.0, \"x\": 0.0, \"z\": 0.0, \"w\": 0.0 }, \"objectShape\": \"Cube\", \"orbitalRadius\": 1.0 }, { \"rigidbody\": false, \"objectOrbiting\": \"None\", \"orbitalPhase\": 0.0, \"name\": \"Callisto\", \"textureName\": \"Callisto\", \"rotationRate\": 3.4916317991631804, \"orbitalPeriod\": 1.0, \"rotationPhase\": 0.0, \"scale\": { \"y\": 0.2116287237378489, \"x\": 0.2116287237378489, \"z\": 0.2116287237378489 }, \"position\": { \"y\": 0.3, \"x\": 2.9523361555346503, \"z\": 0.0 }, \"rotation\": { \"y\": 0.0, \"x\": 0.0, \"z\": 0.0, \"w\": 0.0 }, \"objectShape\": \"Cube\", \"orbitalRadius\": 1.0 }, { \"rigidbody\": false, \"objectOrbiting\": \"None\", \"orbitalPhase\": 0.0, \"name\": \"Ganymede\", \"textureName\": \"Ganymede\", \"rotationRate\": 1.4958158995815902, \"orbitalPeriod\": 1.0, \"rotationPhase\": 0.0, \"scale\": { \"y\": 0.23102414550015682, \"x\": 0.23102414550015682, \"z\": 0.23102414550015682 }, \"position\": { \"y\": 0.3, \"x\": 1.6776418940106617, \"z\": 0.0 }, \"rotation\": { \"y\": 0.0, \"x\": 0.0, \"z\": 0.0, \"w\": 0.0 }, \"objectShape\": \"Cube\", \"orbitalRadius\": 1.0 }]}";
        //string z1="{\"Items\": [{ \"rigidbody\": false, \"objectOrbiting\": \"None\", \"orbitalPhase\": 0.0, \"name\": \"Jupiter\", \"textureName\": \"Jupiter\", \"rotationRate\": 2.071129707112971, \"orbitalPeriod\": 1.0, \"rotationPhase\": 0.0, \"scale\": { \"y\": 0.008250862339291313, \"x\": 0.008250862339291313, \"z\": 0.008250862339291313 }, \"position\": { \"y\": 0.3, \"x\": 0.4194104735026654, \"z\": 0.0 }, \"rotation\": { \"y\": 0.0, \"x\": 0.0, \"z\": 0.0, \"w\": 0.0 }, \"objectShape\": \"Cube\", \"orbitalRadius\": 1.0 }, { \"rigidbody\": false, \"objectOrbiting\": \"None\", \"orbitalPhase\": 0.0, \"name\": \"Io\", \"textureName\": \"Io\", \"rotationRate\": 0.37029288702928875, \"orbitalPeriod\": 1.0, \"rotationPhase\": 0.0, \"scale\": { \"y\": 0.008250862339291313, \"x\": 0.008250862339291313, \"z\": 0.008250862339291313 }, \"position\": { \"y\": 0.3, \"x\": 0.4194104735026654, \"z\": 0.0 }, \"rotation\": { \"y\": 0.0, \"x\": 0.0, \"z\": 0.0, \"w\": 0.0 }, \"objectShape\": \"Cube\", \"orbitalRadius\": 1.0 }, { \"rigidbody\": false, \"objectOrbiting\": \"None\", \"orbitalPhase\": 0.0, \"name\": \"Europa\", \"textureName\": \"Europa\", \"rotationRate\": 0.7426778242677825, \"orbitalPeriod\": 1.0, \"rotationPhase\": 0.0, \"scale\": { \"y\": 0.008250862339291313, \"x\": 0.008250862339291313, \"z\": 0.008250862339291313 }, \"position\": { \"y\": 0.3, \"x\": 0.4194104735026654, \"z\": 0.0 }, \"rotation\": { \"y\": 0.0, \"x\": 0.0, \"z\": 0.0, \"w\": 0.0 }, \"objectShape\": \"Cube\", \"orbitalRadius\": 1.0 }, { \"rigidbody\": false, \"objectOrbiting\": \"None\", \"orbitalPhase\": 0.0, \"name\": \"Callisto\", \"textureName\": \"Callisto\", \"rotationRate\": 3.4916317991631804, \"orbitalPeriod\": 1.0, \"rotationPhase\": 0.0, \"scale\": { \"y\": 0.008250862339291313, \"x\": 0.008250862339291313, \"z\": 0.008250862339291313 }, \"position\": { \"y\": 0.3, \"x\": 0.4194104735026654, \"z\": 0.0 }, \"rotation\": { \"y\": 0.0, \"x\": 0.0, \"z\": 0.0, \"w\": 0.0 }, \"objectShape\": \"Cube\", \"orbitalRadius\": 1.0 }, { \"rigidbody\": false, \"objectOrbiting\": \"None\", \"orbitalPhase\": 0.0, \"name\": \"Ganymede\", \"textureName\": \"Ganymede\", \"rotationRate\": 1.4958158995815902, \"orbitalPeriod\": 1.0, \"rotationPhase\": 0.0, \"scale\": { \"y\": 0.008250862339291313, \"x\": 0.008250862339291313, \"z\": 0.008250862339291313 }, \"position\": { \"y\": 0.3, \"x\": 0.4194104735026654, \"z\": 0.0 }, \"rotation\": { \"y\": 0.0, \"x\": 0.0, \"z\": 0.0, \"w\": 0.0 }, \"objectShape\": \"Cube\", \"orbitalRadius\": 1.0 }]}";
        jsonToObjects(zz1);
        createObjects();
        myupdate = 0;

        StartCoroutine(grabScene());
        
    }

    void prototypeObjects() {
        int nobj = 5;
        float theta = 45.0f;
        float x, y, z;
        float objectSize;
        for (int i = 0; i< nobj; i++){
            Debug.Log("ddd" + i);
            sceneElements[i] = new sceneElement();
            sceneElements[i].name = "ddd"  + i.ToString ();
            x = UnityEngine.Random.Range(-2.0f,2.0f);
            y = UnityEngine.Random.Range(1.25f,2.75f) ;
            z = UnityEngine.Random.Range(1.5f,3.0f) ;
            objectSize = UnityEngine.Random.Range(0.1f,0.7f) ;
            sceneElements[i].position = new Vector3(x, y, z);
            sceneElements[i].rotation = new Quaternion(0.0f, theta, 0.0f, 0.0f); //  The 4th element is dodgy
            sceneElements[i].scale = new Vector3(objectSize, objectSize, objectSize);
            sceneElements[i].rigidbody = true;
            sceneElements[i].textureName = "Jupiter";
            sceneElements[i].objectShape = "Cube";

            sceneElements[i].rotationRate = 0.0f;
            sceneElements[i].rotationPhase = 1.0f;
            sceneElements[i].objectOrbiting = "None";
            sceneElements[i].orbitalRadius = 1.0f;
            sceneElements[i].orbitalPeriod = 1.0f;
            sceneElements[i].orbitalPhase = 0.0f;
        }
    }


    string objectsToJson() {
        string ss = JsonHelper.ToJson(sceneElements, true);
        Debug.Log(ss);
        return ss;
    }

    void jsonToObjects(string jsonString) {
        //Player[] player = JsonHelper.FromJson<Player>(jsonString); 
        sceneElements = JsonHelper.FromJson<sceneElement>(jsonString);
    }

    void createObjects() {
        int nobj = 5;
        PrimitiveType oshape ;
        float tstart;

        tstart = Time.time;

        for (int i = 0; i< nobj; i++){

            Debug.Log("creating object"+ i);
            
            
            bool foundKey = shapeDictionary.TryGetValue (sceneElements[i].objectShape, out oshape);

            if (foundKey ) {
            sceneObjects[i]= GameObject.CreatePrimitive(oshape);
            } else {
                sceneObjects[i]= GameObject.CreatePrimitive(PrimitiveType.Sphere); 
            }
            

            //sceneObjects[i]= GameObject.CreatePrimitive(PrimitiveType.Sphere); 

            /*
            if (sceneElements[i].rigidbody ) {
                sceneObjects[i].AddComponent<Rigidbody>();
                sceneObjects[i].GetComponent<Rigidbody>().useGravity = false;
            }
            */


            sceneObjects[i].transform.position = sceneElements[i].position  + worldZero;
            sceneObjects[i].transform.rotation = sceneElements[i].rotation ;
            sceneObjects[i].transform.localScale = sceneElements[i].scale ;
            Texture2D myPlanetTexture = Resources.Load(sceneElements[i].textureName) as Texture2D;
            sceneObjects[i].GetComponent<Renderer>().material.mainTexture = myPlanetTexture;

            sceneObjects[i].name = sceneElements[i].name ;

        }
    }
    
 
    void destroyObjects() {

        int nobj = 5;    
        for (int i = 0; i< nobj; i++){
            Destroy(sceneObjects[i]);
        }
    }





IEnumerator grabScene()
    {

        while(myupdate < 100) {

        
            // updates the scene every 10 seconds
            string sceneJson;
            string waddress = "http://52.202.177.208/magic/jj.php";

            yield return new WaitForSeconds(5); 
            UnityWebRequest www = UnityWebRequest.Get(waddress);
           
            yield return www.SendWebRequest();

            if (www.isNetworkError || www.isHttpError)
            {
                Debug.Log(www.error);
            }
            else
            {
                // Show results as text
                sceneJson = www.downloadHandler.text;
                myupdate = myupdate + 1;
                Debug.Log("updating "+myupdate);
                destroyObjects ();
                jsonToObjects(sceneJson);
                createObjects();
            }
            
        }
    }

 /*   
IEnumerator grabFirstScene()
    {
        string tout;
        string waddress = "http://52.202.177.208/magic/sceneID.php";

        yield return new WaitForSeconds(2);
        UnityWebRequest www = UnityWebRequest.Get(waddress);
           
        yield return www.SendWebRequest();

        if (www.isNetworkError || www.isHttpError)
        {
            Debug.Log(www.error);
        }
        else
        {
            // Show results as text
            tout = www.downloadHandler.text;
            sceneID = int.Parse(tout);
            
        }
    }



/* 
IEnumerator PollForUpdates()
{
    string tout;
    string waddress = "http://52.202.177.208/magic/sceneID.php";
    int newSceneID;

    while (true)
    {        
        yield return new WaitForSeconds(2);
        UnityWebRequest www = UnityWebRequest.Get(waddress);
        yield return www.SendWebRequest();

        if (www.isNetworkError || www.isHttpError)
        {
            Debug.Log(www.error);
        }
        else
        {
            // Show results as text
            tout = www.downloadHandler.text;
            newSceneID = int.Parse(tout);

            if (newSceneID != sceneID) {
                sceneID = newSceneID;
                destroyObjects();
                //grabScene();
                StartCoroutine(grabScene());
            }   
        }
    }
}


/* 

IEnumerator grabScene()
{
    string tout;
    string waddress = "http://52.202.177.208/magic/grabscene.php";

    UnityWebRequest www = UnityWebRequest.Get(waddress);           
    yield return www.SendWebRequest();
    if (www.isNetworkError || www.isHttpError)
    {
        Debug.Log(www.error);
    }
    else
    {
    // Show results as text
        tout = www.downloadHandler.text;
        jsonToObjects(tout);
        createObjects();
    }
    
}
      
 */

    // Update is called once per frame
    void Update()
    {
        
    }
}
