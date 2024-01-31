#pragma once

void test() {
	Construct construct;

	Nodeset* nodeset1 = construct.ns_manager.create_nodeset("nodeset1");
	Nodeset* nodeset2 = construct.ns_manager.create_nodeset("nodeset2");

	nodeset1->add_nodes(100, dynet::ParameterMap());
	nodeset1->turn_to_const();
	nodeset2->add_nodes(200, dynet::ParameterMap());
	nodeset2->turn_to_const();

	Graph<bool>& test_graph = construct.graph_manager.load_optional("test graph", true, nodeset1, true, nodeset2, true);
	test_graph.apply_row_operation(0, [&](auto it) { return construct.random.randombool(0.3); });
}