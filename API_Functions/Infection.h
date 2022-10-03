#ifndef INFECTION_HH_H
#define INFECTION_HH_H
#include "pch.h"
#include "StandardInteraction.h"









// Very basic SI model
struct Infection_v1 : public Model {

	Infection_v1(Construct* _construct);

	void think();

	std::vector<float> infection_rates;

	// "infection network"
	// agent x infection
	Graph<bool>* infection_net;
};





















// SI model that uses the InteractionMessage Model interface
struct Infection_v2 : public Model {
	Infection_v2(Construct* _construct);

	void communicate(InteractionMessageQueue::iterator msg);

	// "infection rate network"
	// infection x medium
	const Graph<float>* infection_probability;

	// "infection network"
	// agent x infection
	Graph<bool>* infection_net;
};






















// Modification that has people attempt to avoid other agents with an infection
struct Infection_v3 : public StandardInteraction {

	Infection_v3(const dynet::ParameterMap& params, Construct* _construct);

	const Graph<bool>* infection_net;

	const Graph<float>* risk_aversion;

	float get_additions(unsigned int sender, unsigned int reciever);
};








#endif
