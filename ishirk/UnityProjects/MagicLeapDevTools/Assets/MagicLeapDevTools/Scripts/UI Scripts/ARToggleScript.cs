using UnityEngine;
using UnityEngine.UI;
using UnityEngine.Events;
using UnityEngine.XR.MagicLeap;

namespace MtsuMLAR
{
    //This defines a bool Unity Event that allows for editing in the inspector
    [System.Serializable]
    public class ToggleEvent : UnityEvent<bool> { }

    /// <summary>
    /// This class is used on the ARToggle to mimic Unity UI toggle functionality.
    /// </summary>
    public class ARToggleScript : MonoBehaviour, IMLPointerEnterHandler, IMLPointerExitHandler, IMLPointerDownHandler, IMLPointerUpHandler
    {
        #region Private Variables
        //button background reference
        [SerializeField]
        private Image togglebox = null;
        [SerializeField]
        private Image checkmark = null;
        [SerializeField]
        private Collider toggleCollider = null;

        //Colors for the button in different states
        private Color idleColor = Color.white;
        [SerializeField]
        private Color highlightColor = Color.yellow;
        [SerializeField]
        private Color pressColor = Color.green;

        //Set the state
        [SerializeField]
        private bool state = false;

        //Actions to be assigned to by the menu control script, allowing button customization
        public ToggleEvent Toggle;
        #endregion

        #region Unity Methods
        void Start()
        {
            idleColor = togglebox.color;
            if (Toggle == null)
                Toggle = new ToggleEvent();
            checkmark.enabled = state;
        }

        void OnEnable()
        {
            togglebox.enabled = true;
            toggleCollider.enabled = true;
        }

        void OnDisable()
        {
            togglebox.enabled = false;
            toggleCollider.enabled = false;
        }
        #endregion

        #region Event Handlers
        public void MLOnPointerEnter(MLEventData eventData)
        {
            togglebox.color = highlightColor;
            MLInput.GetController(MLInput.Hand.Left).StartFeedbackPatternVibe(MLInputControllerFeedbackPatternVibe.Click, MLInputControllerFeedbackIntensity.Low);
        }

        public void MLOnPointerExit(MLEventData eventData)
        {
            togglebox.color = idleColor;
        }

        public void MLOnPointerDown(MLEventData eventData)
        {
            state = !state;
            togglebox.color = pressColor;
            checkmark.enabled = state;
            Toggle.Invoke(state);
        }

        public void MLOnPointerUp(MLEventData eventData)
        {
            togglebox.color = highlightColor;
        }
        #endregion
    }
}