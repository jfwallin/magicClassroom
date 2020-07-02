[Lab Overview](../LabOverview.html)
[< Sign In Phase](SignInPhase.html)___[Custom Modules Phase >](CustomModulesPhase.html)

# Overview Phase
---
#### **V0.0.1**
## Characteristics
### Description:
Users are shown the lab title, along with other general information across multiple pages. will include lab and learning goals, skills required, and a short description of the lab as a whole, or some other introductory material.
### Expected Usage:
- Users will see multiple pages of information pertaining to the whole lab. Every page will have the name of the lab at the top.
- Pages are navigated  using "next" and "back" buttons.
- The first page will have the description of the lab as a whole.
- The next page will have learning/lab goals.
- the final page will list the lab materials required. Under the subheading "Tools", it will read "The following materials are required:"
- After clicking the "next" button on the last page, the first custom module begins.

### Constant Features:
- Lab name will be at the top of every page, centered.
- The subheading will be below the lab name, and will have predefined text:"Description", "Lab Goals", and "Tools Required."
- Information will always be below the lab name.
- "next" and "back" buttons will always be at the bottom of the page on either side of the information text.

### Customizable Features: 
- The Lab title can be customized, within a character limit.
- The information contents can customized within the character limit.

## Implementation
It will be implemented with Unity UI and the MLInput Module for user interaction, with buttons and text boxes. 
### JSON Representation:

    {
        "PhaseType" : "OverviewPhase",
        "LabTitle" : "Solar System Lab",
        "DescriptionText" : "In this lab, you will be able to see the motion of the planets around the sun.",
        "GoalsText" : ["Understand relative motion of the planets", "Visualize relative sizes of the planets"],
        "Tools" : ["Magic Leap 1", "Magic Leap Controller"]
    }

## Stretch Goals
- Add option for additional, more customizable pages that could show introductory images, or have relevant anecdotes, etc.