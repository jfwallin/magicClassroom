using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEditor;

public class DevMenuTools
{
    //[MenuItem("Tools/Initialize Scene")]
    //private static void InitializeScene()
    //{
    //    Debug.Log("Initializing");
    //    ResetHeadposeMenus();

    //}

    [MenuItem("Tools/Reset Headpose Menus &m")]
    private static void ResetHeadposeMenus()
    {
        Debug.Log("Reseting headpose menus");
        GameObject headposeUIParent = GameObject.Find("HeadposeCanvas");
        if(!headposeUIParent)
        {
            Debug.LogWarning("couldn't find HeadposeCanvas");
            return;
        }
        foreach(Transform phase in headposeUIParent.transform)
        {
            if(phase.name == "OpeningMenus")
            {
                phase.gameObject.SetActive(true);
                foreach(Transform page in phase)
                {
                    if (page.name == "WelcomePage")
                    {
                        page.gameObject.SetActive(true);
                        Debug.Log("Set WelcomePage active");
                    }
                    else
                        page.gameObject.SetActive(false);
                }
            }
            else
            {
                phase.gameObject.SetActive(true);
                foreach (Transform page in phase)
                {
                    page.gameObject.SetActive(false);
                }
                phase.gameObject.SetActive(false);
            }
        }
    }

    [MenuItem("Tools/Call On Handle Peer Found &p")]
    private static void OnHandlePeerFound()
    {
        Debug.Log("Faking finding a peer");
        ConnectingGUIManager manager = GameObject.Find("ConnectingPage")?.GetComponent<ConnectingGUIManager>();
        if (manager != null)
        {
            manager.handleOnPeerFound("somePeer");
            Debug.Log("Peer Faked");
        }
        else
            Debug.LogWarning("Could not find ConnectingGUIManager");
    }
}
