#include "Output.h"
#include "StandardInteraction.h"

int main() {

	Construct construct;

	// Construct parameters used by various components of Construct
	construct.verbose_initialization = true;
	construct.verbose_runtime = true;
	construct.random.set_seed(56732);
	construct.working_directory = ".";

	// whenever a nodeset is created it must be populated with nodes
	// each node can be loaded with attributes
	// if sets of nodes have the same attributes, add_nodes can be used to create sets of nodes
	// when sets of nodes are created, their name is auto-generated using the nodeset name as the root
	construct.ns_manager.add_nodeset(nodeset_names::agents).add_nodes(50, dynet::ParameterMap({ 
		{"can send knowledge", "true"},
		{"can receive knowledge", "true"} }));
	construct.ns_manager.add_nodeset(nodeset_names::knowledge).add_nodes(20, {});
	construct.ns_manager.add_nodeset(nodeset_names::time).add_nodes(10, {});

	//nodes can be added one at a time and each name can be set
	Nodeset& comms = construct.ns_manager.add_nodeset(nodeset_names::comm);

	comms.add_node("face to face", dynet::ParameterMap({
		{comms_att::msg_complex, "1"},
		{comms_att::percent_learnable, "1"},
		{comms_att::tts, "0"}
	}));

	comms.add_node("email", dynet::ParameterMap({
		{comms_att::msg_complex, "2"},
		{comms_att::percent_learnable, "1"},
		{comms_att::tts, "1"}
		}));

	// adding the knowledge network
	// Here I'm choosing for the graph to be dense in both dimensions with a default value of false.
	Graph<bool>& knowledge_net = construct.graph_manager.load_optional(
		graph_names::knowledge, false, nodeset_names::agents, dense, nodeset_names::knowledge, dense);

	// links can be added one by one or with a generator
	construct.random.binary_generator(knowledge_net, 0.2f, Bounding_Box<2>(knowledge_net));

	knowledge_net.at(4, 5) = true;

	// Models can be created using Construct::create_model where the model class is taken as a template input.
	// This function automatically adds the construct to the model, so you'll only need to include model parameters
	// that come before the construct reference
	construct.create_model<StandardInteraction>(dynet::ParameterMap());

	// Outputs are similar to models in that they can be created outside of Construct and require import into its own Manager.
	// Outputs can not be recovered after given to the output manager. This is because there is no uniqueness requirement for outputs.
	// This causes difficulty for a query interface and is avoided.
	dynet::ParameterMap output_params;
	output_params["network name"] = graph_names::knowledge;
	output_params["timeperiods"] = "all";
	output_params["output file"] = "my_output.csv";
	auto kout = new Output_Graph(output_params, construct);
	construct.output_manager.add_output(kout);

	// Runs the main Construct simulation starting with initialization which happens once at the beginning of the run function.
	// The remaining steps are think, update, communicate, and clean_up. 
	// Each model's respective function is called for each of these steps before moving on to the next step.
	// In the communicate function, each message is dispersed one at a time to each model.
	construct.run();
}