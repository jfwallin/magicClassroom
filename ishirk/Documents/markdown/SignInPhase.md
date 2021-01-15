[Lab Overview](../LabOverview.html)
[Overview Phase >](OverviewPhase.html)
# Sign In Phase
---
#### **V0.0.1**
## Characteristics
### Description:
Users are required to input their names and other data, as required, at the very beginning of the lab. Required fields and possible options may differ.
### Expected Usage:
- Users see a panel with instructions, guiding them to enter relevant information, such as their name, m#, or email.
- Users will input text in text fields using the controller and a keyboard which appears after a text field is selected.
- The keyboard will opn by default, while not impeding the user's ability to read the panel.
- Some information will be selected from a predetermined list in dropdown boxes, like class sections and professors.
- There will be one input per page.
- Navigation buttons allow users to switch to the next panel to input more information.
- The required information can be changed.

### Constant Features:
- The Instructions will always be at the top of the panel.
- The input field will always be the same size, and below the instructions.
- Navigation buttons will be to the left and right of the instructions.
- The keyboard will always be below the input field.

### Customizable Features: 
- The instruction text can be customized within character limits.
- Dropdown options can be customized, and in vary in number.
- Field order is variable.

## Implementation
It will be implemented with Unity UI and the MLInput Module for user interaction, with buttons and text boxes. A script will record answers and remember answers if a panel is revisited.
### JSON Representation:

    {
        "PhaseType" : "SignIn",
        "NumFields" : "3",
        "Fields" : [
            {
                "FieldType" : "Text",
                "Name" : "Name",
                "Instructions" : "Please type your name."
            },{
                "FieldType" : "Text",
                "Name" : "mNumber",
                "Instructions" : "Please type your M#, excluding the M."
            }, {
                "FieldType" : "Dropdown",
                "Name" : "Section",
                "Instruction" : "Please select your class section.",
                "Options" : ["001", "002", "003"]
            }
        ]
    }

## Stretch Goals
- None currently