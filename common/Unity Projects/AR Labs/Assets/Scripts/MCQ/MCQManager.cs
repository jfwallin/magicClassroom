using System;
using UnityEngine.UI;
using UnityEngine;
using UnityEngine.Assertions;

namespace Tests
{
    public class MCQManager : MonoBehaviour
    {
        #region Variables
        //Media object
        [SerializeField]
        private GameObject mediaGO = null;                //GameObject with all media playback components attached
        [SerializeField]
        private MediaPlaybackManager mpManager = null;    //Script responsible for displaying media, exopsing UI callbacks

        //Question object
        [SerializeField]
        private GameObject questionGO = null;             //GameObject that contains the question text, answer options, submit button, and continue button
        [SerializeField]
        private QuestionManager questionManager = null;   //Script responsible for arranging question objects and reporting what answer(s) was(were) selected
        [SerializeField]
        private GameObject answerPrefab = null;           //Prefab for an answer option

        #endregion //Variables


        public void Initialize()
        {
            //Null reference checks
            if(mediaGO == null)
            {
                LogU.RNFI("mediaGO");
                foreach(Transform t in transform)
                {
                    if(t.name == "QuestionMediaPlayer")
                    {
                        mediaGO = t.gameObject;
                        break;
                    }
                }
                Assert.IsNotNull(mediaGO);
            }
            if(mpManager == null)
            {
                LogU.RNFI("mpManager");
                mpManager = mediaGO.GetComponent<MediaPlaybackManager>();
                Assert.IsNotNull(mpManager);
            }
            if(questionGO == null)
            {
                LogU.RNFI("questionGO");
                foreach(Transform t in transform)
                {
                    if (t.name == "Question")
                    {
                        questionGO = t.gameObject;
                        break;
                    }
                }
                Assert.IsNotNull(questionGO);
            }
            if(questionManager == null)
            {
                LogU.RNFI("questionManager");
                questionManager = questionGO.GetComponent<QuestionManager>();
                Assert.IsNotNull(questionManager);
            }
            if(answerPrefab == null)
            {
                LogU.RNFI("answerPrefab");
                answerPrefab = (GameObject)Resources.Load("Answer");
                Assert.IsNotNull(answerPrefab);
            }
        }
    }
}