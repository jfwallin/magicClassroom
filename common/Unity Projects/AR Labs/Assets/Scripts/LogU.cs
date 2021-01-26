using System.Collections;
using System.Collections.Generic;
using UnityEngine;

/// <summary>
/// Debug Utility class, calls debug for you or returns formatted messages
/// </summary>
public static class LogU
{
    /// <summary>
    /// A warning with a "{refName} not assigned in inspector, searching for reference" message
    /// </summary>
    /// <param name="refName">name of missing reference</param>
    public static void RNFI(string refName)
    {
        Debug.LogWarning($"{refName} not assigned in inspector, searching for reference");
    }
}
