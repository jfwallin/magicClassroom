using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class FireBallScript : MonoBehaviour
{
    #region Public Variables
    [Header("Projectile Settings")]
    [Range(1,100)]
    public int damageAmount = 11;
    [Range(1f,10f)]
    public float velocity = 4.5f;
    public GameObject explosionPrefab;

    [Header("Audio Variables")]
    public AudioClip[] firingSounds;
    public AudioSource firingAudiosource;
    public AudioClip[] explosionSounds;
    #endregion

    private void Awake()
    {
        //Check references
        firingAudiosource = GetComponent<AudioSource>();
        if(firingAudiosource == null)
        {
            Debug.LogWarning("no audio source component on projectile prefab, disabling");
            enabled = false;
        }
        if (explosionPrefab == null)
        {
            Debug.LogWarning("No explosion prefab assigned to fireball, disabling");
            enabled = false;
        }
    }

    private void Start()
    {
        //setup and play firing sound
        int randomClip = Random.Range(0, firingSounds.Length);
        firingAudiosource.clip = firingSounds[randomClip];
        firingAudiosource.pitch = Random.Range(0.8f, 1.2f);
        firingAudiosource.Play();
    }

    private void OnCollisionEnter(Collision collision)
    {
        //Spawn and orient explosion
        GameObject explosion = GameObject.Instantiate(explosionPrefab, collision.GetContact(0).point, Quaternion.LookRotation(collision.GetContact(0).normal));
        //Setup and play explosion sound
        AudioSource explosionAudioSource = explosion.GetComponent<AudioSource>();
        int randomClip = Random.Range(0, explosionSounds.Length);
        explosionAudioSource.clip = explosionSounds[randomClip];
        explosionAudioSource.pitch = Random.Range(0.8f, 1.2f);
        explosionAudioSource.Play();
        //Remove explosion after its particle system completes
        Destroy(explosion, 2f);
        //apply damage if able
        if (collision.transform.name == "Player Body")
        {
            collision.transform.GetComponent<Player>().ApplyDamage(damageAmount);
        }
        //remove projectile
        Destroy(gameObject);
    }
}
