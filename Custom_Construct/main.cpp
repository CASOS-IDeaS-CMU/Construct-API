#include "Construct.h"

int main() {
	Construct construct;

	// Construct parameters used by various components of Construct
	construct.verbose_initialization = true;
	construct.verbose_runtime = true;
	construct.random.set_seed(56732);
	construct.working_directory = ".";

	// creating the agent nodeset
	// Any attribute pointer previously submitted to add_node can be always safely be used in subsequent add_node calls.
	// if the attribute pointer needs to be deallocated add_node will return true
	Nodeset* agents = construct.ns_manager.create_nodeset(nodeset_names::agents);
	auto agent_attributes = new dynet::ParameterMap();
	agent_attributes->insert(std::pair("can send knowledge", "true"));
	agent_attributes->insert(std::pair("can receive knowledge", "true"));
	for (int i = 0; i < 50; i++) {
		agents->add_node(agent_attributes);
	}
	// All nodesets have to have turn_to_const called in order for them to be discoverable by the nodeset manager and to be used to create graphs.
	agents->turn_to_const();

	Nodeset* knowledge = construct.ns_manager.create_nodeset(nodeset_names::knowledge);
	auto knowledge_attributes = new dynet::ParameterMap();
	for (int i = 0; i < 20; i++) {
		knowledge->add_node(knowledge_attributes);
	}
	knowledge->turn_to_const();

	Nodeset* time = construct.ns_manager.create_nodeset(nodeset_names::time);
	auto time_attributes = new dynet::ParameterMap();
	for (int i = 0; i < 10; i++) {
		time->add_node(time_attributes);
	}
	time->turn_to_const();

	Nodeset* comms = construct.ns_manager.create_nodeset(nodeset_names::comm);

	// each new set of attributes requires a new allocation
	// the pointers are saved in the nodeset so no need to deallocate them after calling add_node
	auto comm_attributes = new dynet::ParameterMap();
	comm_attributes->insert(std::pair(comms_att::msg_complex, "1"));
	comm_attributes->insert(std::pair(comms_att::percent_learnable, "1"));
	comm_attributes->insert(std::pair(comms_att::tts, "0"));
	comms->add_node(comm_attributes);

	comm_attributes = new dynet::ParameterMap();
	comm_attributes->insert(std::pair(comms_att::msg_complex, "2"));
	comm_attributes->insert(std::pair(comms_att::percent_learnable, "1"));
	comm_attributes->insert(std::pair(comms_att::tts, "1"));
	comms->add_node(comm_attributes);

	comms->turn_to_const();


	// adding the knowledge network
	// Here I'm choosing for the graph to be dense in both dimensions with a default value of false.
	Graph<bool>* knowledge_net = construct.graph_manager.load_optional(graph_names::knowledge, false, agents, true, knowledge, true);

	// links can be added one by one or with a generator
	dynet::ParameterMap generator_params;
	generator_params["density"] = "0.2";
	generator_params["src min"] = "0";
	generator_params["src max"] = std::to_string(agents->size() - 1);
	generator_params["trg min"] = "0";
	generator_params["trg max"] = std::to_string(knowledge->size() - 1);
	construct.graph_manager.generators.binary_generator_2d(generator_params, knowledge_net);

	// Models can be created outside of Construct, but still require the construct pointer.
	// Once models are created they must be added to the model manager so it can participate in the simulation.
	auto SIM = new StandardInteraction(dynet::ParameterMap(), &construct);
	construct.model_manager.add_model(SIM);

	// Outputs are similar to models in that they can be created outside of Construct and require import into its own Manager.
	// Outputs can not be recovered after given to the output manager. This is because there is no uniqueness requirement for outputs.
	// This causes difficulty for a query interface and is avoided.
	dynet::ParameterMap output_params;
	output_params["network name"] = graph_names::knowledge;
	output_params["timeperiods"] = "all";
	output_params["output file"] = "my_output.xml";
	auto kout = new Output_Graph(output_params, &construct);
	construct.output_manager.add_output(kout);

	// Runs the main Construct simulation starting with initialization which happens once at the beginning of the run function.
	// The remaining steps are think, update, communicate, and clean_up. 
	// Each model's respective function is called for each of these steps before moving on to the next step.
	// In the communicate function, each message is dispersed one at a time to each model.
	construct.run();

}