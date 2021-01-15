[Lab Overview](../LabOverview.html)
[Custom Modules Phase](CustomModulesPhase.html)
# Multiple Choice Module
---
#### **V0.0.1**
\**Current pattern approaches this multiple choice module as a collection of one or more questions. It may be better to redesign it as one question on one panel, that way the lab assessment as a whole can be made up of differing types of questions that don't necessarily have to be grouped together. The module code would not change much except it would only allow for one question panel, that could lead into whatever next question you wanted, even another multiple choice.*
\*\**There is no way to track overall progress through multiple questions, and therefore the exact question number cannot be shown unless more information is added to the module, or some parent object manages all assessment modules.*
## Characteristics
### Expected Usage:
- User is presented with a question and multiple answers.
- User will select one and then move to the next question or complete the module.
- There will be a visual indication of a choice being highlighted.
- There will be a visual indication of a choice being selected.
- Can have more than one question by having multiple panels that the user can navigate through with "next" and " back" buttons.
- "next" and "back" arrows will have a visual indication of being highlighted
- Upon being returned to, the module will remember what choice was selected previously, and will show it as the currently selected choice.
### Constant Features:
- Position of the Question will always be at the top of the panel.
- Answers will always be below the question, in two columns.
- Next and Back arrows will always be at the bottom of the panel.
- The Panel will always have the same size.
### Customizable Features:
- The Information and Question can be set to whatever you want, within some character limit.
- The number of answers can vary from 2 to 6.
- Each answer choice will have either text or an image as its body, both of which would be totally customizable, again within a size limit.
## Implementation
It will be implemented using Unity's built in UI. Using buttons and the MLInput module for the interface to the user, and a script to record answer selection, submission, and remembering upon the question being returned to. Panel object will be constructed from the data given in the module's JSON representation, and the multiple choice template panel.
### JSON Representation:
**Text Answers**

    {
        "ModuleType" : "mc",
        "QuestionText" : "What is Love?",
        "MultiSelect" : "NotImplemented",
        "ImageAnswers" : "false",
        "NumAnswers" : "3",
        "Answers" : ["Baby don't hurt me", "Don't hurt me", "No more"]
    }
**Image Answers**

    {
        "ModuleType" : "mc",
        "QuestionText" : "Select the image that has a crosswalk in it",
        "MultiSelect" : "NotImplemented",
        "ImageAnswers" : "true",
        "NumAnswers" : "2",
        "Answers" : ["Resources/images/dirtRoad.png", "Resources/images/crosswalk.png"]
    }
## Stretch Goals
- Having 3D objects as the answer choices, with a smaller panel for displaying the question and navigation.
- Having the ability to put more choices.
- The option to make it checkboxes, ie. "check all that apply."
- The option to have a seperate page for long questions prefacing the answer page.
- The option to have answer boxes expand when hovered over in case answer text is too large to fit the panel.
- The option to show a reference image as part of the instructions, would also work as part of the seperate panel mentioned above.