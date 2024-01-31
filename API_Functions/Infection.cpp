#include "pch.h"
#include "Infection.h"








Infection_v1::Infection_v1(Construct& _construct) : Model(_construct) {
	const Nodeset& infections =	this->ns_manager.get_nodeset("infection");


	infections.check_attributes<float>("infection probability", 0.0f, 1.0f);

	infection_rates.resize(infections.size());
	for (const Node& node : infections) {
		infection_rates[node.index] = std::stof(node["infection probability"]);
	}
}

void Infection_v1::think() {

	// loop over all infections
	for (auto infection = infection_net.begin_cols(); infection != infection_net.end_cols(); ++infection) {
		// for each agent infected
		for (auto infected = infection.sparse_begin(false); infected != infection.end(); ++infected) {
			// attempt to infect every other susceptible agent
			for (auto susceptible = infection.sparse_begin(true); susceptible != infection.end(); ++susceptible) {
				
				if (random.uniform() < infection_rates[infection.col()]) {
					// deltas are added later all at once
					// changing the link immediatly would cause a bias towards agents with higher indexes
					infection_net.add_delta(susceptible.row(), susceptible.col(), true);
				}
			}
		}
	}
}






























Infection_v2::Infection_v2(Construct& _construct) : Model(_construct) {

	const Nodeset& agents = ns_manager.get_nodeset(nodeset_names::agents);
	const Nodeset& mediums = ns_manager.get_nodeset(nodeset_names::comm);
	const Nodeset& infections = ns_manager.get_nodeset(nodeset_names::infections);

	infection_probability = graph_manager.load_optional("infection rate network", 1.0f / agents.size(), infections, false, mediums, false);
}

void Infection_v2::communicate(const InteractionMessage& msg) {
	
	unsigned int sender =	msg.sender;
	unsigned int receiver = msg.receiver;
	unsigned int comm_index = msg.medium->index;
	if (comm_index >= infection_probability->target_nodeset->size()) return;

	for (auto inf_prob = infection_probability->full_col_begin(comm_index); inf_prob != infection_probability->col_end(comm_index); ++inf_prob) {
		if (
			*inf_prob >= random.uniform()
			&& infection_net.examine(sender, inf_prob.row())
			&& !infection_net.examine(receiver, inf_prob.row())
			)
			infection_net.add_delta(receiver, inf_prob.row(), true);
	}
	
}