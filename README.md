# The Construct API
Useful Links:

[CASOS Construct Webpage](http://casos.cs.cmu.edu/projects/construct)

[Stand alone executables and GUI](http://casos.cs.cmu.edu/projects/construct/software.php)

[Construct User Guide](http://casos.cs.cmu.edu/publications/papers/CMU-ISR-22-102.pdf)

[API Documenation](http://casos.cs.cmu.edu/projects/construct/API/index.html)

## What is Construct?
Construct is an agent-based simulation framework based in C++20. 
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
Custom media users allows user to customize agent decision making without having to modify the Social Media model they belong to.
These options provide a majority of the customization developers may want to use.
The remainder of the customization allowed for users would be to create an instance of Construct in another program, manually load all the disired components, and run the simulation.

## Should I use the Construct API?
The Construct API is geared towards developers that wish to make use of currently existing models.
In addition, the modularity available from the interface system of nodesets, networks, and interaction messages can be appealing for developers wanting to create new models that interact with existing models, or a library of models to interact with each other.
With the ability to create and run Construct simulations inside other programs without creating a new thread, the API allows for substantial increase in perfomance of any interface or launching a large number of simulation runs of existing models.
Finally, developers may wish to take advantage of some of the data structures available in Construct such as Graph which allows developers to use a two dimensional network that can be represented as sparse or dense without requiring the information at compile time.


## Creating a Custom Model
Custom Model creation examples can be seen in the folder API_Fuctions, which contains the Visual Studio solution Construct_DLL.sln.
This solution contains all the setup required to build the windows dll that the Construct exes can take as input.
For those on a Unix operating system, a Makefile is included which serves a similar purpose as the VS solution.
Construct will search for the dll/shared library in the directory from which the Construct exes are called.
These exes are currently located in x64/(Debug, Quiet, or Release) with each folder holding an exe compiled with different flags based on the configuration.
Construct looks for three functions in these files, "create_custom_model", "create_custom_output", "create_custom_media_user_no_followers", and "create_custom_media_user_with_followers".
These functions are contained in Supp_Library.
In the debug configuration when verbose initialization is set to true, messages will be displayed indicating that the library was loaded and if the functions were found.

The "create_custom_model" function is the injection point for any custom models.
This function returns a Model pointer.
All custom models must inheriet (sometimes indirectly) from the Model base class.
Custom models can inherit from already exsiting models such as the StandardInteraction Model, or the Social_Media Model.
Inheriting from an existing model with virtual functions allows developers to replace that model's function which affects how the model interacts with itself and other entities.
An example would be modifiying StandardInteraction's knowledge_similarity function, which uses jaccard similarity, to use a different metric.

Once a custom model has been defined, it must be included in the definition of "create_custom_model".
The function call gives as input the model's name.
This allows Construct to select the appropriate model.
If the custom model's name is given (the same string given to the Model constructor), the custom model should be allocated with new and its pointer returned by the function.
To aid in the example of how developers can create models and how interfaces between models can work, the Template files serve as an examples for how the Construct interfaces work.

## Creating Custom Output

With custom models, custom outputs are needed to export information in methods not currently supported.
An example would be exporting when certain conditions were met, or aggregating data before exporting.
Custom output operates very similarly to how models are defined.
Each output must inheriet from the Output class, each output is selected using the output_name string, and the output must be allocated with new and returned by the function.

## Custom Media Users

As with the previous two examples, custom media users must inherit from the media_user sub-class of a Social Media model, allocated using new, and returned by the function.
The significant difference is a specific social media model and a nodeset iterator from the agent nodeset is used as input.
Attributes of the agent node then can be used to determine which type of media user should be returned.
This customization allows developers to create different types of users that can interact on a social media.
An example would be creating a user that always reposts events from a specific account, or always creates events with the same content.

## Full Manual Construct

For those wanting additional control and customization, an instance of Construct can be created inside another program.
Input can be read from an xml file, or nodeset, networks, models, and outputs can be manually defined and added to Construct.
An application for this would be for creating a gui in java.
Information can pass through the jni interface into nodesets, networks, models, and output, and can be passed back across the interface using outputs.
For performing a large number of runs, nodesets and networks can be exported and imported through Construct.
This will allow for quick construction of these structures and thus faster overall computation time.

## Disclaimer
Funding for this project has specifically requested that all source and definition files be kept private.
Contact [CASOS](http://casos.cs.cmu.edu)