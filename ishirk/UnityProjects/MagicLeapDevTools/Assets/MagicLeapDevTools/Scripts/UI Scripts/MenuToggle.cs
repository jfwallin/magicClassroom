using UnityEngine;
using UnityEngine.XR.MagicLeap;

namespace MtsuMLAR
{
    /// <summary>
    /// This class is to be placed on the group of menuns you want to close or open using
    /// the home button. It listens directly to the controller's global button events
    /// </summary>
    public class MenuToggle : MonoBehaviour
    {
        #region Private Variables
        [SerializeField, Tooltip("The menu to be opened when the home button is pressed")]
        private GameObject mainMenu = null;
        [SerializeField, Tooltip("The menus to check and disable when the home button is pressed")]
        private GameObject[] menus = null;

        private bool menuEnabled = false;
        #endregion

        #region Properties
        public bool MenuEnabled { get => menuEnabled; set => menuEnabled = value; }
        #endregion

        #region Unity Methods
        void Start()
        {
            if(mainMenu == null)
            {
                Debug.LogError("No MainMenu assigned to the Menu Toggle Script, Disabling");
                enabled = false;
            }
            if(menus == null)
            {
                Debug.LogError("No menus assigned to the Main Menu Toggle script, Disabling");
                enabled = false;
            }
            MLInput.Start();
            MLInput.OnControllerButtonDown += Toggle;

            //This checks if any of the menus are enabled by default in this scene
            foreach(GameObject menu in menus)
            {
                if(menu.activeSelf)
                {
                    menuEnabled = true;
                    break;
                }
            }
        }

        private void OnDestroy()
        {
            MLInput.OnControllerButtonDown -= Toggle;
            MLInput.Stop();
        }
        #endregion

        #region Event Handlers
        void Toggle(byte controllerID, MLInputControllerButton button)
        {
            if(button == MLInputControllerButton.HomeTap)
            {
                if (menuEnabled)
                {
                    foreach (GameObject menu in menus)
                    {
                        if (menu.activeSelf != false)
                            menu.SetActive(false);
                    }
                    menuEnabled = false;
                }
                else
                {
                    mainMenu.SetActive(true);
                    menuEnabled = true;
                }
            }
        }
        #endregion

        //I really don't know what uses this...
        public void CloseMenu()
        {
            foreach(GameObject go in menus)
            {
                if(go.activeSelf)
                {
                    go.SetActive(false);
                    break;
                }
            }
            menuEnabled = false;
        }
    }
}