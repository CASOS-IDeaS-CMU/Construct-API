#pragma once
#include "pch.h"

namespace node_attributes {
	const std::string trust_resistance = "agent trust resistance";
	const std::string alter_trust_weight = "alter trust weight";
}

struct trust_parser : public InteractionMessageParser {

	Graph<std::map<unsigned int, float> >& kttm;

	trust_parser(Graph<std::map<unsigned int, float> >& kttm) : kttm(kttm), InteractionMessageParser("trust") {}

	void parse(const InteractionMessage& msg) override {
		for (auto& item : msg) {
			unsigned int j = msg.sender;
			auto alter = item.indexes.find(InteractionItem::item_keys::alter);
			if (alter != item.indexes.end()) j = alter->second;
			if (item.contains(InteractionItem::item_keys::ktrust)) {
				auto [k, trust] = item.get_knowledge_trust();
				kttm.at(msg.receiver, j).insert(std::pair(k, trust));
			}
		}
	}


};


struct Trust : public Model
{
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

	//graph name - "agent trust network"
	//agent x agnet
	Graph<float>& agent_trust_net = graph_manager.load_optional(graph_names::agent_trust, 0.0f,
		nodeset_names::agents, dense, nodeset_names::agents, sparse);

	//graph name - "knowledge trust resistance"
	//agent x knowledge
	const Graph<float>& knowledge_trust_resistance = graph_manager.load_optional(graph_names::ktrust_resist, 1.0f,
		nodeset_names::agents, sparse, nodeset_names::knowledge, sparse);

	//produced from agent node attribute "agent trust resistance"
	std::vector<float> agent_trust_resistance;

	//produced from agent node attribute "alter trust weight"
	std::vector<float> alter_trust_weight;

	

	Trust(const dynet::ParameterMap& parameters, Construct& construct);

	void initialize(void) override;

	void update(void) override;

	bool intercept(InteractionItem& item, unsigned int sender, unsigned int receiver, const CommunicationMedium* medium) override;

	void communicate(const InteractionMessage& msg) override;

	void cleanup(void) override;
};