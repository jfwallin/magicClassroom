using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[CreateAssetMenu(fileName = "SettingsData", menuName = "ScriptableObjects/Settings", order = 1)]
public class Settings : ScriptableObject
{
    //Enums
    private enum Size { Small, Normal, Large};
    private enum Bounciness { NotVery, Normal, Very};

    [SerializeField]
    private int maxScore;
    [SerializeField]
    private Size ballSize;
    [SerializeField]
    private Bounciness ballBounciness;
    [SerializeField]
    private PhysicMaterial[] physicsMaterials = new PhysicMaterial[3];
    
    //Public Properties
    public int MaxScore { get => maxScore; }
    public Vector3 BallSize
    {
        get
        {
            switch (ballSize)
            {
                case Size.Small:
                    return new Vector3(0.05f, 0.05f, 0.05f);
                case Size.Large:
                    return new Vector3(0.1f, 0.1f, 0.1f);
                default:
                    return new Vector3(0.75f, 0.75f, 0.75f);
            }
        }
    }
    public PhysicMaterial BallBounciness
    {
        get
        {
            switch (ballBounciness)
            {
                case Bounciness.NotVery:
                    return physicsMaterials[1];
                case Bounciness.Very:
                    return physicsMaterials[3];
                default:
                    return physicsMaterials[2];
            }
        }
    }

    private void Awake()
    {
        maxScore = 10;
        ballSize = Size.Normal;
        ballBounciness = Bounciness.Normal;
    }

    #region Public Functions
    public void IncreaseBallSize()
    {
        if (ballSize == Size.Small)
            ballSize = Size.Normal;
        else if (ballSize == Size.Normal)
            ballSize = Size.Large;
    }

    public void DecreasBallSize()
    {
        if (ballSize == Size.Large)
            ballSize = Size.Normal;
        else if (ballSize == Size.Normal)
            ballSize = Size.Small;
    }

    public void IncreaseBallBounciness()
    {
        if (ballBounciness == Bounciness.NotVery)
            ballBounciness = Bounciness.Normal;
        else if (ballBounciness == Bounciness.Normal)
            ballBounciness = Bounciness.Very;
    }

    public void DecreaseBallBounciness()
    {
        if (ballBounciness == Bounciness.Very)
            ballBounciness = Bounciness.Normal;
        else if (ballBounciness == Bounciness.Normal)
            ballBounciness = Bounciness.NotVery;
    }

    public void IncreaseMaxScore()
    {
        if (maxScore < 30)
            maxScore++;
    }

    public void DecreaseMaxScore()
    {
        if (maxScore > 5)
            maxScore--;
    }

    public void ApplySettings()
    {
        Debug.Log("ApplyingSettings");
    }
    #endregion //Public Functions
}
