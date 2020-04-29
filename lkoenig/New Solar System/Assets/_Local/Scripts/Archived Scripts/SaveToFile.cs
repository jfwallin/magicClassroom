using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.IO;

public class SaveToFile : MonoBehaviour
{
    // Start is called before the first frame update
    void Start()
    {
        string startText = "The start function has exicuted in the save class";
        ToFile(startText);
    }

    // Update is called once per frame
    void Update()
    {
        
    }

    public void ToFile(string info)
    {
        FileStream file = File.Create("testFile.txt");

        //file.WriteLine(info);
 
        file.Close();
    }
}
