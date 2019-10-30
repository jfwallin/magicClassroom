///********************************************************///
///-Please note that OnInitializePotentialDrag has not been 
///integrated into the event logic, so you can only use
///OnBeginDrag
///-If the drag system being used relies on forces applied
///to a rigid body, then the onDrag call and its
///ssurrounding logic need to be put in fixed update
///********************************************************///

using UnityEngine;
using UnityEngine.XR.MagicLeap;

namespace MtsuMLAR
{
    /// <summary>
    /// The MLEventSystem takes info from the Input module and determines when certain events need
    /// to be fired and at which objects. It calls the event only if the object has a script that 
    /// implements the matching interface.
    /// </summary>
    [RequireComponent(typeof(MLInputModuleV2))]
    public class MLEventSystem : MonoBehaviour
    {
        enum clickButton { trigger, bumper }

        #region Private Variables
        [SerializeField, Tooltip("Sets primary click, secondary click auto set")]
        private clickButton primaryClick;

        private GameObject lastHitObject = null;      //Object hit last frame, compared to the InputModule current hit to find transition events
        private GameObject lastSelectedObject = null; //Object considered selected
        private bool isDragging = false;              //Drag Flag
        private GameObject draggedObject = null;      //Object currently being dragged

        private GameObject clickDownObject = null;    //Used to see if a button is released on the same object
        private GameObject click_2_DownObject = null;
        private float bumperTimer;                    //Time between button down and up, used to see if a button up event is to be considered a click
        private float triggerTimer;
        private MLEventData eventData;                //Data sent to objects when events are called

        private MLInputModuleV2 inputModule;          //Script that determines what is being pointed at based on current input method

        private MLInputController _controller;        //Object used to access controller events

        /// Variables to cache references to event handlers called every frame,
        /// so calling GetComponent() every frame can be avoided
        private IMLPointerStayHandler stayHandler = null;
        private IMLUpdateSelectedHandler updateSelectedHandler = null;
        private IMLInitializePotentialDragHandler potentialDragHandler;
        private IMLDragHandler dragHandler = null;

        [SerializeField, Tooltip("Set the time window in which a click must be completed for it to register")]
        private float clickWindow = 0.8f;
        #endregion

        #region Public Properties
        public GameObject LastHitObject { get => lastHitObject; }
        public GameObject LastSelectedObject { get => lastSelectedObject; }
        public bool IsDragging { get => isDragging; }
        public GameObject DraggedObject { get => draggedObject; }
        #endregion

        #region Unity Methods
        private void Awake()
        {
            primaryClick = clickButton.trigger;
            eventData = new MLEventData();
        }

        // Start initializes references, the MLInput API, and event subscriptions
        void Start()
        {
            if (!MLInput.Start().IsOk)
            {
                Debug.LogWarning("MLInput failed to start, disabling MLEventSystem");
                enabled = false;
            }

            _controller = MLInput.GetController(MLInput.Hand.Left);
            if(_controller == null)
            {
                _controller = MLInput.GetController(MLInput.Hand.Right);
                if(_controller == null)
                {
                    Debug.LogWarning("Couldn't get a reference to the Controller, disabling MLEventSystem");
                    enabled = false;
                }
            }

            MLInput.OnControllerButtonDown += ControllerButtonDownHandler;
            MLInput.OnControllerButtonUp += ControllerButtonUpHandler;
            MLInput.OnTriggerDown += TriggerDownHandler;
            MLInput.OnTriggerUp += TriggerUpHandler;

            inputModule = GetComponent<MLInputModuleV2>();
            if(inputModule == null)
            {
                Debug.LogWarning("Couldn't get a reference to the InputModule, disabling script");
                enabled = false;
            }
        }

        private void OnDestroy()
        {
            MLInput.OnControllerButtonDown -= ControllerButtonDownHandler;
            MLInput.OnControllerButtonUp -= ControllerButtonUpHandler;
            MLInput.OnTriggerDown -= TriggerDownHandler;
            MLInput.OnTriggerUp -= TriggerUpHandler;

            MLInput.Stop();
        }

        /// <summary>
        /// The update function takes the current hit object from the input module and compares it
        /// to the last hit object in order to detect transitions and call necessary events.
        /// </summary>
        void Update()
        {
            UpdateEventData(eventData);

            //If we aren't dragging, then check we are interested in what is being pointed at
            if (!isDragging)
            {
                if (inputModule.CurrentHitState != MLInputModuleV2.HitState.NoHit)
                {
                    ///The current hit object to compare with,
                    ///Tries to find an object in the hierarchy with event handlers,
                    ///If none can be found it uses the base hit object
                    GameObject hitObject = SearchForEventHandlerInAncestors(inputModule.PrimaryHitObject);
                    if (hitObject == null)
                        hitObject = inputModule.PrimaryHitObject;

                    //Hit new object
                    if (lastHitObject == null)
                    {
                        //Call handler if it exists
                        IMLPointerEnterHandler enterHandler = hitObject.GetComponent<IMLPointerEnterHandler>();
                        if (enterHandler != null)
                        {
                            enterHandler.MLOnPointerEnter(eventData);
                        }
                        //cache handler reference to be called every frame
                        stayHandler = hitObject.GetComponent<IMLPointerStayHandler>();
                        lastHitObject = hitObject;
                    }
                    //Hit same object
                    else if (hitObject == lastHitObject)
                    {
                        //Call handler if it exists
                        if (stayHandler != null)
                        {
                            stayHandler.MLOnPointerStay(eventData);
                        }
                    }
                    else //We left an object
                    {
                        //Call handler if it exists
                        IMLPointerExitHandler exitHandler = lastHitObject.GetComponent<IMLPointerExitHandler>();
                        if (exitHandler != null)
                        {
                            exitHandler.MLOnPointerExit(eventData);
                        }
                        
                        //This causes the Event system to go into the hit new object logic above next update
                        lastHitObject = null;
                    }
                }
                else //Not hitting anything
                {
                    //If we switched from hitting an object to not, call its exit handler
                    if (lastHitObject != null)
                    {
                        //Call handler if it exists
                        IMLPointerExitHandler exitHandler = lastHitObject.GetComponent<IMLPointerExitHandler>();
                        if (exitHandler != null)
                        {
                            exitHandler.MLOnPointerExit(eventData);
                        }
                        lastHitObject = null;
                    }
                }
            }
            else  //isDragging == true
            {
                //****If the drag update is physics base, then this must be placed in fixed updat****

                ///When a drag starts, the dragged object stays the lastHitObject, since the event system stops updating it until the drag stops
                ///If a drag starts, its basically assured there is an object to reference, but this makes sure it hasn't been spontaneously deleted
                if (lastHitObject != null)
                {
                    if (dragHandler != null)
                    {
                        dragHandler.MLOnDrag(eventData);
                    }
                }
            }

            //Selected update every frame
            if (lastSelectedObject != null)
            {
                if (updateSelectedHandler != null)
                {
                    updateSelectedHandler.MLOnUpdateSelected(eventData);
                }
            }
        }
        #endregion //Unity Methods

        #region Private Methods
        /// <summary>
        /// Updates eventData with the most current information from the event system
        /// </summary>
        /// <param name="eventData">Class containing important data for the recieving methods to use</param>
        private void UpdateEventData(MLEventData eventData)
        {
            eventData.CurrentSelectedObject = lastSelectedObject;
            eventData.PointerTransform = inputModule.PrimaryInputPointerObject.transform;
            eventData.CurrentHitObject = inputModule.PrimaryHitObject;
            eventData.CurRayHit = inputModule.CurRayHit;
        }

        /// <summary>
        /// This searches an object and its ancestors for any eventhandler,
        /// allowing for colliders placed on children of an object with an event handling behavior
        /// to be registered as an interactable object by the input system.
        /// returns null if no handler is found
        /// </summary>
        /// <param name="searchObject">object to begin the search</param>
        private GameObject SearchForEventHandlerInAncestors(GameObject searchObject)
        {
            if (searchObject?.GetComponent<IMLEventHandler>() != null)
                return searchObject;
            else
            {
                searchObject = searchObject.transform.parent?.gameObject;
                while(searchObject != null)
                {
                    if (searchObject.GetComponent<IMLEventHandler>() != null)
                    {
                        return searchObject;
                    }
                    else
                    {
                        searchObject = searchObject.transform.parent?.gameObject;
                    }
                }
                return null;
            }
        }
        #endregion //Private Methods

        #region Button Handlers
        /// <summary>
        /// Called on controller button down events to determine what events to send
        /// to the objects the event system is interacting with
        /// </summary>
        /// <param name="controllerID">ID of the controller that sent the event</param>
        /// <param name="button">Enum for what button sent the event</param>
        public void ControllerButtonDownHandler(byte controllerID, MLInputControllerButton button)
        {
            //Controller validation, check button pressed was bumper
            if (_controller != null && _controller.Id == controllerID && button == MLInputControllerButton.Bumper)
            {
                UpdateEventData(eventData);
                
                //Timer so the button up handler can see how long it was held
                bumperTimer = Time.time;

                //Only call events if not dragging
                if (!isDragging)
                {
                    if (lastHitObject != null)
                    {
                        if (primaryClick == clickButton.bumper)
                        {
                            //Used to check if the button is released on the same object in the button up handelr
                            clickDownObject = lastHitObject;

                            //Call handler if it exists
                            IMLPointerDownHandler downHandler = lastHitObject.GetComponent<IMLPointerDownHandler>();
                            if (downHandler != null)
                            {
                                downHandler.MLOnPointerDown(eventData);
                            }
                            
                            //Used to delay the start of drags until some condition is met, or data is set
                            IMLInitializePotentialDragHandler potentialDragHandler = lastHitObject.GetComponent<IMLInitializePotentialDragHandler>();
                            if (potentialDragHandler != null)
                            {
                                potentialDragHandler.MLOnInitializePotentialDrag(eventData);
                            }
                            else //Try to begin drag
                            {
                                //For an object to be considered draggable, it must implement the beginDrag and Drag interfaces
                                IMLBeginDragHandler beginDragHandler = lastHitObject.GetComponent<IMLBeginDragHandler>();
                                if (beginDragHandler != null)
                                {
                                    beginDragHandler.MLOnBeginDrag(eventData);
                                    isDragging = true;
                                    draggedObject = lastHitObject;

                                    //cache reference to drag handler
                                    dragHandler = lastHitObject.GetComponent<IMLDragHandler>();
                                    if (LastHitObject != lastSelectedObject)
                                    {
                                        //The last selected object is no longer selected once a drag starts
                                        IMLDeselectHandler deselectHandler = lastSelectedObject?.GetComponent<IMLDeselectHandler>();
                                        if (deselectHandler != null)
                                        {
                                            deselectHandler.MLOnDeselect(eventData);
                                            lastSelectedObject = null;
                                        }
                                    }
                                }
                            }
                        }
                        else //Bumper is not primary click
                        {
                            click_2_DownObject = lastHitObject;
                            IMLPointer_2_DownHandler down_2_Handler = lastHitObject.GetComponent<IMLPointer_2_DownHandler>();
                            if (down_2_Handler != null)
                            {
                                down_2_Handler.MLOnPointer_2_Down(eventData);
                            }
                        }
                    }
                    else //lastHitObject == null, not hitting anything
                    {
                        //set clickdownobjects to null so that the up handler function knows the down click was on nothing
                        if (primaryClick == clickButton.bumper)
                            clickDownObject = null;
                        else
                            click_2_DownObject = null;
                    }
                }
            }
        }

        /// <summary>
        /// Called on controller button up events to determine what events to send
        /// to the objects the event system is interacting with
        /// </summary>
        /// <param name="controllerID">ID of the controller that sent the event</param>
        /// <param name="button">Enum for what button sent the event</param>
        void ControllerButtonUpHandler(byte controllerID, MLInputControllerButton button)
        {
            //Controller validation, check button pressed was bumper
            if (_controller != null && _controller.Id == controllerID && button == MLInputControllerButton.Bumper)
            {
                UpdateEventData(eventData);
                //Only call events if not dragging
                if (!isDragging)
                {
                    if (lastHitObject != null)
                    {
                        if (primaryClick == clickButton.bumper)
                        {
                            //Call handler if it exists
                            IMLPointerUpHandler upHandler = lastHitObject.GetComponent<IMLPointerUpHandler>();
                            if (upHandler != null)
                            {
                                upHandler.MLOnPointerUp(eventData);
                            }

                            //If button up was on the same object as button down, consider select or click handlers
                            if (clickDownObject != null && lastHitObject == clickDownObject)
                            {
                                //Quick enough release is a click
                                if (Time.time - bumperTimer < clickWindow)
                                {
                                    IMLPointerClickHandler clickHandler = lastHitObject.GetComponent<IMLPointerClickHandler>();
                                    if (clickHandler != null)
                                    {
                                        clickHandler.MLOnPointerClick(eventData);
                                    }
                                }
                                //Call select handler on new object
                                if (lastSelectedObject == null)
                                {
                                    //Call handler if it exists, update selected object only if the hit object has a select handler indicating it is selectable
                                    IMLSelectHandler selectHandler = lastHitObject.GetComponent<IMLSelectHandler>();
                                    if (selectHandler != null)
                                    {
                                        selectHandler.MLOnSelect(eventData);
                                        lastSelectedObject = lastHitObject;
                                        IMLUpdateSelectedHandler updateSelectedHandler = lastHitObject.GetComponent<IMLUpdateSelectedHandler>();
                                        UpdateEventData(eventData);
                                    }
                                }
                                //If there was a previously selected object, call its deselect only if the newly hit object is selectable, i.e. has a select handler
                                else if (lastHitObject != lastSelectedObject)
                                {
                                    IMLSelectHandler selectHandler = lastHitObject.GetComponent<IMLSelectHandler>();
                                    if (selectHandler != null)
                                    {
                                        selectHandler.MLOnSelect(eventData);
                                        IMLDeselectHandler deselectHandler = lastSelectedObject.GetComponent<IMLDeselectHandler>();
                                        if (deselectHandler != null)
                                        {
                                            deselectHandler.MLOnDeselect(eventData);//checks for not null
                                        }
                                        //This caches the reference to the update select handler if it exists
                                        IMLUpdateSelectedHandler updateSelectedHandler = lastHitObject.GetComponent<IMLUpdateSelectedHandler>();
                                        lastSelectedObject = lastHitObject;
                                    }
                                }
                            }
                        }
                        else //Bumper is not primary click
                        {
                            IMLPointer_2_UpHandler up_2_Handler = lastHitObject.GetComponent<IMLPointer_2_UpHandler>();
                            if (up_2_Handler != null)
                            {
                                up_2_Handler.MLOnPointer_2_Up(eventData);
                            }
                            //If click started and ended on the same object, and was within the time window, treat it as a click
                            if (click_2_DownObject != null && lastHitObject == click_2_DownObject && Time.time - bumperTimer < clickWindow)
                            {
                                IMLPointer_2_ClickHandler click_2_Handler = lastHitObject.GetComponent<IMLPointer_2_ClickHandler>();
                                if (click_2_Handler != null)
                                {
                                    click_2_Handler.MLOnPointer_2_Click(eventData);
                                }
                            }
                        }
                    }
                    else //Released button on empty space
                    {
                        ///If bumper is the primary button(can change selection), and it also began the click on empty space, then it is deselecting
                        ///Also check if there is a last selected object to now deselect
                        if (primaryClick == clickButton.bumper && clickDownObject == null && lastSelectedObject != null)
                        {
                            IMLDeselectHandler deselectHandler = lastSelectedObject.GetComponent<IMLDeselectHandler>();
                            if (deselectHandler != null)
                            {
                                deselectHandler.MLOnDeselect(eventData);
                            }
                            lastSelectedObject = null;
                        }
                    }
                }
                //if dragging and the primary button is released, stop the drag
                else if (isDragging == true && primaryClick == clickButton.bumper)
                {
                    IMLEndDragHandler endDragHandler = lastHitObject.GetComponent<IMLEndDragHandler>();
                    if (endDragHandler != null)
                    {
                        endDragHandler.MLOnEndDrag(eventData);
                    }
                    //Double check that the dragged object isn't already the selected object
                    if (draggedObject != lastSelectedObject)
                    {
                        //Call select handler at the end of the drag, update selected object
                        IMLSelectHandler selectHandler = draggedObject.GetComponent<IMLSelectHandler>();
                        if (selectHandler != null)
                        {
                            selectHandler.MLOnSelect(eventData);
                            lastSelectedObject = draggedObject;
                        }
                    }
                    isDragging = false;
                    draggedObject = null;
                }
            }
        }
        #endregion //Button Handlers

        #region Trigger Handlers
        /// <summary>
        /// Called on controller trigger down events to determine what events to send
        /// to the objects the event system is interacting with
        /// </summary>
        /// <param name="controllerID">ID of the controller that sent the event</param>
        /// <param name="triggerValue">Float,0-1, how much the trigger is pressed</param>
        void TriggerDownHandler(byte controllerID, float triggerValue)
        {
            if (_controller != null && _controller.Id == controllerID)
            {
                UpdateEventData(eventData);
                //Only call events if not dragging
                if (!isDragging)
                {
                    if (lastHitObject != null)
                    {
                        //Timer so the trigger up handler can see how long it was held
                        triggerTimer = Time.time;
                        if (primaryClick == clickButton.trigger)
                        {
                            //Used to check if the button is released on the same object in the trigger up handelr
                            clickDownObject = lastHitObject;
                            IMLPointerDownHandler downHandler = lastHitObject.GetComponent<IMLPointerDownHandler>();
                            if (downHandler != null)
                            {
                                downHandler.MLOnPointerDown(eventData);
                            }
                            //Used to delay the start of drags until some condition is met, or data is set
                            IMLInitializePotentialDragHandler potentialDragHandler = lastHitObject.GetComponent<IMLInitializePotentialDragHandler>();
                            if (potentialDragHandler != null)
                            {
                                potentialDragHandler.MLOnInitializePotentialDrag(eventData);
                            }
                            else //Try to begin drag
                            {
                                //For an object to be considered draggable, it must implement the beginDrag and drag interfaces
                                IMLBeginDragHandler beginDragHandler = lastHitObject.GetComponent<IMLBeginDragHandler>();
                                if (beginDragHandler != null)
                                {
                                    beginDragHandler.MLOnBeginDrag(eventData);
                                    isDragging = true;
                                    draggedObject = lastHitObject;
                                    dragHandler = lastHitObject.GetComponent<IMLDragHandler>();

                                    //cache reference to drag handler
                                    if (lastSelectedObject != null && LastHitObject != lastSelectedObject)
                                    {
                                        //This is done so that the last selected object is no longer selected once a drag starts
                                        IMLDeselectHandler deselectHandler = lastSelectedObject?.GetComponent<IMLDeselectHandler>();
                                        if (deselectHandler != null)
                                        {
                                            deselectHandler.MLOnDeselect(eventData);
                                            lastSelectedObject = null;
                                        }
                                    }
                                }
                            }
                        }
                        else //Trigger is not primary click
                        {
                            click_2_DownObject = lastHitObject;//To be checked before select and click is called in up handler
                            IMLPointer_2_DownHandler down_2_Handler = lastHitObject.GetComponent<IMLPointer_2_DownHandler>();
                            if (down_2_Handler != null)
                            {
                                down_2_Handler.MLOnPointer_2_Down(eventData);
                            }
                        }
                    }
                    else //not hitting anything
                    {
                        //reset clickdownobjects so that the up handler function knows the down click was on nothing
                        if (primaryClick == clickButton.trigger)
                            clickDownObject = null;
                        else
                            click_2_DownObject = null;
                    }
                }
            }
        }

        /// <summary>
        /// Called on controller trigger up events to determine what events to send
        /// to the objects the event system is interacting with
        /// </summary>
        /// <param name="controllerID">ID of the controller that sent the event</param>
        /// <param name="triggerValue">Float,0-1, how much the trigger is pressed</param>
        void TriggerUpHandler(byte controllerID, float triggerValue)
        {
            if (_controller != null && _controller.Id == controllerID)
            {
                UpdateEventData(eventData);
                if (!isDragging)
                {
                    if (lastHitObject != null)
                    {
                        if (primaryClick == clickButton.trigger)
                        {
                            IMLPointerUpHandler upHandler = lastHitObject.GetComponent<IMLPointerUpHandler>();
                            if (upHandler != null)
                            {
                                upHandler.MLOnPointerUp(eventData);
                            }
                            //Check to see if the clickdown started and ended on the same object. If it did, consider click or select handlers
                            if (clickDownObject != null && lastHitObject == clickDownObject)
                            {
                                //If the release was quick enough, it is considered a click
                                if (Time.time - triggerTimer < clickWindow)
                                {
                                    IMLPointerClickHandler clickHandler = lastHitObject.GetComponent<IMLPointerClickHandler>();
                                    if (clickHandler != null)
                                    {
                                        clickHandler.MLOnPointerClick(eventData);
                                    }
                                }

                                //Call the select handler on the new hit object
                                if (lastSelectedObject == null)
                                {
                                    //Get, check, and call handler, update selected object only if the hit object has a select handler, indicating it is selectable
                                    IMLSelectHandler selectHandler = lastHitObject.GetComponent<IMLSelectHandler>();
                                    if (selectHandler != null)
                                    {
                                        selectHandler.MLOnSelect(eventData);
                                        lastSelectedObject = lastHitObject;
                                        //This caches the reference to the update select handler if it exists
                                        IMLUpdateSelectedHandler updateSelectedHandler = lastHitObject.GetComponent<IMLUpdateSelectedHandler>();
                                    }
                                }
                                //If there was a previously selected object, call its deselect only if the newly hit object is selectable, i.e. has a select handler
                                else if (lastHitObject != lastSelectedObject)
                                {
                                    IMLSelectHandler selectHandler = lastHitObject.GetComponent<IMLSelectHandler>();
                                    if (selectHandler != null)
                                    {
                                        IMLDeselectHandler deselectHandler = lastSelectedObject.GetComponent<IMLDeselectHandler>();
                                        if (deselectHandler != null)
                                        {
                                            deselectHandler.MLOnDeselect(eventData);
                                        }
                                        //This caches the reference to the update select handler if it exists
                                        IMLUpdateSelectedHandler updateSelectedHandler = lastHitObject.GetComponent<IMLUpdateSelectedHandler>();
                                        selectHandler.MLOnSelect(eventData);
                                        lastSelectedObject = lastHitObject;
                                    }
                                }
                            }
                        }
                        else //Trigger is not primary click
                        {
                            IMLPointer_2_UpHandler up_2_Handler = lastHitObject.GetComponent<IMLPointer_2_UpHandler>();
                            if (up_2_Handler != null)
                            {
                                up_2_Handler.MLOnPointer_2_Up(eventData);
                            }
                            //If click started and ended on the same object, and was within the time window, treat it as a click
                            if (click_2_DownObject != null && lastHitObject == click_2_DownObject && Time.time - triggerTimer < clickWindow)
                            {
                                IMLPointer_2_ClickHandler click_2_Handler = lastHitObject.GetComponent<IMLPointer_2_ClickHandler>();
                                if (click_2_Handler != null)
                                {
                                    click_2_Handler.MLOnPointer_2_Click(eventData);
                                }
                            }
                        }
                    }
                    else //Released button on empty space
                    {
                        //If trigger is the primary button(can change selection), and it also began the click on empty space, then it is deselecting
                        //Also check if there is a last selected object to now deselect
                        if (primaryClick == clickButton.trigger && clickDownObject == null && lastSelectedObject != null)
                        {
                            IMLDeselectHandler deselectHandler = lastSelectedObject.GetComponent<IMLDeselectHandler>();
                            if (deselectHandler != null)
                            {
                                deselectHandler.MLOnDeselect(eventData);
                            }
                            lastSelectedObject = null;
                        }
                    }
                }
                //This makes sure we are dragging, because we need to be able to end a drag at any time
                else if (isDragging == true)
                {
                    //If he bumper can change selections, is primary button
                    if (primaryClick == clickButton.trigger)
                    {
                        IMLEndDragHandler endDragHandler = lastHitObject.GetComponent<IMLEndDragHandler>();
                        if (endDragHandler != null)
                        {
                            endDragHandler.MLOnEndDrag(eventData);
                        }
                        //Double check that the dragged object isn't already the selected object
                        if (draggedObject != lastSelectedObject)
                        {
                            //Call select handler at the end of the drag, update selected object
                            IMLSelectHandler selectHandler = draggedObject.GetComponent<IMLSelectHandler>();
                            if (selectHandler != null)
                            {
                                selectHandler.MLOnSelect(eventData);
                                lastSelectedObject = draggedObject;
                            }
                        }
                        isDragging = false;
                        draggedObject = null;
                    }
                }
            }
        }
        #endregion //Trigger Handlers
    }
}