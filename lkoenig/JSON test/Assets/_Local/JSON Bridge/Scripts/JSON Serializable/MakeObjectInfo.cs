using System.Collections;
using System.Collections.Generic;
using UnityEngine;


//Due to the nature of ObjectInfo at this moment this also has to be updated every time a new JsonObject script is added
public class MakeObjectInfo : MonoBehaviour
{
    public string name;

    public Vector3 position;
    public Vector3 scale;
    public string material;

    public int numScriptsToAdd;
    public string[] scriptsToAdd;

    // Start is called before the first frame update
    void Start()
    {
        
    }

    
}
