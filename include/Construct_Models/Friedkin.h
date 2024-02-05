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
	/*
	Graph<std::map<unsigned int, float> >& interpersonal_influence = graph_manager.load_optional("interpersonal influence network", 
		std::map<unsigned int, float>(), nodeset_names::agents, dense, nodeset_names::agents, dense, nodeset_names::knowledge);
	*/
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
		for (auto& [agent_trust, agent_influences, agent_trust_tm, agent_knowledge] : graph_utils::it_align(
			knowledge_trust_net.begin_rows(), interpersonal_influence.cbegin_rows(), kttm.begin_rows(), knowledge_net.begin_rows())) 
		{
			
			for (auto& [k_trust, k] : graph_utils::it_align(agent_trust.sparse_begin(0.5f), agent_knowledge.sparse_begin(false))) {
				float weight = 0;
				
				float influence_normalization = 0;
				unsigned int influence_count = 0;
				


				/*for (auto& [alter_influence, alter_trust] : graph_utils::it_align(
				agent_influences.full_begin(), agent_trust_tm.full_begin())) 
				{
					auto trustit = alter_trust->find(k_trust.col());
					auto infit = alter_influence->find(k_trust.col());
					if (trustit != alter_trust->end()) {

						distance += (*k_trust - trustit->second) * (*k_trust - trustit->second);
						distance_alter_count++;

						if (infit != alter_influence->end()) {
							weight += trustit->second * infit->second;
							influence_normalization++;
						}
					}
				}*/

				for (auto& [alter_infuence, alter_trust] : graph_utils::it_align(
					agent_influences.sparse_begin(0.0f), agent_trust_tm.sparse_begin(std::map<unsigned int, float>())))
				{
					auto it = alter_trust->find(k_trust.col());
					if (it != alter_trust->end()) {
						weight += *alter_infuence * it->second;
						influence_normalization += *alter_infuence;
						influence_count++;
					}
				}
				if (influence_normalization > 0)
					knowledge_trust_net.add_delta(k_trust.row(), k_trust.col(), 
						susceptibility * weight / influence_count + (1 - susceptibility * influence_normalization) * (*k_trust));
			}

			for (auto& [alter_trust, alter_influence] : graph_utils::it_align(
				agent_trust_tm.sparse_begin(std::map<unsigned int, float>()), agent_influences.full_begin())) 
			{
				float distance = 0;
				unsigned int distance_alter_count = 0;
				for (auto& [k_trust, k] : graph_utils::it_align(agent_trust.full_begin(), agent_knowledge.sparse_begin(false))) {
					auto it = alter_trust->find(k_trust.col());
					if (it != alter_trust->end()) {
						distance += (*k_trust - it->second) * (*k_trust - it->second);
						distance_alter_count++;
					}
				}
				if (distance_alter_count > 0)
					interpersonal_influence.add_delta(alter_influence.row(), alter_influence.col(),
						influence * (1 - pow(distance / distance_alter_count, 0.5)) + (1 - influence) * (*alter_influence));
			}
		}
	}
};
