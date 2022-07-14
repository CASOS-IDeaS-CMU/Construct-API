
#ifndef TEMPLATE_MODEL_HH_H
#define TEMPLATE_MODEL_HH_H

#include "pch.h"
//All models need to inherit the "Model" class in order to be added to the Model Manager
//The Model class allows for inheritance of Interaction Messages and Communication Mediums
//Sets the pointers for the construct, graph and nodeset manager, and the random generator


/// <summary>
/// %Template for building new Construct models.
/// <para>
/// This model provides the bare bones necessities for building a new model.
/// Simply copy and rename this header and cpp file, rename the flag TEMPLATE_MODEL_HH_H at the top of the header file, and rename the class.
/// Additionally be sure to change the string being passed to the Model constructor to your model's name.
/// </para>
/// </summary>
class Template : public Model
{
public:

	Template(dynet::ParameterMap parameters, Construct* construct);
	void initialize(void);
	void think(void);
	void update(void);
	void communicate(InteractionMessageQueue::iterator msg);
	void cleanup(void);
};
#endif



