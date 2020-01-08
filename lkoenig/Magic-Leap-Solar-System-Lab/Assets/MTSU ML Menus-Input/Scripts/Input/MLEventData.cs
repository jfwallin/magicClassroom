using System.Collections.Generic;
using UnityEngine;


namespace MtsuMLAR
{
    //This stores important hit object data passed to object event handler functions, in case they need the info to execute
    public class MLEventData
    {
        private RaycastHit pointerRayHitInfo;
        private Transform pointerTransform;
        private GameObject currentSelectedObject;
        private GameObject currentHitObject;

        public RaycastHit PointerRayHitInfo { get => pointerRayHitInfo; set => pointerRayHitInfo = value; }
        public Transform PointerTransform { get => pointerTransform; set => pointerTransform = value; }
        public GameObject CurrentSelectedObject { get => currentSelectedObject; set => currentSelectedObject = value; }
        public GameObject CurrentHitObject { get => currentHitObject; set => currentHitObject = value; }
    }

    //This is a base class for event handlers, in case a reference to a handler of any kind is needed
    interface IMLEventHandler
    {
    }

    //These interfaces defined properties containing delegates for event handler functions, called only under certain conditions.
    //The Event System knows to send event calls to objects only if they implement these interfaces, thus if an object controller
    //Implements the IMLPointerEnterHandler, then it can recieve event calls when you first point at it, thus allowing it to change
    //color, if that is the function subscribed to the action implementing the interface.

    interface IMLPointerEnterHandler : IMLEventHandler //when pointer first colides with a colider
    {
        void MLOnPointerEnter(MLEventData eventData);
    }

    interface IMLPointerStayHandler : IMLEventHandler //Frames where the pointer is colliding
    {
        void MLOnPointerStay(MLEventData eventData);
    }

    interface IMLPointerExitHandler : IMLEventHandler //frame where the pointer leaves the collodier
    {
        void MLOnPointerExit(MLEventData eventData);
    }
    
    interface IMLSelectHandler : IMLEventHandler 
    {
        void MLOnSelect(MLEventData eventData);
    }
    //this handler depends on the object also having a select handler, making it selectable
    interface IMLDeselectHandler : IMLSelectHandler
    {
        void MLOnDeselect(MLEventData eventData);
    }
    //this one too
    interface IMLUpdateSelectedHandler : IMLSelectHandler
    {
        void MLOnUpdateSelected(MLEventData eventData);
    }
    //The drag handlers are all linked, and they all require a basic drag handler in order to start or end handle a drag
    interface IMLInitializePotentialDragHandler : IMLBeginDragHandler
    {
        void MLOnInitializePotentialDrag(MLEventData eventData);
    }
    //here
    interface IMLBeginDragHandler : IMLDragHandler  //First frame that the drag starts
    {
        void MLOnBeginDrag(MLEventData eventData);
    }

    interface IMLDragHandler : IMLEventHandler   //Every Frame the drag is happing
    {
        void MLOnDrag(MLEventData eventData);
    }
    //and here
    interface IMLEndDragHandler : IMLDragHandler   //When the drag ends
    {
        void MLOnEndDrag(MLEventData eventData);
    }

    interface IMLPointerDownHandler : IMLEventHandler  //First frame of a "click"
    {
        void MLOnPointerDown(MLEventData eventData);
    }

    interface IMLPointerUpHandler : IMLEventHandler  //Frame where it is done clicking
    {
        void MLOnPointerUp(MLEventData eventData);
    }

    interface IMLPointerClickHandler : IMLEventHandler  
    {
        void MLOnPointerClick(MLEventData eventData);
    }

    interface IMLPointer_2_DownHandler : IMLEventHandler   //pointer two is the secondary button
    {
        void MLOnPointer_2_Down(MLEventData eventData);
    }

    interface IMLPointer_2_UpHandler : IMLEventHandler
    {
        void MLOnPointer_2_Up(MLEventData eventData);
    }

    interface IMLPointer_2_ClickHandler : IMLEventHandler
    {
        void MLOnPointer_2_Click(MLEventData eventData);
    }
}
