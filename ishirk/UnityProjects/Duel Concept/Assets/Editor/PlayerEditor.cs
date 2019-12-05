using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEditor;

[CustomEditor(typeof(Player))]
public class PlayerEditor : Editor
{
    private void OnSceneGUI()
    {
        Player playerScript = (Player)target;

        EditorGUI.BeginChangeCheck();
        Vector3 newRightShoulderPosition = Handles.PositionHandle(playerScript.transform.TransformPoint(playerScript.rightShoulderPosition), Quaternion.identity);
        if(EditorGUI.EndChangeCheck())
        {
            Undo.RecordObject(playerScript, "Change right shoulder position");
            playerScript.rightShoulderPosition = newRightShoulderPosition;

        }

        EditorGUI.BeginChangeCheck();
        Vector3 newLeftShoulderPosition = Handles.PositionHandle(playerScript.transform.TransformPoint(playerScript.leftShoulderPosition), Quaternion.identity);
        if(EditorGUI.EndChangeCheck())
        {
            Undo.RecordObject(playerScript, "Changed left shoulder position");
            playerScript.leftShoulderPosition = newLeftShoulderPosition;

        }
    }

    [DrawGizmo(GizmoType.Selected | GizmoType.NonSelected, typeof(Player))]
    static void DrawShoulderGizmos(Player playerObj, GizmoType type)
    {
        Gizmos.color = Color.red;
        Gizmos.DrawSphere(playerObj.transform.TransformPoint(playerObj.rightShoulderPosition), 0.05f);
        Gizmos.color = Color.blue;
        Gizmos.DrawSphere(playerObj.transform.TransformPoint(playerObj.leftShoulderPosition), 0.05f);
    }
}
