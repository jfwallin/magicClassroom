using NUnit.Framework;
using UnityEngine.TestTools;
using UnityEngine.UI;
using UnityEngine;
using System.Reflection;
using System.Collections;
using System.Collections.Generic;
using NSubstitute;

namespace Tests
{
    public class AnswerChoiceTests
    {
        #region Vairables
        private GameObject answerGameObject = null; //GameObject the class is attached to
        private AnswerChoice answer = null;         //Reference to the class to be tested
        #endregion //Variables

        #region Test Suite
        //Called before each unit test is run to ensure a clean run
        [SetUp]
        public void SetUp()
        {
            //Setup Class to test
            answerGameObject = new GameObject();
            answer = answerGameObject.AddComponent<AnswerChoice>();
        }

        //Called after each unit test is run to clean up residual state
        [TearDown]
        public void TearDown()
        {
            //Reset the class
            answer = null;
            GameObject.Destroy(answerGameObject);
        }

        [Test]
        public void Initialize_SetsAnswerIndex_WhenPassedInt(
            [NUnit.Framework.Range(0,4,2)] int answerIndex)
        {
            //Arrange
            FieldInfo answerIDFieldInfo = answer.GetType().GetField("answerID", BindingFlags.NonPublic | BindingFlags.Instance);
            //Action
            answer.Initialize(answerIndex, null, null);
            //Assert
            Assert.AreEqual(answerIndex, answerIDFieldInfo.GetValue(answer));
        }

        [Test]
        public void Initialize_FindsQuestionManager_WhenReferenceNotPassed()
        {
            //Arrange
            GameObject qmGameObject = new GameObject();
            QuestionManager questionManager = qmGameObject.AddComponent<QuestionManager>();
            GameObject middleGameObject = new GameObject();
            middleGameObject.transform.SetParent(qmGameObject.transform);
            answerGameObject.transform.SetParent(middleGameObject.transform);
            //Reflection to access private field
            FieldInfo questionManagerFieldInfo = answer.GetType().GetField("questionManager", BindingFlags.NonPublic | BindingFlags.Instance);
            //Action
            answer.Initialize(0, null, null);
            //Assert
            Assert.AreEqual(questionManager, questionManagerFieldInfo.GetValue(answer));
        }

        [Test]
        public void Initialize_SetsToggleGroup_WhenPassedToggleGroup()
        {
            //Arrange
            answerGameObject.AddComponent<Toggle>().group = null;
            ToggleGroup newToggleGroup = answerGameObject.AddComponent<ToggleGroup>();
            //Action
            answer.Initialize(0, null, newToggleGroup);
            //Assert
            Assert.AreEqual(newToggleGroup, answerGameObject.GetComponent<Toggle>().group);
        }

        [Test]
        public void OnSelectchange_CallsAnswerSelectedWithCorrectID_WhenToggleSelected(
            [NUnit.Framework.Range(0, 4, 2)] int answerIndex)
        {
            //Arrange
            bool selected = true;
            IQuestionManager qmMock = Substitute.For<IQuestionManager>();
            answer.Initialize(answerIndex, qmMock, null);
            //Action
            qmMock.ClearReceivedCalls();
            answer.OnSelectChange(selected);
            //Assert
            Assert.IsTrue(qmMock.Received().OnAnswerSelected(answerIndex));
        }

        [Test]
        public void OnSelectChange_CallsAnswerDeselectedWithCorrectID_WhenToggleDeselected(
            [NUnit.Framework.Range(0, 4, 2)] int answerIndex)
        {
            //Arrange
            bool selected = false;
            IQuestionManager qmMock = Substitute.For<IQuestionManager>();
            answer.Initialize(answerIndex, qmMock, null);
            //Action
            qmMock.ClearReceivedCalls();
            answer.OnSelectChange(selected);
            //Assert
            Assert.IsTrue(qmMock.Received().OnAnswerDeselected(answerIndex));
        }
        #endregion //Test Suite
    }
}
