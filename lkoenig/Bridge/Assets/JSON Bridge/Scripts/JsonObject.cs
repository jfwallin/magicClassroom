using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class JsonObject : MonoBehaviour
{ 

    public virtual void Construct(string info)
    {
        /*
         * This virtual function will get overloaded in the scripts that can be attached to Objects from JSONs.
         * Use "public override void Construct(string info){}" to create a specific contruct function in the script.
         * 
         * The inside of this function should include a getInfo thing to grab the JSON stuff you need for variables.
         */
    }


    public T getInfo<T>(string toBe)
    {
        T info = JsonUtility.FromJson<T>(toBe);
        return info;
    }
}
/*
 * Scripts that are to be attached to objects contructed with JSONs should inherit from this.
 * Inheriting from this allows you to create a contruct function that can be called from outside the class without knowing what the class actually is.
 * This allows us to generically add functionality in a very flexible manner without knowing exactly what type of script I'm adding.
 * It also allows the scripts to handle the assignment of their variables from the scripts themselves.
 * 
 * Every script that inherits from this should also have a serializable script written to match it that contains the apropriet public variables.
 */


