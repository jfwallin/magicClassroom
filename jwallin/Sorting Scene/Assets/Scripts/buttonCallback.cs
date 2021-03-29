using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class buttonCallback : MonoBehaviour
{

    private MagicLeapTools.InputReceiver _inputReceiver;
    public bool enableOnClick = true;

    private void Awake()
    {
        _inputReceiver = GetComponent<MagicLeapTools.InputReceiver>();
        if (_inputReceiver == null)
            Debug.Log("input receiver not found");

    }

    private void OnEnable()
    {
        if (enableOnClick)
            _inputReceiver.OnSelected.AddListener(HandleOnClick);


    }

    private void OnDisable()
    {
        if (enableOnClick)
            _inputReceiver.OnSelected.RemoveListener(HandleOnClick);

    }

    private void HandleOnClick(GameObject sender)
    {

        GameObject sorter = GameObject.Find("sortingManager");

        if (sorter != null)
            sorter.GetComponent<sortingData>().feedbackOnOrder();
    }


}
