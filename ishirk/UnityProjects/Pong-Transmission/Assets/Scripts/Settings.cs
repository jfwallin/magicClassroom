using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System;

public enum Size { Small, Normal, Large };
public enum Bounciness { NotVery, Normal, Very };

[CreateAssetMenu(fileName = "SettingsData", menuName = "ScriptableObjects/Settings", order = 1)]
public class Settings : ScriptableObject
{
    public event Action<string> OnSettingsChanged;

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
    public Size BallSizeEnum { get => ballSize; }
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
    public Bounciness BallBouncinessEnum { get => ballBounciness; }
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
        OnSettingsChanged("BallSize");
    }

    public void DecreasBallSize()
    {
        //if (ballSize == Size.Large)
        //    ballSize = Size.Normal;
        //else if (ballSize == Size.Normal)
        //    ballSize = Size.Small;
        if (ballSize != Size.Small)
            ballSize--;
        OnSettingsChanged("BallSize");
    }

    public void IncreaseBallBounciness()
    {
        //if (ballBounciness == Bounciness.NotVery)
        //    ballBounciness = Bounciness.Normal;
        //else if (ballBounciness == Bounciness.Normal)
        //    ballBounciness = Bounciness.Very;
        if (ballBounciness != Bounciness.Very)
            ballBounciness++;
        OnSettingsChanged("BallBounciness");
    }

    public void DecreaseBallBounciness()
    {
        //if (ballBounciness == Bounciness.Very)
        //    ballBounciness = Bounciness.Normal;
        //else if (ballBounciness == Bounciness.Normal)
        //    ballBounciness = Bounciness.NotVery;
        if (ballBounciness != Bounciness.NotVery)
            ballBounciness--;
        OnSettingsChanged("BallBounciness");
    }

    public void IncreaseMaxScore()
    {
        if (maxScore < 30)
            maxScore++;
        OnSettingsChanged("MaxScore");
    }

    public void DecreaseMaxScore()
    {
        if (maxScore > 5)
            maxScore--;
        OnSettingsChanged("MaxScore");
    }
    #endregion //Public Functions
}
