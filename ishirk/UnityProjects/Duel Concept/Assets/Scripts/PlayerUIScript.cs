using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

/// <summary>
/// Manages HUD,shows HP,MP levels, shows text on death
/// </summary>
public class PlayerUIScript : MonoBehaviour
{
    #region Public Variables
    public Player playerScript;   //used to get current HP, MP levels
    public Image healthBarFill;   //adjusted to display HP, MP levels
    public Image magicBarFill;
    public Text deathText;        //Displayed once the player dies
    #endregion

    #region Unity Methods
    void Start()
    {
        //Make sure playerScript is assigned
        if(playerScript == null)
        {
            Debug.LogWarning("no playerInfo object assigned to PlayerUIScript, disabling");
            enabled = false;
        }
        playerScript.OnPlayerDeath += EnableDeathText;

        //Initial values
        deathText.enabled = false;
        healthBarFill.fillAmount = 1;
        magicBarFill.fillAmount = 1;
    }

    private void OnDestroy()
    {
        playerScript.OnPlayerDeath -= EnableDeathText;
    }

    void Update()
    {
        //update HUD with current MP, HP values
        healthBarFill.fillAmount = playerScript.HealthPoints / playerScript.maxHP;
        magicBarFill.fillAmount = playerScript.MagicPoints / playerScript.maxMP;
    }
    #endregion

    public void EnableDeathText()
    {
        deathText.enabled = true;
    }
}
