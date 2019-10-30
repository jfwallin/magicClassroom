using UnityEngine;
using UnityEngine.UI;
using UnityEngine.Events;
using UnityEngine.XR.MagicLeap;

namespace MtsuMLAR
{   
    /// <summary>
    /// This Script is used on the ARButton prefab to work with the event system to replicate
    /// Unity UI Button functionality.
    /// </summary>
    public class ARButtonScript : MonoBehaviour, IMLPointerEnterHandler, IMLPointerExitHandler, IMLPointerDownHandler, IMLPointerUpHandler
    {
        #region Private Variables
        //button background reference
        [SerializeField]
        private Image buttonImage = null;
        [SerializeField]
        private Collider buttonCollider = null;

        //Colors for the button in different states
        [SerializeField]
        private Color idleColor = Color.white;
        [SerializeField]
        private Color highlightColor = Color.yellow;
        [SerializeField]
        private Color pressColor = Color.green;

        //This lets you set what the button does in the Editor
        public UnityEvent PointerDown;
        #endregion

        #region Unity Methods
        void Start()
        {
            //Initialize idleColor and PointerDown event
            idleColor = buttonImage.color;
            if (PointerDown == null)
                PointerDown = new UnityEvent();
        }

        void OnEnable()
        {
            buttonImage.enabled = true;
            buttonCollider.enabled = true;
        }

        void OnDisable()
        {
            buttonImage.enabled = false;
            buttonCollider.enabled = false;
        }
        #endregion

        #region Event Handlers
        public void MLOnPointerEnter(MLEventData eventData)
        {
            buttonImage.color = highlightColor;
            MLInput.GetController(MLInput.Hand.Left).StartFeedbackPatternVibe(MLInputControllerFeedbackPatternVibe.Click, MLInputControllerFeedbackIntensity.Low);
        }

        public void MLOnPointerExit(MLEventData eventData)
        {
            buttonImage.color = idleColor;
        }

        public void MLOnPointerDown(MLEventData eventData)
        {
            buttonImage.color = pressColor;
            PointerDown.Invoke();
        }

        public void MLOnPointerUp(MLEventData eventData)
        {
            buttonImage.color = highlightColor;
        }
        #endregion
    }
}