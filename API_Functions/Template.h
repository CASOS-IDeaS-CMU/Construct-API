
#ifndef TEMPLATE_MODEL_HH_H
#define TEMPLATE_MODEL_HH_H

#include "pch.h"
//All models need to inherit the "Model" class in order to be added to the Model Manager
//The Model class allows for inheritance of Interaction Messages and Communication Mediums
//Sets the pointers for the construct, graph and nodeset manager, and the random generator

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



