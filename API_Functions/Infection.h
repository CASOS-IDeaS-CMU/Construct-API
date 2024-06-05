#pragma once
#include "pch.h"

namespace nodeset_names {
	const std::string infections = "infection";
}

namespace graph_names {
	const std::string infection_net = "infection network";
}

struct Infection : public Model {
	Infection(Construct& construct) : Model(construct) {}

	Graph<bool>& infection_net = graph_manager.load_required(graph_names::infection_net, nodeset_names::agents, nodeset_names::infections);
	const Graph<float>& comm_infection_probability = graph_manager.load_required("medium infection probability network", nodeset_names::infections, nodeset_names::comm);

	void update() {
		for (auto& msg : construct.interaction_message_queue) {
			if (msg.medium->index < comm_infection_probability.col_size)
			for (auto it = infection_net.sparse_row_cbegin(msg.sender, false); it != infection_net.row_end(msg.sender); ++it) {
				if (msg.medium->max_msg_complexity < msg.size() && construct.random.uniform() < comm_infection_probability.examine(it.col(), msg.medium->index)) {
					InteractionItem item;
					item.attributes.insert(InteractionItem::item_keys::infection);
					item.indexes[InteractionItem::item_keys::infection] = it.col();
					msg.add_item(item);
				}
			}
		}
	}

	void communicate(const InteractionMessage& msg) {
		for (auto& item : msg) {
			if (item.contains(InteractionItem::item_keys::infection)) {
				infection_net.at(msg.receiver, item.indexes.at(InteractionItem::item_keys::infection), true);
			}
		}
	}
};

#include "StandardInteraction.h"

// Modification that has people attempt to avoid other agents with an infection
struct InfectionInteraction : public StandardInteraction {

	InfectionInteraction(const dynet::ParameterMap& params, Construct& _construct) :
		StandardInteraction(params, _construct) {}

	const Graph<bool>& infection_net = graph_manager.load_required(graph_names::infection_net, nodeset_names::agents, nodeset_names::infections);

	const Graph<float>& risk_aversion = graph_manager.load_optional("infection risk aversion network", 1.0f, nodeset_names::agents, sparse, nodeset_names::infections, dense);;

	// this overrides StandardInteraction::get_additions
	// this function will now be called when the parent class calls its get_additions function
	float get_additions(unsigned int sender, unsigned int reciever) override {
		float risk = 0;
		// skips over infections
		for (auto infections = infection_net.sparse_row_begin(reciever, true); infections != infection_net.row_end(reciever); ++infections) {
			risk += risk_aversion.examine(sender, infections.col());
		}
		return risk;
	}
};
