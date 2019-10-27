
using System;
using System.Collections;
using System.Collections.Generic;
using System.Net;
using System.Net.Sockets;
using System.Text;
using UnityEngine;

public class udpManager : MonoBehaviour 
{
    UdpClient udpClient;
    IPEndPoint RemoteIpEndPoint;
    public GameObject dataTarget;
    int messageReceived;
    public string IPAddressString = "127.0.0.1";
    public int EndpointPort = 5006; 
    public string MessageString = "Hello from Unity3D!";
        
    public Vector3 mylocation, myrotation;

    //port exposed for unity
    [SerializeField] int EndPointPort = 9999;

    private string jsonStuff;

    [Serializable]
    private class headpose
    {

        public Vector3 mylocation;
        public Vector3 myrotation;
    }

    headpose theHeadPose = new headpose();


        // Use this for initialization
        void Start () 
    {
        
        mylocation = new Vector3(0.0f, 0.0f, 0.0f);
        myrotation = new Vector3(0.0f, 0.0f, 0.0f);
        RemoteIpEndPoint = new IPEndPoint(IPAddress.Any, EndPointPort);

        udpClient = new UdpClient();
        udpClient.Client.Bind(RemoteIpEndPoint);

        messageReceived = 0;
        PrepareForUDP();

        
        }


    //init an asynchronous callback to be called on reciept of a udp packet
    void PrepareForUDP()
    {
        AsyncCallback callback = new AsyncCallback(UDPMsgRecievedCallback);
        udpClient.BeginReceive(callback, null);
    }

    void Update()
    {
        if (messageReceived == 1){
            messageReceived = 0;

            mylocation = dataTarget.GetComponent<datacontainer>().mylocation;
            myrotation = dataTarget.GetComponent<datacontainer>().myrotation;
            theHeadPose.mylocation = mylocation;
            theHeadPose.myrotation  = myrotation;
            
            
            jsonStuff = JsonUtility.ToJson(theHeadPose );
            Debug.Log("json = "+jsonStuff);
            Debug.Log("loc"+ mylocation);
            MessageString = jsonStuff ;
            //JsonUtility.FromJsonOverwrite(json, myObject);
            /*
            
            //Convert to Jason  
            Player[] playerInstance = new Player[2];
            ...
            string playerToJason = JsonHelper.ToJson(playerInstance, true);
            Debug.Log(playerToJason);

            Player[] player = JsonHelper.FromJson<Player>(jsonString);
            Debug.Log(player[0].playerLoc);
            Debug.Log(player[1].playerLoc);

             */
    
            UDPSend();
            PrepareForUDP();
        }

    }

    /// <summary>
    /// Function designed to be called when a UDP message is recieved
    /// if we wanted to listen for the next we might put another 
    /// 'BeginReceive()' method at the end of this method.
    /// </summary>
    /// <param name="result">Result.</param>
    private void UDPMsgRecievedCallback(IAsyncResult result)
{
        byte[] received = udpClient.EndReceive(result, ref RemoteIpEndPoint);
        string receiveString = Encoding.ASCII.GetString(received);
        Debug.Log("Received an encoded string: "+ receiveString);
        messageReceived = 1;
    }

    void UDPSend()
    {
        //init socket object
        Socket sock = new Socket(AddressFamily.InterNetwork, 
                                 SocketType.Dgram,
                                 ProtocolType.Udp);

        //parse the IP address
        IPAddress serverAddr = IPAddress.Parse(IPAddressString);
        IPEndPoint endPoint = new IPEndPoint(serverAddr, EndpointPort);

        //convert to bytes
        byte[] send_buffer = Encoding.ASCII.GetBytes(MessageString);
        sock.SendTo(send_buffer, endPoint);

        //close up socket object
        sock.Close();
    }



    private void OnDestroy()
    {
        // make sure to clean up sockets on exit
        udpClient.Client.Close();
        udpClient.Close();
    }
}
