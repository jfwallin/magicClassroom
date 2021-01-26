using NUnit.Framework;
using UnityEngine;
using UnityEngine.TestTools;
using System.Reflection;

namespace Tests
{
    public class MCQManagerTests
    {
        #region Variables
        private MCQManager manager = null;         //The class to test
        #endregion //Variables

        #region Test Suite
        //Called before each unit test is run to ensure a clean run
        [SetUp]
        public void Setup()
        {
            manager = new MCQManager();
        }

        //Called after each unit test is run to clean up
        [TearDown]
        public void Teardown()
        {
            manager = null;
        }

        [Test]
        public void Initialize_SetsAllReferences_WhenCalled()
        {
            //Arrange
            FieldInfo videoComponentInfo = typeof(UnityEngine.Video.VideoPlayer).GetField(BindingFlags.NonPublic | BindingFlags.Instance);
            //Action
            manager.Initialize();
            //Assert
        }
        #endregion //Test Suite
    }
}
