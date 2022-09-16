#ifndef TASKS_HEADER_GUARD
#define TASKS_HEADER_GUARD
#include "pch.h"

//This model is a variant on the Standard Interaction Model
#include "StandardInteraction.h"

struct CONSTRUCT_LIB Tasks: public StandardInteraction
{
		const Nodeset* tasks = ns_manager->get_nodeset(nodeset_names::task);

		// graph name - "task assignment network"
		// agent x task
		const Graph<bool>& task_assignment = graph_manager->load_optional(graph_names::task_assignment, true, agents, sparse, tasks, sparse);

		// graph name - "task knowledge requirement network"
		// task x knowledge
		const Graph<bool>& tk_req = graph_manager->load_required(graph_names::task_k_req, tasks, knowledge);

		// graph name - "knowledge importance network"
		// task x knowledge
		const Graph<float>& tk_import = graph_manager->load_optional(graph_names::task_k_importance, 1.0f, tasks, sparse, knowledge, sparse);

		// graph name - "task guess probability network"
		// task x knowledge
		const Graph<float>& tk_prob = graph_manager->load_optional(graph_names::task_guess_prob, 0.0f, tasks, sparse, knowledge, sparse);

		// graph name - "completed tasks network"
		// agent x task
		Graph<unsigned int>& task_completion = graph_manager->load_optional(graph_names::task_completion, 0u, agents, sparse, tasks, sparse);

		// graph name - "task availability network"
		// task x timeperiod
		const Graph<bool>& availablity = graph_manager->load_optional(graph_names::task_availability, true, tasks, sparse, time, sparse);


		Tasks(const dynet::ParameterMap& parameters, Construct* construct);

		void initialize(void);
		void cleanup(void);

		float get_k_exp(unsigned int agent_i, unsigned int agent_j);
};
#endif



