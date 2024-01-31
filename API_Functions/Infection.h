#ifndef INFECTION_HH_H
#define INFECTION_HH_H
#include "pch.h"


// common names should be stored in a central location to limit typing errors

namespace nodeset_names {
	const std::string infections = "infection";
}

namespace graph_names {
	const std::string infection_net = "infection network";
}

namespace model_names {
	const std::string infection_v1 = "Infection v1 Model";
}




// Very basic SI model
struct Infection_v1 : public Model {

	// all models require at a minimum the Construct pointer
	Infection_v1(Construct& _construct);

	// use the override qualifier to make sure your model functions will be called correctly
	void think() override;

	// stores the attributes in the infection nodeset as floats
	std::vector<float> infection_rates;

	// "infection network"
	// agent x infection
	Graph<bool>& infection_net = graph_manager.load_required(graph_names::infection_net, nodeset_names::agents, nodeset_names::infections);
};





















// SI model that uses the InteractionMessage Model interface
struct Infection_v2 : public Model {
	Infection_v2(Construct& _construct);

	void communicate(const InteractionMessage& msg) override;


	// Graphs can be loaded as a pointer or reference
	// references are used similar to other object, but must be defined in the class/struct definition

	// "infection rate network"
	// infection x medium
	const Graph<float>* infection_probability;

	// "infection network"
	// agent x infection
	Graph<bool>& infection_net = graph_manager.load_required(graph_names::infection_net, nodeset_names::agents, nodeset_names::infections);
};




















#include "StandardInteraction.h"

// Modification that has people attempt to avoid other agents with an infection
// models don't require cpp file and everything can be declared in the header file
// cpp files simply make it easier to recompile or handle complex dependencies
struct Infection_v3 : public StandardInteraction {

	Infection_v3(const dynet::ParameterMap& params, Construct& _construct) :
		StandardInteraction(params, _construct) {}

	const Graph<bool>& infection_net = graph_manager.load_required(graph_names::infection_net, nodeset_names::agents, nodeset_names::infections);

	const Graph<float>& risk_aversion = graph_manager.load_optional("infection risk aversion network", 1.0f, nodeset_names::agents, sparse, nodeset_names::infections, dense);;

	// this overrides StandardInteraction::get_additions
	// this function will now be called when the parent class calls its get_additions function
	float get_additions(unsigned int sender, unsigned int reciever) override {
		float risk = 0;
		for (auto infections = infection_net.sparse_row_begin(reciever, true); infections != infection_net.row_end(reciever); ++infections) {
			risk += risk_aversion.examine(sender, infections.col());
		}
		return risk;
	}
};








#endif
