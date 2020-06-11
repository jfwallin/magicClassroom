using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System;

public enum Size { Small, Normal, Large };
public enum Bounciness { NotVery, Normal, Very };

[CreateAssetMenu(fileName = "SettingsData", menuName = "ScriptableObjects/Settings", order = 1)]
public class Settings : ScriptableObject
{
    public event Action<string, string> OnSettingsChanged;

    [SerializeField]
    private int maxScore;
    [SerializeField]
    private Size ballSize;
    [SerializeField]
    private Bounciness ballBounciness;
    [SerializeField]
    private PhysicMaterial[] physicsMaterials = new PhysicMaterial[3];

    #region Public Properties
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
                    return physicsMaterials[0];
                case Bounciness.Very:
                    return physicsMaterials[2];
                default:
                    return physicsMaterials[1];
            }
        }
    }
    #endregion //Public Properties

    private void Awake()
    {
        //Initializations
        maxScore = 10;
        ballSize = Size.Normal;
        ballBounciness = Bounciness.Normal;
    }

    #region Public Functions
    //All of these functions are called from settings menu button events

    public void IncreaseBallSize()
    {
        //if (ballSize == Size.Small)
        //    ballSize = Size.Normal;
        //else if (ballSize == Size.Normal)
        //    ballSize = Size.Large;
        if (ballSize != Size.Large)
            ballSize++;
        OnSettingsChanged("BallSize", ballSize.ToString());
    }

    public void DecreasBallSize()
    {
        //if (ballSize == Size.Large)
        //    ballSize = Size.Normal;
        //else if (ballSize == Size.Normal)
        //    ballSize = Size.Small;
        if (ballSize != Size.Small)
            ballSize--;
        OnSettingsChanged("BallSize", ballSize.ToString());
    }

    public void IncreaseBallBounciness()
    {
        //if (ballBounciness == Bounciness.NotVery)
        //    ballBounciness = Bounciness.Normal;
        //else if (ballBounciness == Bounciness.Normal)
        //    ballBounciness = Bounciness.Very;
        if (ballBounciness != Bounciness.Very)
            ballBounciness++;
        OnSettingsChanged("BallBounciness", ballBounciness.ToString());
    }

    public void DecreaseBallBounciness()
    {
        //if (ballBounciness == Bounciness.Very)
        //    ballBounciness = Bounciness.Normal;
        //else if (ballBounciness == Bounciness.Normal)
        //    ballBounciness = Bounciness.NotVery;
        if (ballBounciness != Bounciness.NotVery)
            ballBounciness--;
        OnSettingsChanged("BallBounciness", ballBounciness.ToString());
    }

    public void IncreaseMaxScore()
    {
        if (maxScore < 20)
            maxScore++;
        OnSettingsChanged("MaxScore", maxScore.ToString());
    }

    public void DecreaseMaxScore()
    {
        if (maxScore > 5)
            maxScore--;
        OnSettingsChanged("MaxScore", maxScore.ToString());
    }
    #endregion //Public Functions
}
