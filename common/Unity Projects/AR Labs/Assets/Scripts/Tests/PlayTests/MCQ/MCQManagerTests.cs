using NUnit.Framework;
using UnityEngine;
using UnityEngine.TestTools;
using System.Reflection;

namespace Tests
{
    public class MCQManagerTests
    {
        #region Vairables
        //Reference to class being tested
        GameObject managerGameObject = null;
        MCQManager mcqManager = null;

        //Media object dependency variables
        GameObject mediaGO = null;
        MediaPlaybackManager mpManager = null;

        //Question object dependency variables
        GameObject questionGO = null;
        QuestionManager qManager = null;
        #endregion //Variables

        #region Test Suite
        //Called before each unit test is run to ensure a clean run
        [SetUp]
        public void SetUp()
        {
            //Initialize the class to test
            managerGameObject = new GameObject();
            mcqManager = managerGameObject.AddComponent<MCQManager>();

            //Setup media object dependency
            mediaGO = new GameObject();
            mediaGO.name = "QuestionMediaPlayer";
            mpManager = mediaGO.AddComponent<MediaPlaybackManager>();
            mediaGO.transform.SetParent(managerGameObject.transform);

            //Setup question object dependency
            questionGO = new GameObject();
            questionGO.name = "Question";
            qManager = questionGO.AddComponent<QuestionManager>();
            questionGO.transform.SetParent(managerGameObject.transform);
        }

        //Called after each unit test is run to clean up residual state
        [TearDown]
        public void TearDown()
        {
            //Reset media object dependency
            mpManager = null;
            GameObject.Destroy(mediaGO);

            //Reset question object dependency
            qManager = null;
            GameObject.Destroy(questionGO);

            //Reset the class to test
            mcqManager = null;
            GameObject.Destroy(managerGameObject);

            //Reset logging
            if (LogAssert.ignoreFailingMessages)
                LogAssert.ignoreFailingMessages = false;
        }

        [Test]
        public void Initialize_FindsRef_RefNotSetInInspector(
            [NUnit.Framework.Values("mediaGO", "mpManager", "questionGO", "qManager")] string fieldName,
        {
            //Arrange
            FieldInfo refCheckFieldInfo = mcqManager.GetType().GetField(fieldName, BindingFlags.NonPublic | BindingFlags.Instance);
            LogAssert.ignoreFailingMessages = true;
            //Action
            mcqManager.Initialize();
            //Assert
            switch(fieldName)
            {
                case "mediaGO":
                    Assert.AreEqual(mediaGO, refCheckFieldInfo.GetValue(mcqManager));
                    break;
                case "mpManager":
                    Assert.AreEqual(mpManager, refCheckFieldInfo.GetValue(mcqManager));
                    break;
                case "questionGO":
                    Assert.AreEqual(questionGO, refCheckFieldInfo.GetValue(mcqManager));
                    break;
                case "qManager":
                    Assert.AreEqual(qManager, refCheckFieldInfo.GetValue(mcqManager));
                    break;
            }
        }
        #endregion //Test Suite
    }
}
