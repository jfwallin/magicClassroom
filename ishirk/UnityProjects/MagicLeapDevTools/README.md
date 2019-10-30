# MTSUMagicLeapDevTools

## Description
Magic Leap Dev Tools is a package that has some basic reusable tools to make development move quicker. It currently has:  
- A Magic Leap controller prefab with basic input managing
- An event system that handles sending events to specific objects
- UI Prefabs and scripts compatible with the event system

And the hope is to add more. 

### Requirements
This package was developed in Unity 2019.2, with MLSDK v 0.22.0, with Magic Leap OS version 0.97
To make sure your development progress is smooth, please make sure your versions are minimally at this level.

## Installation
To immediately get started using the tools, just download the project and find the unitypackage file named DevToolsPackage, make
sure it is the correct version
Go into your project,
Under the assets toolbar, click import package->custom package
Navigate to the package in the download.
Click all and Import!

If you want to use the entire project to work on the tools themselves, add the root folder of the download from Unity Hub.

## Usage
The Prefabs are designed to literally be dropped into the scene and start working (whether they do is constantly being observed).

The AR Universal Canvas is to be the root for all menus displayed in the scene. The child canvas called Headpose Canvas is a
subgrouping for all of the menus that are supposed to be seen in front of the player's face. The menus are grouped into a
gameobject, which then can be enabled or disabled when needed

The MLEventSystem is a script that takes input from the ML API and the custom input module to determine what is being pointed at,
what is selected, what is being dragged, etc, and sends related events to those objects so they can react individually. MLEventData
outlines the interfaces that must be satisfied in order for a callback from the event system to be recieved

## Contributions
If something broke and you want it fixed, create an issue report.
If you want to add something to the tools, I guess make a branch and go to town.
