[Lab Overview](../LabOverview.html)
[< Custom Modules Phase](CustomModulesPhase.html)

# Conclusion Phase

***

#### **V0.0.1**

## Characteristics

### Description:

Users are shown some closing information and remarks and a final susbmission form before finishing the lab and returning to the main application.

### Expected Usage:

* Users will see a page with closing information and remarks.
* Optionally, users may see a honor code statement to which they must agree before submission.
* Users see a final submission dialogue where they will either click "submit" or "back".

### Constant Features:

* Subheadings on the pages cannot be changed. ie "Conclusion", and "Submit Answers."
* The Submission page will always have the same text.
* "next" and "back" buttons will always be at the bottom of the page on either side of the panel.

### Customizable Features:

* The text of the conclusion page can be modified, within a character limit.
* The honors statement page is optional, and can be disabled.
* Honors statement text is customizable, within some upper limit.

## Implementation

Unity UI and the MLInput module will be used to construct the phase with panels, text boxes, and buttons.

### JSON Representation:

```
{
    "PhaseType" : "ConclusionPhase",
    "ConclusionText" : "Now you're done! You've seen all the planets.",
    "CheckHonorStatement" : "True",
    "HonorStatementText" : "I am submitting my work only, and did not cheat."
}
```

## Stretch Goals

* None currently
