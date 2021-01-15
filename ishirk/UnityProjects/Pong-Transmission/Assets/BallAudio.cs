using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Assertions;

public class BallAudio : MonoBehaviour
{
    //Audio variables
    public AudioClip ballHitClip = null;
    public AudioClip ballResetClip = null;
    private AudioSource ballAudioSource = null;

    private void Awake()
    {
        ballAudioSource = GetComponent<AudioSource>();
        Assert.IsNotNull(ballAudioSource, "ballAudioSource could not be found on ball");
        Assert.IsNotNull(ballHitClip, "ballHitClip not assigned");
        Assert.IsNotNull(ballResetClip, "ballResetClip not assigned");
    }

    private void Start()
    {
        GetComponent<Ball>().onBallReset += PlayBallResetSound;
    }

    private void OnDisable()
    {
        GetComponent<Ball>().onBallReset -= PlayBallResetSound;
    }

    private void OnCollisionEnter(Collision collision)
    {
        ballAudioSource.clip = ballHitClip;
        ballAudioSource.Play();
    }

    private void PlayBallResetSound()
    {
        ballAudioSource.clip = ballResetClip;
        ballAudioSource.Play();
    }
}
