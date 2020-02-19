using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEditor;

[CustomEditor(typeof(Settings))]
public class SettingsEditor : Editor
{
    public override void OnInspectorGUI()
    {
        Settings S = (Settings)target;
        DrawDefaultInspector();
        if(GUILayout.Button("Apply Settings"))
        {
            S.ApplySettings();
        }
    }
}
