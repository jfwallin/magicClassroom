using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.XR.MagicLeap;


/*So this is going to be the slider script for the AR Slider Prefab
 * It should work with Isaac's controller package
 * ie his event system
 * 
 * It uses namespace MtsuMLAR so that it can inherit and use callback functions that are part of that package
 * Not sure which ones yet.
 * But will probably need them
 */
namespace MtsuMLAR
{
    public class ARSliderScript : MonoBehaviour, IMLPointerEnterHandler, IMLPointerExitHandler, IMLBeginDragHandler, IMLDragHandler, IMLEndDragHandler
    {

        /*Basic Idea is that there are two colliders
         * 1) the main box collider that encompasses everything (might not actually need that)
         * 2) the sphere collider on the handle itself
         * Should be able to grab the handle and drag it to anywhere on the slider
         * won't leave the slider (will need some contraint of some kind for that)
         * 
         * These are going to have to be event handlers of some kind
         * I should know more about event handlers
         * Need to figure out which ones to include and how to implement them
         * 
         * There also need to be a float value that is location of the handle relitive to the slider
         * 
         * Some way for it to do something
         * At first that will just be set something based on the float
         * in my case the time mulitiplier (can be hardcoaded for testing)
         */


        private void MoveTheBall(float xPosition)
        {
        transform.GetChild(0).position = new Vector3(xPosition, 0, 0);
        }

        #region Event Handlers
        public void MLOnPointerEnter(MLEventData eventData)
        {
            //this will make the slider "highlighted" when hoveing over it
        }

        public void MLOnPointerExit(MLEventData eventData)
        {
            //This will remove highlight when no longer hovering
        }

        public void MLOnBeginDrag(MLEventData eventData)
        {
            //not convinced I actually need this

        }

        public void MLOnDrag(MLEventData eventData)
        {
            //This will be the workhorse function
            //Might make it call other functions for readability/encapsulation purposes

            /*This needs to:
             * Change the position of the sphere based on the pointer
             * constrain that change to be only in one dimention
             * report that change to some variable
             * something else might change based on that but I need to think about that.
             */
            
            MoveTheBall(transform.InverseTransformPoint(eventData.CurRayHit.point).x);
        }

        public void MLOnEndDrag(MLEventData eventData)
        {
            //Also not convinced I need this
        }
        #endregion

    }
}



