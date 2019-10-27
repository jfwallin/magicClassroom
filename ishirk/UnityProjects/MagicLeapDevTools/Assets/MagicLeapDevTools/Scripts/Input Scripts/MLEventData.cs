using System.Collections.Generic;
using UnityEngine;


namespace MtsuMLAR
{
    /// <summary>
    /// Stores Event system information, including pointer object transform,
    /// the current selected object, and the current hit object. Sent to event handlers
    /// </summary>
    public class MLEventData
    {
        private Transform pointerTransform;
        private GameObject currentSelectedObject;
        private GameObject currentHitObject;

        public Transform PointerTransform { get => pointerTransform; set => pointerTransform = value; }
        public GameObject CurrentSelectedObject { get => currentSelectedObject; set => currentSelectedObject = value; }
        public GameObject CurrentHitObject { get => currentHitObject; set => currentHitObject = value; }
    }

    /// <summary>
    /// A base event handler interface all other event interfaces inherit from
    /// in case a test for the existence of any handler is needed
    /// </summary>
    interface IMLEventHandler
    {
    }

    ///These interfaces define function names for event handling functions. The event system only knows
    ///to send an object an event callback if it implements the proper interface.
    
    /// <summary>
    /// Implement to recieve a callback for the first frame the pointer hits the object
    /// </summary>
    interface IMLPointerEnterHandler : IMLEventHandler
    {
        void MLOnPointerEnter(MLEventData eventData);
    }

    /// <summary>
    /// Implement to recieve a callback every frame the pointer hovers over the object
    /// </summary>
    interface IMLPointerStayHandler : IMLEventHandler
    {
        void MLOnPointerStay(MLEventData eventData);
    }

    /// <summary>
    /// Implement to recieve a callback the first frame the pointer leaves the previously hit object
    /// </summary>
    interface IMLPointerExitHandler : IMLEventHandler
    {
        void MLOnPointerExit(MLEventData eventData);
    }
    
    /// <summary>
    /// Implement this interface to recieve a callback on the first frame an object is considered selected
    /// by the event system. Implementing this interface is what makes an object selectable
    /// </summary>
    interface IMLSelectHandler : IMLEventHandler
    {
        void MLOnSelect(MLEventData eventData);
    }

    /// <summary>
    /// Implement this interface to recieve a callback on the first frame an object is no longer 
    /// considered by the event system. This inherits from IMLSelectHandler to ensure that the
    /// object is selectable.
    /// </summary>
    interface IMLDeselectHandler : IMLSelectHandler
    {
        void MLOnDeselect(MLEventData eventData);
    }

    /// <summary>
    /// Implement this interface to recieve a callback every fram the object is considered selected
    /// by the event system. This inherits from IMLSelectHandler to ensure that the object is
    /// selectable.
    /// </summary>
    interface IMLUpdateSelectedHandler : IMLSelectHandler
    {
        void MLOnUpdateSelected(MLEventData eventData);
    }

    /// <summary>
    /// ***Not Supported Yet*** Implement this interface to recieve a callback the first frame
    /// a draggable object is clicked to start checking for drag initiation conditions or to
    /// set initial values. This inherits from IMLBeginDragHandler to make sure the object
    /// can begin a drag.
    /// </summary>
    interface IMLInitializePotentialDragHandler : IMLBeginDragHandler
    {
        void MLOnInitializePotentialDrag(MLEventData eventData);
    }

    /// <summary>
    /// Implement this interface to recieve a callback on the first frame a draggable object
    /// is considered as being dragged by the event system. This inherits from IMLDragHandler
    /// to ensure that the object can be dragged.
    /// </summary>
    interface IMLBeginDragHandler : IMLDragHandler
    {
        void MLOnBeginDrag(MLEventData eventData);
    }

    /// <summary>
    /// Implement this interface to recieve a callback every frame an object is considered
    /// as being dragged by the event system. Implementing this interface makes the object
    /// draggable.
    /// </summary>
    interface IMLDragHandler : IMLEventHandler
    {
        void MLOnDrag(MLEventData eventData);
    }
    
    /// <summary>
    /// Implement this interface to recieve a callback on the first frame an object is no
    /// longer considered as being dragged by the event system. This inherits from 
    /// IMLDragHandler to ensure that the object can be dragged.
    /// </summary>
    interface IMLEndDragHandler : IMLDragHandler
    {
        void MLOnEndDrag(MLEventData eventData);
    }

    /// <summary>
    /// Implement this interface to recieve a callback on the first frame the primary
    /// click is pressed while the pointer is hitting an object
    /// </summary>
    interface IMLPointerDownHandler : IMLEventHandler
    {
        void MLOnPointerDown(MLEventData eventData);
    }

    /// <summary>
    /// Implement this interface to recieve a callback on the first frame the primary
    /// click is released while the pointer is hitting an object
    /// </summary>
    interface IMLPointerUpHandler : IMLEventHandler
    {
        void MLOnPointerUp(MLEventData eventData);
    }

    /// <summary>
    /// Implement this interface to recieve a callback on the frame the primary click
    /// is released quickly enough that it is considered a click by the event system
    /// </summary>
    interface IMLPointerClickHandler : IMLEventHandler
    {
        void MLOnPointerClick(MLEventData eventData);
    }

    /// <summary>
    /// Implement this interface to recieve a callback on the first frame the secondary
    /// click is pressed while the pointer is hitting an object
    /// </summary>
    interface IMLPointer_2_DownHandler : IMLEventHandler
    {
        void MLOnPointer_2_Down(MLEventData eventData);
    }

    /// <summary>
    /// Implement this interface to recieve a callback on the first frame the secondary
    /// click is released while the pointer is hitting an object
    /// </summary>
    interface IMLPointer_2_UpHandler : IMLEventHandler
    {
        void MLOnPointer_2_Up(MLEventData eventData);
    }

    /// <summary>
    /// Implement this interface to recieve a callback on the frame the secondary click
    /// is released quickly enough that it is considered a click by the event system
    /// </summary>
    interface IMLPointer_2_ClickHandler : IMLEventHandler
    {
        void MLOnPointer_2_Click(MLEventData eventData);
    }
}
