using UnityEngine;
using UnityEngine.XR.MagicLeap;

namespace MtsuMLAR
{
    /// <summary>
    /// Controls the Line Renderer on the Controller Prefab to draw cursor based on what state
    /// the event system is in. UI objects that don't want the bendy select cursor must have
    /// the "ARUI" Tag
    /// </summary>
    [RequireComponent(typeof(LineRenderer))]
    public class LineControl : MonoBehaviour
    {
        #region Private Variables
        //Line Drawing Variables
        [SerializeField]
        private float cursorExtent = 10.0f;    //How far the cursor is drawn when not pointing at anything
        [SerializeField]
        private int numSegments = 30;          //Higher value means smoother line
        private Vector3[] pointPositions;

        private MLInputController controller;

        //Component Variables
        private MLEventSystem eventSystem;
        private MLInputModuleV2 inputModule;
        private LineRenderer lRend;
        #endregion

        #region Unity Methods
        private void Awake()
        {
            lRend = GetComponent<LineRenderer>();
            lRend.positionCount = numSegments + 1;
            pointPositions = new Vector3[numSegments + 1];
        }

        private void Start()
        {
            MLInput.Start();
            controller = MLInput.GetController(MLInput.Hand.Left);
            if(controller == null)
            {
                controller = MLInput.GetController(MLInput.Hand.Right);
                if(controller == null)
                {
                    Debug.LogWarning("LineControl.cs could not get a reference to the Magic Leap controller, diabling script");
                    enabled = false;
                }
            }

            inputModule = GameObject.FindGameObjectWithTag("MLEventSystem").GetComponent<MLInputModuleV2>();
            if(inputModule == null)
            {
                Debug.LogWarning("Could not Get a reference to the inputModule, disabling script");
                enabled = false;
            }
            eventSystem = GameObject.FindGameObjectWithTag("MLEventSystem").GetComponent<MLEventSystem>();
            if(eventSystem == null)
            {
                Debug.LogWarning("Could not get a reference to the eventSystem, disabling script");
                enabled = false;
            }
        }

        private void OnDestroy()
        {
            MLInput.Stop();
        }

        private void Update()
        {
            transform.position = controller.Position;
            transform.rotation = controller.Orientation;
        }

         /// <summary>
         /// This updates the pointer after everything else should have moved,
         /// depending on if we are hitting an object or not
         /// </summary>
        private void LateUpdate()
        {
            if (eventSystem.IsDragging)
                DrawSelectLine(eventSystem.DraggedObject.transform);
            else if (inputModule.CurrentHitState == MLInputModuleV2.HitState.ObjectHit && inputModule.PrimaryHitObject.tag == "ARUI")
                DrawStraightLine(inputModule.PrimaryHitObjectDistance);
            else if (inputModule.CurrentHitState == MLInputModuleV2.HitState.ObjectHit)
                DrawSelectLine(inputModule.PrimaryHitObject.transform);
            else
                DrawStraightLine(cursorExtent);
        }
        #endregion //Unity Methods

        #region Private Methods
        /// <summary>
        /// Draws a straight line from the front of the controller
        /// </summary>
        /// <param name="length">how long, in unity units, to make th line</param>
        private void DrawStraightLine(float length)
        {
            for (int j = 0; j <= numSegments; j++)
            {
                lRend.sortingOrder = 0;
                float fracComplete = (float)j / numSegments;
                pointPositions[j] = Vector3.Lerp(transform.position, transform.position + (transform.forward * length), fracComplete);
            }
            lRend.SetPositions(pointPositions);
        }

        /*This function draws a curved line to the target transform with a straight
         * beginning, giving it a flexible feeling*/
         /// <summary>
         /// Draws a line that curves to the target
         /// </summary>
         /// <param name="target">Transform of the target object</param>
        private void DrawSelectLine(Transform target)
        {
            lRend.sortingOrder = 4;
            pointPositions[0] = transform.position;
            float firstSegmentScale = (target.position - transform.position).magnitude * 4 / numSegments;
            for (int i = 1; i <= numSegments; i++)
            {
                float fracComplete = (float)i / numSegments;
                pointPositions[i] = Vector3.Slerp(transform.forward * firstSegmentScale, target.position - transform.position, fracComplete) + transform.position;
            }
            lRend.SetPositions(pointPositions);
        }
        #endregion
    }
}