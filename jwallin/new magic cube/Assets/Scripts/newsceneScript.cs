using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Networking;



using System;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Net.NetworkInformation;




public class newsceneScript : MonoBehaviour
{
    private GameObject dataTarget;
    public Vector3 p1, p2, pcenter, pdiff;
    public string tout;



    public struct myInterface
    {
        public String IP;
        public String Mac;
        public String Name;
        public String Interface;
        public String Status;
    }



    // Start is called before the first frame update
    void Start()
    {
        float x, y, z, dist, theta;
        transform.Rotate(0.0f, 0.0f, 0.0f);
        //Debug.Log("new scene");

        dataTarget = GameObject.Find("dataObject");
        //target = GameObject.FindObjectWithTag("simulationData");
        //Debug.Log("location = " + dataTarget.GetComponent<datacontainer>().cat.ToString());

        p1 = dataTarget.GetComponent<datacontainer>().imageLocation1;
        p2 = dataTarget.GetComponent<datacontainer>().imageLocation2;
        pcenter =  0.5f * (p1 + p2);

        pdiff = p2 - p1;
        transform.position = pcenter;

        x = pdiff[0];
        y = pdiff[1]; 
        z = pdiff[2];
        theta = Mathf.Atan2(x, z) * Mathf.Rad2Deg + 90.0f;
        //Debug.Log("theta" + theta.ToString());
        transform.Rotate(0.0f, theta, 0.0f);

        dist = Mathf.Sqrt(x * x + y * y + z * z);

        transform.localScale = new Vector3(dist, 0.3f, 0.3f);


        myInterface[] ilist = new myInterface[10];
        int icount = 0;
        (icount, ilist) = ShowNetworkInterfaces();
        for (int ii = 0; ii < icount; ii++)
        {
            Debug.Log(" name        = {0}"+ ilist[ii].Name);
            Debug.Log(" IP          = {0}"+ ilist[ii].IP);
            Debug.Log(" MAC         = {0}"+ ilist[ii].Mac);
            Debug.Log(" Interface   = {0}"+ ilist[ii].Interface);
            Debug.Log(" Status      = {0}"+ ilist[ii].Status);
        }



        StartCoroutine(PollText());
        //StartCoroutine(GetText());

    }

    // Update is called once per frame
    void Update()
    {
        
    }


    IEnumerator PollText()
    {

        string waddress = "https://www.cs.mtsu.edu/~jwallin/ML/timestamp.php";
        //long servertime;

        System.DateTime  dtDateTime ;
        long msUnixStart ;
        long millisecondsLocal;
        //long deltat;
          
        // Creating a DateTime object and  
        // taking a particular date and time 

        dtDateTime = new DateTime(1970,1,1,0,0,0,0,System.DateTimeKind.Utc);
        msUnixStart = dtDateTime.Ticks / TimeSpan.TicksPerMillisecond;
        Debug.Log("msUnixStart = " + msUnixStart.ToString() );
        while (true)
        {
            //Debug.Log("start"+Time.time);
            yield return new WaitForSeconds(2);
            //StartCoroutine(GetText());


            UnityWebRequest www = UnityWebRequest.Get(waddress);
            long tserver;
            long tlocal;
            long ttt;
            long tlag;
            yield return www.SendWebRequest();

            if (www.isNetworkError || www.isHttpError)
            {
                Debug.Log(www.error);
            }
            else
            {
                // Show results as text
                tout = www.downloadHandler.text;
                tserver = Convert.ToInt64(tout);
                millisecondsLocal = DateTime.Now.Ticks / TimeSpan.TicksPerMillisecond;
                tlocal = millisecondsLocal-msUnixStart;

                ttt =tserver - tlocal;

                TimeZone zone = TimeZone.CurrentTimeZone;
                TimeSpan offset = zone.GetUtcOffset(DateTime.Now);
                long msOffset = offset.Hours * 3600 * 1000;
                
                tlag = ttt + msOffset;
                Debug.Log("c-s diff = "+ (tlag).ToString());

            }
                //Debug.Log("done" +Time.time);
        }

     


    }


    IEnumerator GetText()
    {
        string waddress = "https://www.cs.mtsu.edu/~jwallin/ML/mltest.txt";




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
            Debug.Log("output = "+tout);

            // Or retrieve results as binary data
            //byte[] results = www.downloadHandler.data;
        }
    }




    public static (int, myInterface[]) ShowNetworkInterfaces()
    {

        myInterface[] ilist = new myInterface[40];
        int icount = 0;

        IPGlobalProperties computerProperties = IPGlobalProperties.GetIPGlobalProperties();
        NetworkInterface[] nics = NetworkInterface.GetAllNetworkInterfaces();
        if (nics == null || nics.Length < 1)
        {
            return (icount, ilist);
        }


        foreach (NetworkInterface ni in NetworkInterface.GetAllNetworkInterfaces())
        {
            ilist[icount].IP = "";
            ilist[icount].Mac = "";
            ilist[icount].Name = "";
            ilist[icount].Interface = "";
            ilist[icount].Status = "";

            if (ni.NetworkInterfaceType == NetworkInterfaceType.Wireless80211 || ni.NetworkInterfaceType == NetworkInterfaceType.Ethernet)
            {
                foreach (UnicastIPAddressInformation ip in ni.GetIPProperties().UnicastAddresses)
                {

                    if (ip.Address.AddressFamily == System.Net.Sockets.AddressFamily.InterNetwork)
                    {
                        ilist[icount].IP = ip.Address.ToString();
                        ilist[icount].Mac = ni.GetPhysicalAddress().ToString();
                        ilist[icount].Name = ni.Name;
                        ilist[icount].Interface = ni.NetworkInterfaceType.ToString();
                        ilist[icount].Status = ni.OperationalStatus.ToString();
                        icount = icount + 1;

                    }
                }
            }
        }

        return (icount, ilist);

    }

}
