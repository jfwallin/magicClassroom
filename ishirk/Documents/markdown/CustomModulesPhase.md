[Lab Overview](../LabOverview.html)
[< Overview Phase](OverviewPhase.html)___[Conclusion Phase >](ConclusionPhase.html)
# Custom Modules Phase
---
#### **V0.0.1**
## Characteristics
### Description:
This phase is made up of many smaller modules that are externally defined and then constructed by the application once the lab loads. The Phase will manage the flow into, between, and out of the modules by linking them to each other, and tracking the total number of modules.
### Expected Usage:
- Users will see any number of any type of custom module after completing the overview phase.
- After completing a custom module, the user can click a "next" button to go to the next module.
- If the user wants, they may click a "back" button to return to the previous module.
- Users will see a progress bar indicating how far they are in the lab.

### Constant Features:
- The Custom Modules phase will always generate some data to return to the server upon submission in the conclusion phase.

### Customizable Features: 
- The number of modules  can be customized, within some high upper bound.
- The type of modules can be customized, without grouping of like kinds. 
- Content of modules can be customized, within limits.

### Available Modules:
##### Informative Modules
- [Information]()
- [Interaction]()
##### Assessment Modules
- [Multiple Choice]()
- [Sorting]()
- [Classify]()
- [Interaction Assessment]()

## Implementation
A script will read and construct the module JSON objects, and connect them via button navigation.A script will also collect answeres from the assessment modules to submit to the server. Another script will also track overall progress through the phase, and update a progress bar.
### JSON Representation:

    {
        "PhaseType" : "CustomModulePhase",
        "NumModules" : "n",
        "Modules" : [
            {module object},
            {module object},
            etc...
        ]
    }

## Stretch Goals
- None currently