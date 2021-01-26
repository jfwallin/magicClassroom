using NUnit.Framework;
using UnityEngine;
using UnityEngine.Video;
using UnityEngine.TestTools;
using System.Reflection;

namespace Tests
{
    public class MediaPlaybackManagerTests
    {
        #region Vairables
        //Reference to class being tested
        GameObject mediaPlayerGO = null;
        MediaPlaybackManager mpManager = null;

        //References to component dependencies
        VideoPlayer videoPlayer = null;
        AudioSource audioSource = null;
        MeshRenderer meshRenderer = null;
        #endregion //Variables

        #region Test Suite
        //Called before each unit test is run to ensure a clean run
        [SetUp]
        public void SetUp()
        {
            //Setup the class to test
            mediaPlayerGO = new GameObject();
            mpManager = mediaPlayerGO.AddComponent<MediaPlaybackManager>();

            //Setup the dependencies
            videoPlayer = mediaPlayerGO.AddComponent<VideoPlayer>();
            audioSource = mediaPlayerGO.AddComponent<AudioSource>();
            meshRenderer = mediaPlayerGO.AddComponent<MeshRenderer>();
        }

        //Called after each unit test is run to clean up residual state
        [TearDown]
        public void TearDown()
        {
            //Reset the dependencies
            videoPlayer = null;
            audioSource = null;
            meshRenderer = null;

            //Reset the class being tested
            mpManager = null;
            GameObject.Destroy(mediaPlayerGO);

            //Reset logging
            if (LogAssert.ignoreFailingMessages)
                LogAssert.ignoreFailingMessages = false;
        }

        [Test,Sequential]
        public void Initialize_FindsRef_RefNotSetInInspector(
            [NUnit.Framework.Values("videoPlayer", "audioSource", "meshRenderer")] string refName,
            [NUnit.Framework.Range(0, 2)] int caseIndex)
        {
            //Arrange
            FieldInfo refCheckFieldInfo = mpManager.GetType().GetField("refName", BindingFlags.NonPublic | BindingFlags.Instance);
            LogAssert.ignoreFailingMessages = true;
            //Action
            mpManager.Initialize();
            //Assert
            switch(caseIndex)
            {
                case 0:
                    Assert.AreEqual(videoPlayer, refCheckFieldInfo.GetValue(mpManager));
                    break;
                case 1:
                    Assert.AreEqual(audioSource, refCheckFieldInfo.GetValue(mpManager));
                    break;
                case 2:
                    Assert.AreEqual(meshRenderer, refCheckFieldInfo.GetValue(mpManager));
                    break;
            }
        }

        [Test]
        public void DisplayMedia_EnablesOrDisablesProperComopnents_DependingOnMediaType(
            [NUnit.Framework.Values(MediaPlaybackManager.MediaType.Video,
                                    MediaPlaybackManager.MediaType.Audio,
                                    MediaPlaybackManager.MediaType.Image,
                                    MediaPlaybackManager.MediaType.ImageAndAudio,
                                    MediaPlaybackManager.MediaType.None)] MediaPlaybackManager.MediaType mediaType,
            [NUnit.Framework.Values(true, false)] bool componentsStartEnabled)
        {
            //Arrange
            videoPlayer.enabled = componentsStartEnabled;
            audioSource.enabled = componentsStartEnabled;
            meshRenderer.enabled = componentsStartEnabled;
            VideoClip vClip = null;
            AudioClip aClip = null;
            Texture2D image = null;
            //Action
            switch(mediaType)
            {
                case MediaPlaybackManager.MediaType.Video:
                    mpManager.DisplayMedia(mediaType, vClip);
                    break;
                case MediaPlaybackManager.MediaType.Audio:
                    mpManager.DisplayMedia(mediaType, aClip);
                    break;
                case MediaPlaybackManager.MediaType.Image:
                    mpManager.DisplayMedia(mediaType, image);
                    break;
                case MediaPlaybackManager.MediaType.ImageAndAudio:
                    mpManager.DisplayMedia(mediaType, image, aClip);
                    break;
            }
        }
        #endregion //Test Suite
    }
}
