
How to format the "Big JSON":

ObjectInfoCollection holds an array of ObjectInfo objects. 
Each object in this array should correlate to an object in the scene.

ObjectInfo contains fields for basic information every objeect must have. (name, parentName, type, position, scale, material)
It also contains a string array called componentsToAdd. 
componentsToAdd contains JSONs passed as strings with the name of the component you are adding 
as well as the assignments of each public variable. 

Note: The Bridge creates the scene in the order that the objects are put into the array.
That means that parents must be put into the array before thier children. 

----------------------------------------------------------------------------------------------------------

How to add new functionality:

Adding new scripts is a two part process.

1) Create a new script that inherits from JsonObjects. (Don't worry it will still have Monobehaviour too)
Make sure you override the Construct funtion.

2)Create a serializable class to hold all the public variables. 
This class should be named YourScriptNameInfo and the public variables should have exactly the same names.
