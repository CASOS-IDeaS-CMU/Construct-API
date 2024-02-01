#pragma once
#include "pch.h"

struct Friedkin : public Model {
	//graph name - "knowledge network"
	//agent x knowledge
	const Graph<bool>& knowledge_net = graph_manager.load_required(graph_names::knowledge, nodeset_names::agents, nodeset_names::knowledge);

	//graph name - "knowledge trust network"
	//agent x knowledge
	Graph<float>& knowledge_trust_net = graph_manager.load_optional(graph_names::k_trust, 0.5f,
		nodeset_names::agents, knowledge_net.row_dense, nodeset_names::knowledge, knowledge_net.col_dense);

	//graph name - "knowledge trust transactive memory network"
	//agent x agent x knowledge
	Graph<std::map<unsigned int, float> >& kttm = graph_manager.load_optional(graph_names::kttm, std::map<unsigned int, float>(),
		nodeset_names::agents, dense, nodeset_names::agents, sparse, nodeset_names::knowledge);

	Graph<float>& interpersonal_influence = graph_manager.load_optional("interpersonal influence network", 0.5f, nodeset_names::agents, dense, nodeset_names::agents, dense);
	float susceptibility;
	float influence;

	Friedkin(Construct& construct, const dynet::ParameterMap& parameters) : Model(construct) {
		susceptibility = std::stof(parameters.at("susceptibility"));
		influence = std::stof(parameters.at("influence"));

		const Nodeset& agents = ns_manager.get_nodeset(nodeset_names::agents);

		agents.check_attributes<bool>(node_attributes::send_t);
		agents.check_attributes<bool>(node_attributes::recv_t);
	}

	void update() override {
		for (auto& msg : construct.interaction_message_queue) {
			unsigned int sndr = msg.sender;
			unsigned int recv = msg.receiver;
			if (sndr == recv)continue;
			bool send_trust = dynet::convert((*knowledge_net.source_nodeset)[sndr][node_attributes::send_t]);
			bool recv_trust = dynet::convert((*knowledge_net.source_nodeset)[recv][node_attributes::recv_t]);
			if (send_trust && recv_trust) {
				for (auto& item : msg) {
					if (item.contains(InteractionItem::item_keys::knowledge) && !item.contains(InteractionItem::item_keys::ktrust)) {
						unsigned int k = item.get_knowledge();
						item.set_knowledge_trust_item(k, knowledge_trust_net.examine(sndr, k));
					}
				}
			}
		}
	}

	void communicate(const InteractionMessage& msg) override {
		for (auto& item : msg) {
			if (item.contains(InteractionItem::item_keys::ktrust)) {
				unsigned int j = msg.sender;
				auto alter = item.indexes.find(InteractionItem::item_keys::alter);
				if (alter != item.indexes.end()) j = alter->second;
				auto [k, trust] = item.get_knowledge_trust();
				kttm.at(msg.receiver, j).insert(std::pair(k, trust));
			}
		}
	}

	void cleanup() override {
		for (auto [agent_trust, alter_influence] : graph_utils::it_align(knowledge_trust_net.begin_rows(), interpersonal_influence.begin_rows())) {
		
		}
	}
};
