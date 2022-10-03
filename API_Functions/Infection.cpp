#include "pch.h"
#include "Infection.h"








Infection_v1::Infection_v1(Construct* _construct) : Model(_construct, "Infection v1 Model") {

	auto agents =		this->ns_manager->get_nodeset(nodeset_names::agents);
	auto infections =	this->ns_manager->get_nodeset("infection");


	infections->check_attributes<float>("infection probability", 0.0f, 1.0f);

	infection_rates.resize(infections->size());
	for (auto it = infections->begin(); it != infections->end(); ++it) {
		infection_rates[it->index] = std::stof(it->attributes->find("infection probability")->second);
	}

	infection_net = this->graph_manager->load_required("infection network", agents, infections);
}

void Infection_v1::think() {

	// loop over all infections
	for (auto infection = infection_net->begin_cols(); infection != infection_net->end_cols(); ++infection) {
		// for each agent infected
		for (auto infected = infection.sparse_begin(false); infected != infection.end(); ++infected) {
			// attempt to infect every other susceptible agent
			for (auto susceptible = infection.sparse_begin(true); susceptible != infection.end(); ++susceptible) {
				
				if (random->uniform() < infection_rates[infection.col()]) {
					// deltas are added later all at once
					// changing the link immediatly would cause a bias towards agents with higher indexes
					infection_net->add_delta(susceptible.row(), susceptible.col(), true);
				}
			}
		}
	}
}






























Infection_v2::Infection_v2(Construct* _construct) : Model(_construct, "Infection v2 Model") {

	auto agents = ns_manager->get_nodeset(nodeset_names::agents);
	auto mediums = ns_manager->get_nodeset(nodeset_names::comm);
	auto infections = ns_manager->get_nodeset("infection");

	infection_probability = graph_manager->load_optional("infection rate network", 1.0f / agents->size(), infections, false, mediums, false);

	infection_net = graph_manager->load_required("infection network", agents, infections);
}

void Infection_v2::communicate(InteractionMessageQueue::iterator msg) {
	
	unsigned int sender =	msg->sender;
	unsigned int receiver = msg->receiver;
	unsigned int comm_index = msg->medium->index;
	if (comm_index >= infection_probability->target_nodeset->size()) return;

	for (auto inf_prob = infection_probability->full_col_begin(comm_index); inf_prob != infection_probability->col_end(comm_index); ++inf_prob) {
		if (
			*inf_prob
			&& infection_net->examine(sender, inf_prob.row())
			&& !infection_net->examine(receiver, inf_prob.row())
			)
			infection_net->add_delta(receiver, inf_prob.row(), true);
	}
	
}





















Infection_v3::Infection_v3(const dynet::ParameterMap& params, Construct* _construct) : 
	StandardInteraction(params, _construct), Model(_construct, "Standard Interaction Infection Model") 
{
	auto agents = ns_manager->get_nodeset(nodeset_names::agents);
	auto infections = ns_manager->get_nodeset("infection");

	infection_net = graph_manager->load_required("infection network", agents, infections);
	risk_aversion = graph_manager->load_required("infection risk aversion network", agents, infections);
}

float Infection_v3::get_additions(unsigned int sender, unsigned int reciever) {
	float risk = 0;
	for (auto infections = infection_net->sparse_row_begin(reciever, true); infections != infection_net->row_end(reciever); ++infections) {
		risk += risk_aversion->examine(sender, infections.col());
	}
	return risk;
}