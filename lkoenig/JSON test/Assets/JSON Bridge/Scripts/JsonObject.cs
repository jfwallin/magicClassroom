using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class JsonObject : MonoBehaviour
{
    public virtual void Construct(ScriptVariables info)
    {
        /*
         * This virtual function will get overloaded in the scripts that can be attached to Objects from JSONs.
         * Use "public override void Construct(ScriptVariables info){}" to create a specific contruct function in the script.
         */
    }
}
/*
 * Scripts that are to be attached to objects contructed with JSONs should inherit from this.
 * Inheriting from this allows you to create a contruct function that can be called from outside the class without knowing what the class actually is.
 * This allows us to generically add functionality in a very flexible manner without knowing exactly what type of script I'm adding.
 * It also allows the scripts to handle the assignment of their variables from the scripts themselves.
 */
