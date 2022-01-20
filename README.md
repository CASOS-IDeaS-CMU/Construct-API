# Construct-API
Construct is an agent-based simulation framework based in c++. 
Within this framework are various agent-based models that allow the simulation agents to make desicisions based on stimuli.
Stimuli comes from two sources, examining the current state of various networks and receiving interaction messages for other agents.
Agents can decide which person to interact with based on their connections in a network.
Agents can also decide whether to respond and create a reply chain when sent an interaction message.

Construct hosts a plethora of models built on the framework of nodes and networks.
For a full review of how Construct handles nodes, networks, models, and outputting information, refer to the Construct User Guide (Insert Link when TR becomes public).
This guide handles how to input the various components into Construct using an input xml file.
Construct is very flexible when handling input and will parse everything it is given if possible.
This will allow developers to include custom nodesets, networks, outputs, and models that utilize these custom inputs.

To get started, we have the Visual Studio solution Construct_DLL.sln that has all the intial configurations setup for the developer.
Second, a set of input xml files have already been provided in Example Files.
The basic_custom_model_test will call the "Template Model" which can be seen in Template.h and Template.cpp.
Edits to the constructor and various model functions can then be seen when using this input file.
To run construct with this file move it the solution folder and rename it construct.xml.
The solution by default will search for a file with this name as the command argument for execution.
The other two example files show two examples of Construct executing one of its models.

Once it is tested that Construct runs properly, developers can explore creating custom models, output, and social_media users in Supp_Library.h and Supp_Library.cpp.
The functions contained in the dynet namespace produce the dynamiclly linked library (DLL) that versions of Construct given here must have.
The DLL and exe files for Construct are contained in x64\Debug and x64\Release.
These folders correspond to the Debug and Release configurations in Construct_DLL.sln.
It is recommended when testing to use the Debug configuration which allows Construct to raise various saftey assertions if problems arise.

To create a new model, output, or media_user simply create and include a new class that inheriets from the desired base class.
This class must implement any required functions such as the Model::Constructor and Output::process.
Then using an if statement a pointer to these classes should be returned.
Models are decided by model names, output by output names, and media_user by node attributes.
Classes should be allocated using new to presist beyond the function scope.
Once passed through these functions, the calling class takes ownership and will deallocate the pointers on deconstruction.

Finally a blank instance of Construct can be created and the developer can manually add nodes, nodesets, networks, models, and outputs.
This allows for pointers that exists outside of Construct to be included in models for full custom support.
As before once Construct and its entities take ownership of a pointer, it will deallocate that pointer upon deconstruction.

Final Notes:
Some classes are not meant to be dereferenced.
Funding for this project has specifically requested that all source files be kept private and only the minimum amount of information be visible to allow users to use this API.
For that reason a number of classes have had private and protected variables removed and so long as pointers to these classes are not derefrenced, no undefined behavour is expected.
To protect developers an automatic assertion is raised any time a protected pointer is dereferenced.
