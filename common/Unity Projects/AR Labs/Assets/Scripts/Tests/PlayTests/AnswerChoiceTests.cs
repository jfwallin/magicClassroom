using NUnit.Framework;
using UnityEngine.TestTools;
using UnityEngine.UI;
using UnityEngine;
using System.Reflection;
using NSubstitute;

namespace Tests
{
    public class AnswerChoiceTests
    {
        #region Vairables
        //Reference to class being tested
        private AnswerChoice answer = null;
        #endregion //Variables

        #region Test Suite
        //Called before each unit test is run to ensure a clean run
        [SetUp]
        public void SetUp()
        {
            //Initialize the class to test
            answer = new AnswerChoice();
        }

        //Called after each unit test is run to clean up residual state
        [TearDown]
        public void TearDown()
        {
            //Reset the class to test
            answer = null;
        }

        //Sample Test
        [Test]
        public void Function_ExpectedBehavior_WhenInputConditions()
        {
            //Arrange (Set up any test specific values or inputs)
            //Action  (Call the function)
            //Assert  (Assert that the expected outcome is true)
        }

        [Test]
        public void Initialize_SetsAnswerIndex_WhenCalled(
            [Range(0,4,2)] int answerIndex)
        {
            //Arrange
            QuestionManager qm;
            ToggleGroup tg;
            //Action
            answer.Initialize(answerIndex, qm, tg);
            //Assert
            
        }
        #endregion //Test Suite
    }
}
