# The Construct API
Useful Links:
[Casos Construct Webpage](http://casos.cs.cmu.edu/projects/construct)
[Stand alone executables and GUI](http://casos.cs.cmu.edu/projects/construct/software.php)
[Construct User Guide](http://casos.cs.cmu.edu/publications/papers/CMU-ISR-22-102.pdf)
[API Documenation](http://casos.cs.cmu.edu/projects/construct/API/index.html)

## What is Construct?
Construct is an agent-based simulation framework based in C++. 
Within this framework are various agent-based models that allow the simulated agents to make desicisions based on stimuli.
Stimuli in these models comes from three sources, examining the current state of various networks, the attributes a node has, and receiving interaction messages from other agents.
Nodesets, networks, models, and output are all loaded from an input xml file.
Agents can decide which person to interact with based on their connections in certain networks.
When agents interact they send interaction messages with content determined by the state of various networks and node attributes.

## What is the Construct API?
The API allows users to have greater customization of Construct than what is available in the text input.
While it is certainly possible to add additional features available through the input, this adds complexity and increases the difficulty for a new user to begin using the software.
Instead, the Construct API has been developed to give developers the customization that may be lacking in the software package.
Using this API, developers have the ability to include dynamic changes in proximity between agents, develop sophisticated and intritic relational dynamics between agents, and reconfigure how feeds are structured in social media.

To accomplish this, three functions are exported to a dynamic/shared library that allow developers to inject custom models, outputs, and media users.
Custom models can be created that inheriet from existing models allowing users to modify a behaviour without completely rebuilding all dynamics or a unique model can be created that interacts with other existing models.
Custom outputs allow developers to create dedicated output interface or do precalculations before exporting information to disk.
Custom media users allows user to customize agent decision making without having to modify the Social Media model.
These options provide a majority of the customization developers may want to use




Construct hosts a plethora of models built on this framework.
For a full review of how Construct handles the input of nodes, networks, models, and outputs, as well as a review of the behavior of each model, refer to the [Construct User Guide](http://casos.cs.cmu.edu/publications/papers/CMU-ISR-22-102.pdf).
This guide handles how to create new models to interact with prexisting models and how to call those models via the input xml file.
In addition this guide will also go over creating custom networks, nodesets, and outputs and how to load those entities into Construct.
For documentation of how to use the various classes and functions in Construct, refer to the [API Documenation](http://casos.cs.cmu.edu/projects/construct/API/index.html).

To get started, we have the Visual Studio solution Construct_DLL.sln that has all the intial configurations setup for the developer.
Construct is currently only supported in Windows and is compiled using C++20.
A set of input xml files have already been provided in Example Files folder.
The basic_custom_model_test will call the "Template Model" which can be seen in Template.h and Template.cpp.
Edits to the constructor and various model functions can then be seen when using this input file.
To run construct with this file move it the same folder as the solution file and rename it "construct.xml".
The solution by default will search for a file with this name as the command argument for execution.
Finally, Construct can be ran by selecting Debug > Start Debugging in visual studio or by hitting F5.
The other two example files show two examples of Construct executing one of its models.

When Construct successfully launches, Construct will immediatly print the current version number to the screen.
Once it has been confirmed that Construct is running properly, developers can explore creating custom models, output, and social_media users in Supp_Library.h and Supp_Library.cpp.
The functions contained in the dynet namespace produce the dynamiclly linked library (DLL) that the contained exectuables require.
The DLL and exe files for Construct are contained in x64\Debug, x64\Release, and x64\Quiet.
These folders correspond to the Debug, Release, and Quiet configurations in Construct_DLL.sln.
It is recommended when testing to use the Debug configuration which allows Construct to raise various saftey assertions if problems arise.
The Quiet configuration simply disables all output except error reporting.

To create a new model, output, or media_user simply create and include a new class that inheriets from the desired base class.
This class must implement any required functions such as the Model::Constructor and Output::process.
Then using an if statement a pointer to a newly allocated class should be returned.
Models are decided by model names, output by output names, and media_user by node attributes.
Classes should be allocated using new to presist beyond the function scope.
Once passed through these functions, the calling class takes ownership and will deallocate the pointers on deconstruction.

Finally a blank instance of Construct can be created and the developer can manually add nodes, nodesets, networks, models, and outputs without creating an input xml file.
This allows for pointers that exists outside of Construct to be included in models for full custom support.
As before once Construct and its entities take ownership of a pointer, it will deallocate that pointer upon deconstruction.

Final Notes:
Some classes are not meant to be dereferenced.
Funding for this project has specifically requested that all source files be kept private and only the minimum amount of information be visible to allow users to use this API.
For that reason a number of classes have had private and protected variables removed and so long as pointers to these classes are not derefrenced, no undefined behavour is expected.
To protect developers, an assertion is raised any time a protected pointer is dereferenced.
