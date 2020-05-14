using System.Collections;
using System.Collections.Generic;
using System.Collections.Specialized;
using System.IO;
using System.Security.Cryptography.X509Certificates;
using UnityEngine;


/* I want to test dynamically creating something from a JSON
 * 
 * This needs to read a .json file and then turn it into an object
 * following that it needs to then instantiate a GameObject using that info
 * 
 * The parsing of the JSON may eventually be it's own code but I'll probably leave it here for now.
 */
public class JSONTest1Logic : MonoBehaviour
{
    private JSONTest1 info = new JSONTest1(); //To be instantiated from
    //private GameObject gameObject;
   // private GameObject dynamic;


    // Start is called before the first frame update
    void Start()
    {
        //makeJsonFile(); //comment in when you want to make a new json filec
        getInfo();
        //dynamic = GameObject.Find("[_DYNAMIC]");

        //gameObject = (GameObject)
        Instantiate(Resources.Load(info.id), new Vector3(info.xPosition, info.yPosition, info.zPosition), Quaternion.identity);
        //gameObject.transform.parent = dynamic.transform; 
    }
    /*Notes:
     * Resources.Load(string) grabs things in the resources folder by name.
     * It can't be in a subfolder of resources
     * but the interwebs says you can have more than one resources files (untested)
     */


    // Update is called once per frame
    void Update()
    {

    }

    private void getInfo()
    {
        string path = "Assets/_Local/JSON files/CubeTest0.json"; //Again, this is hardcoded where it will be general later
        StreamReader reader = new StreamReader(path);
        string line;

        line = reader.ReadLine();
        Debug.Log("In reader. Line reads as: " + line);
        info = JsonUtility.FromJson<JSONTest1>(line);

        Debug.Log("id: " + info.id);
    }

    //This is hardcoded for this test but the idea should be able to be moved and made more general
    private void makeJsonFile()
    {
        JSONTest1 test = new JSONTest1();
        string json;
        string path = "Assets/_Local/JSON files/CubeTest0.json";

        StreamWriter writer = new StreamWriter(path);

        test.id = "Cube";
        test.xPosition = 1f;
        test.yPosition = 1f;
        test.zPosition = -1f;
        test.color = "Blue";
        test.scale = 0.25f;

        json = JsonUtility.ToJson(test);
        Debug.Log("json file is: " + json);
        writer.WriteLine(json);

        writer.Close();
    }


}
