using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEditor;
using MagicLeapTools;

public class DevMenuTools : MonoBehaviour
{
    //[MenuItem("Tools/Initialize Scene")]
    //private static void InitializeScene()
    //{
    //    Debug.Log("Initializing");
    //    ResetHeadposeMenus();

    //}
#if UNITY_EDITOR
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
                Debug.Log("Enabling Opening Menus");
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
            else if(phase.name == "InGameMenus")
            {
                foreach(Transform page in phase)
                {
                    if (page.name == "StartConfirmationPage")
                    {
                        page.gameObject.SetActive(true);
                        Debug.Log("Set StartConfirmationPage active");
                    }
                    else
                        page.gameObject.SetActive(false);
                }
                phase.gameObject.SetActive(false);
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

    [MenuItem("Tools/Fake Player Readiness &#p")]
    private static void FakePlayerReadiness()
    {
        Debug.Log("Faking player readiness");
        Transmission.SetGlobalBool("HostReady", true);
        Transmission.SetGlobalBool("PeerReady", true);
    }

    [MenuItem("Tools/Call On Handle Peer Found &p")]
    private static void OnHandlePeerFound()
    {
        Debug.Log("Faking finding a peer");
        GameObject.Find("SceneControl").GetComponent<SceneControl>().FakeConnection();
            Debug.Log("Peer Faked");
    }
#endif

    public void PublicOnHandlePeerFound()
    {
        Debug.Log("Faking finding a peer");
        GameObject.Find("SceneControl").GetComponent<SceneControl>().FakeConnection();
        Debug.Log("Peer Faked");
    }

    public void PublicFakePlayerReadiness()
    {
        Debug.Log("Faking player readiness");
        Transmission.SetGlobalBool("HostReady", true);
        Transmission.SetGlobalBool("PeerReady", true);
    }
}  
