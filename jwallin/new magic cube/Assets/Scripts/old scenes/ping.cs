using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class ping : MonoBehaviour
{

    public AudioClip crashSoft;
    public AudioClip crashHard;

 

    private AudioSource source;
    private float lowPitchRange = .75F;
    private float highPitchRange = 1.5F;


    void Awake()
    {

        source = GetComponent<AudioSource>();
    }


    // Start is called before the first frame update
    void Start()
    {
        beep();
    }

    // Update is called once per frame
    void Update()
    {
        
    }

void beep()
{

    source.pitch = Random.Range(lowPitchRange, highPitchRange);

        source.pitch = highPitchRange;
        source.pitch = lowPitchRange;
        float hitVol = 0.5f;
        //source.PlayOneShot(crashHard, hitVol);
   source.PlayOneShot(crashSoft, hitVol);

    }


}