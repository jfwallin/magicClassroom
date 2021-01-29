using UnityEngine;
using UnityEngine.UI;

public class AnswerChoice : MonoBehaviour
{
    #region Variables
    private int answerId = -1;          //Unique answer choice id, made from its index in the array of possible answers.
    private IQuestionManager qm = null; //Reference to pass id to when selected/unselected.
    #endregion //Variables

    #region Public Functions
    /// <summary>
    /// Sets class variables and toggle group
    /// </summary>
    /// <param name="answerIndex"></param>
    /// <param name="passedQM"></param>
    /// <param name="tg"></param>
    /// <returns></returns>
    public void Initialize(int answerIndex, IQuestionManager passedQM, ToggleGroup tg)
    {
        answerId = answerIndex;
        qm = passedQM;
        GetComponent<Toggle>().group = tg;
        if(answerIndex == -1 || qm == null || GetComponent<Toggle>()?.group == null)
        {
            Debug.LogWarning("Could some values on an answer choice were not set correctly");
        }
    }

    public void OnSelectChange(bool selected)
    {
        if(selected)
        {
            qm.OnAnswerSelected(answerId);
        }
        else //not selected
        {
            qm.OnAnswerDeselected(answerId);
        }
    }
    #endregion //Public Functions
}
