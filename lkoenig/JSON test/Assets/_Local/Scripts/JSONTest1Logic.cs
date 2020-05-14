using System.Collections;
using System.Collections.Generic;
using System.Collections.Specialized;
using System.IO;
using System.Runtime.Versioning;
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
    private GameObject _dynamic;
    private bool grow = true;


    // Start is called before the first frame update
    void Start()
    {
        _dynamic = GameObject.Find("[_DYNAMIC]");

        MakeObject("Sphere");
        MakeObject("Cube");
        //makeJsonFile(); //comment in when you want to make a new json filec
        
    }
    /*Notes:
     * Resources.Load(string) grabs things in the resources folder by name.
     * It can't be in a subfolder of resources
     * but you can have more than one resources file
     */


    // Update is called once per frame
    void Update()
    {
        Vector3 scaleChange = new Vector3(0.01f, 0.01f, 0.01f);
        
        if (grow == true)
        {
            _dynamic.transform.localScale += scaleChange;
            if (_dynamic.transform.localScale.x >= 1f)
            {
                grow = false;
            }

        }
        else
        {
            _dynamic.transform.localScale -= scaleChange;
            if (_dynamic.transform.localScale.x <= 0.1f) grow = true;
        }
        
    }

    private void getInfo(string path)
    {
        //string path = "Assets/_Local/JSON files/SphereTest0.json"; //Again, this is hardcoded where it will be general later
        StreamReader reader = new StreamReader(path);
        string line;

        line = reader.ReadLine();
        Debug.Log("In reader. Line reads as: " + line);
        info = JsonUtility.FromJson<JSONTest1>(line);

        Debug.Log("id: " + info.id);
    }

    private void MakeObject(string typeName)
    {
        GameObject myObject;
        getInfo("Assets/_Local/JSON files/" + typeName+"Test0.json"); //serializes the json data and makes it an obect that I can access. 

        myObject = (GameObject)Instantiate(Resources.Load(info.id), new Vector3(info.xPosition, info.yPosition, info.zPosition), Quaternion.identity);
        myObject.transform.parent = _dynamic.transform; //Puts the intantiated object in the proper location in the Hierarchy
        myObject.name = info.id; //renames the gameobject in the hierarchy so it should be easier to find by other scripts

        Renderer rend = myObject.GetComponent<Renderer>(); //This grabs the renderer to change the material
        rend.material = Resources.Load<Material>(info.color); //This assigns the material

        myObject.transform.localScale = new Vector3(info.scale, info.scale, info.scale);
    }

    //This is hardcoded for this test but the idea should be able to be moved and made more general
    private void makeJsonFile()
    {
        JSONTest1 test = new JSONTest1();
        string json;
        string path = "Assets/_Local/JSON files/SphereTest0.json";

        StreamWriter writer = new StreamWriter(path);

        test.id = "Sphere";
        test.xPosition = 0f;
        test.yPosition = 1f;
        test.zPosition = -2f;
        test.color = "Red";
        test.scale = 0.4f;

        json = JsonUtility.ToJson(test);
        Debug.Log("json file is: " + json);
        writer.WriteLine(json);

        writer.Close();
    }


}
