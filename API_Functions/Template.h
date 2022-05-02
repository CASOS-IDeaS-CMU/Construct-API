
#ifndef TEMPLATE_MODEL_HH_H
#define TEMPLATE_MODEL_HH_H

#include "pch.h" 

struct Template : public Model {
	Template(const dynet::ParameterMap& parameters, Construct* construct);
	void initialize(void);
	void think(void);
	void update(void);
	void communicate(InteractionMessageQueue::iterator msg);
	void cleanup(void);
};
#endif



