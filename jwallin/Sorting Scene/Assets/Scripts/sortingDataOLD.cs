using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System;





public class sortingData2 : MonoBehaviour
{

    public class sortInfo
    {
        public GameObject theObject;
        public int originalOrder;
        public int sortedOrder;
        public int oldOrder;
        public int newOrder;
        public bool isSorted;

        public float fractionalDistance;

        // implement IComparable interface
        public int CompareTo(object obj)
        {
            if (obj is sortInfo)
            {
                return this.fractionalDistance.CompareTo((obj as sortInfo).fractionalDistance);  // compare user names
            }
            else
            {
                return 0;
            }
            //throw new ArgumentException("Object is not a sortInfo");
        }
    }



    public static int nObjects;

    // declare the sort info array
    public sortInfo[] sortData;

    public string objectTag = "sortable";
    public float xstart, ystart, zstart;
    public float xend, yend, zend;
    public GameObject[] gameObjects;
    public Vector3[] sortPts;
    //public sortHelper[] sortDataHelper;

    bool isSorted;

    // default values for the delay, move time, and flourish of the movements
    public float tdelay = 2.0f;
    public float tmove = 5.0f;
    public int pretty = 1;
    private bool beenMoved = false;

    private class orderElement
    {
        int orderValue;
    }

    public GameObject myPrefab;
    public Texture[] myTexture = new Texture[5];
    public String[] tnames = new String[5];
    public GameObject[] markers = new GameObject[5];
    public float mscale = 0.1f;
    public GameObject markerPrefab;

    // Start is called before the first frame update
    void Start()
    {

        // create data

        // find all the game objects to be sorted
        //gameObjects = GameObject.FindGameObjectsWithTag("sortable");
        //nObjects = gameObjects.Length;

        nObjects = 5;

        // this define the points where objects should be located
        setSortedLocations(nObjects, distance: 2.2f, angle: 0.0f, height: 0.0f,
            width: 1.5f, xoffset: 0.0f, zoffset: 0.0f);

        for (int i = 0; i < nObjects; i++)
        {
            mscale = 0.07f;
            markers[i] = Instantiate(markerPrefab, sortPts[i] - new Vector3(0.0f, 0.2f, 0.0f), Quaternion.identity) as GameObject;
            //gameObjects[i].transform.eulerAngles = new Vector3(90.0f, 0.0f, 0.0f);
            markers[i].transform.localScale = new Vector3(mscale, mscale, mscale);
            markers[i].GetComponent<Renderer>().material.SetColor("_Color", Color.blue);
            Debug.Log(i);
            Debug.Log(i.ToString() + "  " + tnames[i]);
        }

        // set the array to be unsorted
        isSorted = false;

        // creates the sortable panels
        createSortables();


        // create an array to help with the sorting
        sortData = new sortInfo[nObjects];
        //sortDataHelper = new sortHelper[nObjects];


        // populate the sorting array with needed information
        for (int i = 0; i < nObjects; i++)
        {
            sortData[i] = new sortInfo();
            sortData[i].theObject = gameObjects[i];
            sortData[i].originalOrder = i;
            sortData[i].sortedOrder = i;
            sortData[i].oldOrder = i;
            sortData[i].newOrder = i;
            sortData[i].fractionalDistance = 0.0f;

            sortData[i].isSorted = false;

            // add the moveObject script to the sortable objects
            sortData[i].theObject.AddComponent<moveObjects>();

        }



        // this sets the intial position to the current position of the objects
        // and sets the time limits so nothing actually moves
        initializePath();



        // this moves the objects to a scrambled locationo
        //List<int> orderList = scrambleOrder(nObjects);

        scrambleLocations();
        //setNewOrder(orderList);
        resetPositions(); //, orderList);


    }

    public void resort()
    {
        Debug.Log("RELEASED!!!!!!!!!!!!!");
        setProjectedLocation();


        //List <int> orderList =   
        //orderByProjectedLocation();
        //setNewOrder(nObjects, orderList);

        pretty = 0;
        tdelay = 0.10f;
        tmove = 1.0f;
        resetPositions(); //, orderList);       
    }

    

    void createSortables()
    {
        float oscale = 0.02f;
        gameObjects = new GameObject[nObjects];
        Debug.Log("starting=======================================================================");
        for (int i = 0; i < nObjects; i++)
        {
            gameObjects[i] = Instantiate(myPrefab, sortPts[i], Quaternion.identity) as GameObject;
            gameObjects[i].transform.eulerAngles = new Vector3(90.0f, 0.0f, 0.0f);
            gameObjects[i].transform.localScale = new Vector3(2.0f * oscale, 1.0f * oscale, 1.0f * oscale);
            gameObjects[i].tag = "sortable";
            //            Texture2D theTexture = Resources.Load(tnames[i]) as Texture2D;
            //texname =  tnames[i] ;
            //Texture2D theTexture = Resources.Load(texname) as Texture2D;
            //Debug.Log("xxx " + texname);
            //gameObjects[i].GetComponent<Renderer>().material.mainTexture = theTexture; // theTexture;

            gameObjects[i].GetComponent<Renderer>().material.mainTexture = myTexture[i]; // theTexture;

            gameObjects[i].name = tnames[i];
            gameObjects[i].GetComponent<Rigidbody>().useGravity = false;
            Debug.Log(i);
            Debug.Log(i.ToString() + "  " + tnames[i]);

        }



    }


    void setSortedLocations(int nObjects, float distance = 3.0f, float angle = 0.0f, float height = 0.0f, float width = 5.0f, float xoffset = 0.0f, float zoffset = 0.0f)
    {

        float xcenter, ycenter, zcenter;
        float angleStart;
        float angleEnd;
        float angleOffset;
        float angleCenter;

        // convert the direction of the sorting line to be in radians
        angleCenter = angle * Mathf.PI / 180.0f;

        // find the center of the sorting line
        // height in the Unity environment is y, not z
        xcenter = distance * Mathf.Sin(angleCenter) + xoffset;
        zcenter = distance * Mathf.Cos(angleCenter) + zoffset;
        ycenter = height;

        // find the angular directions for the left and right side of the sorting line
        angleOffset = Mathf.Atan(width / 2.0f / distance);
        angleStart = angleCenter + angleOffset;
        angleEnd = angleCenter - angleOffset;

        // find the starting and ending positions of the sorting line
        xstart = distance * Mathf.Sin(angleStart) / Mathf.Cos(angleOffset) + xoffset;
        zstart = distance * Mathf.Cos(angleStart) / Mathf.Cos(angleOffset) + zoffset;
        ystart = height;

        xend = distance * Mathf.Sin(angleEnd) / Mathf.Cos(angleOffset) + xoffset;
        zend = distance * Mathf.Cos(angleEnd) / Mathf.Cos(angleOffset) + zoffset;
        yend = height;

        // initialize the array of sorting locaitons
        sortPts = new Vector3[nObjects];

        // determine the correct positions for the elements 
        float dx = (xend - xstart) / (float)(nObjects - 1);
        float dz = (zend - zstart) / (float)(nObjects - 1);
        for (int i = 0; i < nObjects; i++)
        {
            sortPts[i] = new Vector3(xstart + dx * i, height, zstart + dz * i);
        }
    }



    void checkOrder(int nObjects)
    {
        for (int i = 0; i < nObjects; i++)
        {
            if (i == sortData[i].oldOrder)
            {
                sortData[i].isSorted = true;
            }
            else
            {
                sortData[i].isSorted = false;
                isSorted = false;
            }
        }
    }

    void scrambleLocations()
    {

        for (int i = 0; i < nObjects; i++)
        {
            sortData[i].fractionalDistance = UnityEngine.Random.value;
        }
     }
    /*
    List<int> scrambleOrder(int nObjects) {
        // create a List or ordered integers
        List <int> orderList = new List<int>();
        for (int i = 0; i< nObjects; i++) {
            orderList.Add(i);
        }

        // shuffle the List into a random order
        int n = nObjects; 
        System.Random rng = new System.Random();
        while (n > 1) {  
            n--;  
            int k = rng.Next(n + 1);  
            int value = orderList[k];  
            orderList[k] = orderList[n];  
            orderList[n] = value;  
        }  

        return orderList;
    }
    */
    
    void setProjectedLocation( ) {
        // find the project position of an object along the direction of the sorted locations line

        // projected distance along a line r for a vector p is given by
        // distance = r dot p / r
        // the fractional distance is r dot p / r**2

        for (int i = 0; i < nObjects; i++ ) {
            Vector3 targetPosition = sortData[i].theObject.transform.position;
            Vector3 projectedPath = new Vector3( xend-xstart, yend-ystart, zend-zstart);
            float fractionalDistance = Vector3.Dot( targetPosition, projectedPath)  / 
            Vector3.Dot( projectedPath, projectedPath);
            sortData[i].fractionalDistance = fractionalDistance;

            Debug.Log("projected location - "  + i + "  "+ fractionalDistance);

        }
        Debug.Log("---------------------------------------------------------------------------------");
    }

    /*
    void orderByProjectedLocation() {

        for (int i = 0; i < nObjects; i++ ) {
            //Debug.Log("projected location - "  + i + "  "+ sortData[i].fractionalDistance);
            sortDataHelper[i].fractionalDistance = sortData[i].fractionalDistance;
            sortDataHelper[i].objectID = i;

        }

        
        // sort by the new order
        Array.Sort(sortDataHelper, delegate(sortHelper s1, sortHelper s2) {
        return s1.fractionalDistance.CompareTo(s2.fractionalDistance);
        });

        for (int i = 0; i < nObjects; i++)
        {
            Debug.Log("================   " + i.ToString() + "       " + sortDataHelper[i].objectID.ToString() + "       " + sortDataHelper[i].fractionalDistance);
        }


        //-----------------
       for (int i = 0; i< nObjects; i++) {
            sortData[i].newOrder = sortDataHelper[i].objectID;
        } 
        

    }
    */
    /*

    void setNewOrder( List<int> orderList) {
        // set up the moveObject scripts to move the objects
        for (int i = 0; i< nObjects; i++) {
            sortData[i].newOrder = orderList[i];
        }

    }
    */

    void resetPositions() { 
        float myTime;
        myTime = Time.time;

        Debug.Log("******************************************************************************");
        Debug.Log("******************************************************************************");
        Debug.Log("******************************************************************************");

        for (int i = 0; i < nObjects; i++)
        {
            Debug.Log("OLD ORDER -------- " + i.ToString() + "      dista " + sortData[i].fractionalDistance.ToString());
        }
        Debug.Log("******************************************************************************");
        // sort by the new order
        Array.Sort(sortData, delegate(sortInfo s1, sortInfo s2) {
        return s1.fractionalDistance.CompareTo(s2.fractionalDistance);
        });

        // initialize the moveObject variables to safe default values
        initializePath();

        // set up the moveObject scripts to move the objects
        for (int i = 0; i< nObjects; i++) {

            // set the final positions of the particles to be the target locations
            sortData[i].theObject.GetComponent<moveObjects>().FinalPos = sortPts[i];
            Debug.Log("NEW ORDER -------- " + i.ToString() + "      dista " + sortData[i].fractionalDistance.ToString());
            //gameObject[i].theObject.GetComponent<moveObjects>().FinalPos = sortPts[i];


            // move them in an indirect path or a direct path
            if (pretty == 1) {
            
                float rrange = 0.85f; 
                sortData[i].theObject.GetComponent<moveObjects>().MidPos = (sortData[i].theObject.transform.position + sortPts[i]) * 0.5f + 
            new Vector3(UnityEngine.Random.Range(-rrange, rrange), UnityEngine.Random.Range(0, rrange), UnityEngine.Random.Range(-rrange, rrange));

                // this adds a nice spin during the sort
                float a1 = 450;
                float a2 = 360;
                float a3 = 360;

                a1 = 90;
                a2 = 0;
                a3 = 0;
                sortData[i].theObject.GetComponent<moveObjects>().FinalAngle = new Vector3(a1, a2, a3);  

            } else {

                sortData[i].theObject.GetComponent<moveObjects>().MidPos = (sortData[i].theObject.transform.position +
                    sortPts[i]) * 0.5f;

                float a1 = 90;
                float a2 = 0;
                float a3 = 0;
                sortData[i].theObject.GetComponent<moveObjects>().FinalAngle = new Vector3(a1, a2, a3);  
            }

            // start the move after a delay and finishing in a specified time
            sortData[i].theObject.GetComponent<moveObjects>().TimeRange = new Vector2(myTime+tdelay, myTime+tdelay+tmove);

            // update the order of the current object
            sortData[i].oldOrder = i;

            // this is the last thing to do to make it all work
            sortData[i].theObject.GetComponent<moveObjects>().initializePath();

        }

    }



    void initializePath( )
    {

        for (int i=0; i<nObjects;i++) {
            sortData[i].theObject.GetComponent<moveObjects>().StartPos =  sortData[i].theObject.transform.position; 
            sortData[i].theObject.GetComponent<moveObjects>().MidPos = new Vector3(0.001f, 0.001f, 0.001f);
            sortData[i].theObject.GetComponent<moveObjects>().FinalPos = new Vector3(0.001f, 0.001f, 0.001f);

            sortData[i].theObject.GetComponent<moveObjects>().StartSize = sortData[i].theObject.transform.localScale;
            sortData[i].theObject.GetComponent<moveObjects>().FinalSize =sortData[i].theObject.transform.localScale; 
            
            sortData[i].theObject.GetComponent<moveObjects>().StartAngle = sortData[i].theObject.transform.eulerAngles;
            sortData[i].theObject.GetComponent<moveObjects>().FinalAngle = sortData[i].theObject.transform.eulerAngles;

            // this disables the move
            sortData[i].theObject.GetComponent<moveObjects>().TimeRange = new Vector2(-100.0f, -90.0f);

        }
    }

    void testMove() {

    
        
        setProjectedLocation();
        //sortData[3].fractionalDistance = -10f;
        sortData[0].fractionalDistance = 100f;

        //List <int> orderList =   
        //orderByProjectedLocation();
        //setNewOrder(nObjects, orderList);

        pretty = 0;
        tdelay = 3.0f;
        tmove = 3.0f;
        resetPositions() ; //, orderList);        
        

    }


    // Update is called once per frame
    void Update()
    {
        float myTime;
    
        myTime = Time.time;

        /*
        if (myTime  > 8.0 & !beenMoved) {
            beenMoved = true;
            testMove();
            
        //List <int> orderList =  scrambleOrder(nObjects);
        //setNewOrder(nObjects, orderList);
        //resetPosiitons(nObjects); //, orderList);

        }
        */
}
}
