#include "pch.h"
#include "Friedkin.h"
#include "SocialMedia.h"
#include "Output.h"

int main() {

	Construct construct;

	dynet::ParameterMap ns_attributes;

	unsigned int time_count = 100;
	unsigned int knowledge_count = 1000;
	unsigned int agent_count = 1000;
	float knowledge_density = 0.1f;
	float dt = 1; //in hours
	float maximum_inactivity = 24;

	construct.ns_manager.create_nodeset(nodeset_names::time)->add_nodes(time_count, ns_attributes);
	construct.ns_manager.create_nodeset(nodeset_names::knowledge)->add_nodes(knowledge_count, ns_attributes);

	ns_attributes["Twitter post density"] = "0.208";
	ns_attributes["Twitter read density"] = "4.16";
	ns_attributes["Twitter repost probability"] = "0.07";
	ns_attributes["Twitter reply probability"] = "0.025";
	ns_attributes["Twitter quote probability"] = "0.0125";
	ns_attributes[node_attributes::send_k] = "true";
	ns_attributes[node_attributes::recv_k] = "true";
	ns_attributes[node_attributes::send_t] = "true";
	ns_attributes[node_attributes::recv_t] = "true";

	construct.ns_manager.create_nodeset(nodeset_names::agents)->add_nodes(agent_count, ns_attributes);

	Graph<bool>& knowledge_net = construct.graph_manager.load_optional(graph_names::knowledge, false, nodeset_names::agents, dense, nodeset_names::knowledge, sparse);
	Graph<float>& trust_net = construct.graph_manager.load_optional(graph_names::k_trust, 0.5f, nodeset_names::agents, dense, nodeset_names::knowledge, sparse);

	for (auto& [agent_knowledge, agent_trust] : graph_utils::it_align(knowledge_net.begin_rows(), trust_net.begin_rows())) {
		for (auto& [k, t] : graph_utils::it_align(agent_knowledge.full_begin(), agent_trust.full_begin())) {
			if (construct.random.uniform() < knowledge_density) {
				knowledge_net.at(k) = true;
				trust_net.at(t) = construct.random.uniform();
			}
		}
	}

	dynet::ParameterMap model_parameters;
	model_parameters["susceptibility"] = "0.1";
	model_parameters["influence"] = "0.1";

	construct.model_manager.add_model(model_names::TRUST, new Friedkin(construct, model_parameters));

	model_parameters.clear();
	model_parameters["interval time duration"] = std::to_string(dt);
	model_parameters["maximum post inactivity"] = std::to_string(maximum_inactivity);

	construct.model_manager.add_model(model_names::TWIT_nf, new Twitter_nf(model_parameters, construct));

	dynet::ParameterMap output_parameters;

	output_parameters["network names"] = graph_names::k_trust;
	output_parameters["timeperiods"] = "all";
	output_parameters["output file"] = "trust.xml";

	construct.output_manager.add_output(new Output_dynetml(output_parameters, construct));
	
}