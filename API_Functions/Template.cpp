#include "pch.h"
#include "Template.h"

Template::Template(const dynet::ParameterMap& parameters, Construct* _construct) : Model(_construct, model_names::TEMP)
{
#ifdef DO_NOT_FLAG
	//this section goes over a few examples
	//none of the following examples will be compiled or affect runtime

	//getting the current time period
	construct->current_time;

	//other important parameters are
	construct->verbose_initialization;
	construct->verbose_runtime;


	//getting the pointer to a nodeset, node, and accessing an attribute of that node
	//all nodeset names are held in the "nodes" namespace defined in NodesetManager.h
	const Nodeset* agents = ns_manager->get_nodeset(nodeset_names::agents);
	//All nodesets are constant. Their names, indexes, and attributes are unmutable by models
	const Node* phil = agents->get_node_by_name("phil");
	std::string phil_gender = phil->get_attribute("gender");

	//graphs are the primary data storage in construct
	//each model requires the associated nodesets and data type match exactly
	//if a graph is already loaded, but does not match an exception will be thrown
	//if there is no graph in a hard load an exception will also be thrown.
	// 
	//all string exceptions are handled by main.cpp giving detailed information as to the error
	//std::exception type exceptions are also handled by main.cpp, but indicate an unforseen error
	//finally all other possible types of exceptions are caught to ensure the user always knows what happened
	//
	//all graph/network names are stored in the "graph" namespace
	//confirming the data type is handled automatically, but take as arguments the required nodesets
	//all non-const graph loading takes the name of the model using the function
	//in debug mode this allows users to track which models have accessed and possible modified which graphs/networks

	Graph<bool>* interaction_network;
	graph_manager->hard_load(graph_names::interact, agents, agents, interaction_network, name);

	//if you want to create a graph you can add a default network below
	//this will only create a graph if the graph does not already exist.
	//all relevant information for a graph is required such as nodesets, dimension representation, and default values

	interaction_network = graph_manager->add_network(graph_names::interact, agents, true, agents, false, true, name);

	//to access an element without creating a link you can directly examine an entry
	//when examining an element it can not be modified

	unsigned int row = 4;
	unsigned int col = 3;
	const bool& result1 = interaction_network->examine(row, col);

	//to access an element you intend to change use the at function
	//if no link exists at this entry a link is created using the default value

	bool& result2 = interaction_network->at(row, col);
	result2 = !result2;

	//the clear function can remove a specific link to which the default value takes over
	//without any arguments the clear function will reset the entire data structure
	//to avoid needing to check the default value, submitting the new value to the at function
	//will remove the link if the submitted value is equal to the default value

	interaction_network->at(row, col, !result2);
	interaction_network->clear(0);
	//all values now set to zero

	//depending on dimension representation it can be costly to look up each individual link in a network
	//if you wish to iterate through a row or column in the graph the various set of iterators can be used
	//dereferencing these iterators will return the same result as examining that element
	//the element an iterator is pointing to can be found with the row() and col() functions

	std::cout << "Row " << row << ": ";
	for (auto it = interaction_network->full_row_begin(row); it != interaction_network->row_end(row); ++it) {
		std::cout << "(" << it.col() << "," << *it << ") ";
	}
	std::cout << std::endl;

	//to modify an element submit the iterator to the graph's at function which returns a reference to that element
	//as with the base case of the at function you can also submit the changed value to check if it equals the default value

	for (auto it = interaction_network->full_col_begin(col); it != interaction_network->col_end(col); ++it) {
		bool& temp = interaction_network->at(it);
		interaction_network->at(it, !temp);
	}

	//the full row iterators go over each index in that dimension
	//because we can have a sparse structure it may be advantegous to skip over elements we don't care about
	//ex we only care about the elements that are true in a graph with a default value of false
	//sparse iterators can be used to allow you to skip over elements of your choice

	std::cout << "True values in row " << row << ": ";
	for (auto it = interaction_network->sparse_row_begin(row, false); it != interaction_network->row_end(row); ++it) {
		std::cout << "(" << it.col() << "," << *it << ") ";
	}

	//sparse iterators obey the same rules as the full iterators regarding modifying values
	//in addition const_iterators are also available and are the default return when using a const graph
	//as expected these iterators can be used to examine, but not to modify graph elements
	//Note: iterators compare equality based on their row and col index so make sure iterator comparison happens on iterators from the same graph

	//When one wants to iterator over the entire data structute, another set of iterators and const_iterators can be used
	//these iterators point to the beginning of a row or column and skips the potential binary search for this beginning iterator

	for (auto row_begin = interaction_network->begin_rows(); row_begin != interaction_network->end_rows(); ++row_begin) {
		for (auto it = row_begin.full_begin(); it != row_begin.end(); ++it) {
			std::cout << *it << " ";
		}
		std::cout << std::endl;
	}

	//to do operations such as the overlap between two rows or a column and a row of the same or different graphs
	//iterators can be aligned using the graph_utils namespace

	auto rit = interaction_network->full_row_begin(row);
	auto cit = interaction_network->sparse_col_begin(col, false);
	std::vector<typeless_graph_iterator*> it_list = { &rit, &cit };

	//to align the iterators without first incrementing them use init_align
	//to advance to the next alignment of iterators use it_align
	//these functions rely on the index() function of the Graph_iterators
	//they are also prevented from incrementing beyond the bounds of a graph by the max() function
	
	unsigned int count = 0;
	unsigned int count2 = 0;

	for (graph_utils::init_align(it_list); rit != interaction_network->row_end(row); graph_utils::it_align(it_list)) {
		count++;
		if (*rit) count2++;
	}

	//for an intersection a similar set of functions can be used
	//with alignment, order in the vector of iterators is irrelevant
	//in these functions iterators are advanced such that the first iterator is always ahead of all other iterators in the list

	auto it1 = interaction_network->sparse_row_begin(row, false);
	auto it2 = interaction_network->sparse_row_begin(row + 1, false);
	std::vector<typeless_graph_iterator*> it_list2 = { &it1, &it2 };

	count = 0;

	for (graph_utils::init_align_before_first(it_list2); it1 != interaction_network->row_end(row); graph_utils::it_align_before_first(it_list2)) {
		count++;
	}

	//now count will represent the number of true elements that row + 1 has, but row does not

	//finally you can queue changes to a graph using the add delta function
	interaction_network->add_delta(row, col, true);

	//the deltas are automatically applied to each graph between the communicate and clean up functions
	//one can trigger a manual application by pushing deltas
	interaction_network->push_deltas();

	//many models via the exchange of messages
	//Construct will automatically disperse messages to be parsed in the communicate function of models
	//These messages are dispersed from the private message queue and they are erased after the clean up function
	auto interaction_queue = &construct->interaction_message_queue;
	auto public_queue = &construct->public_message_queue;

	//When distributing messages for parsing, construct will distribute from the private messages
	//To add messages one must add items into a message.
	//items have three data storage members; attributes, indexes, and values.

	InteractionItem item;
	item.attributes.insert(dynet::item_keys::knowledge);
	item.indexes[dynet::item_keys::knowledge] = 2;
	item.values[dynet::item_keys::ktrust] = 0.5;

	//messages require a vector of items

	std::vector<InteractionItem> items;
	//creates a knowledge item
	items.push_back(InteractionItem::create_knowledge_item(10));
	//creates a beliefTM item
	items.push_back(InteractionItem::create_beliefTM_item(0, 1, .5));

	//messages also require a communication medium
	//communication medium's information is stored in communication nodes
	//an iterator pointing to a communication node can be used to create a CommunicationMedium class
	
	const Nodeset* agent = ns_manager->get_nodeset(nodeset_names::agents);
	const Nodeset* comms = ns_manager->get_nodeset(nodeset_names::comm);

	comms->check_attributes<float>(comms_att::percent_learnable);
	comms->check_attributes<unsigned int>(comms_att::msg_complex);
	comms->check_attributes<unsigned int>(comms_att::tts);
	auto medium_node = comms->begin();

	CommunicationMedium medium(medium_node);

	//A custom CommunicationMedium can be created with all the relevant attribute information
	std::string comm_name = "medium name";
	unsigned int node_index = 0;
	float max_percent_learnable = 1;
	unsigned int max_msg_complexity = 10;
	unsigned int time_to_send = 0;

	CommunicationMedium medium2(comm_name, node_index, max_percent_learnable, max_msg_complexity, time_to_send);

	//Messages require a sender index, receiver index, a vector of items, and a Communication Medium.
	//The sender and receiver indexes corresponds to agent indexes
	unsigned int sndr = 0;
	unsigned int recv = 1;

	InteractionMessage msg(sndr, recv, &medium, items);

	//This messages can then be added to the message queue
	interaction_queue->addMessage(msg);

	//you can also interact with other models via the model manager
	//this manager is not pre loaded as it has a limited applications
	//models can be queried for their pointers. This is primarily to check if a model exists though other applications may be useful

	if (construct->model_manager.get_model_by_name(model_names::SIM)) {
		std::cout << "Standard Interaction Model is active." << std::endl;
	}

	//Some models are nested in that a model may execute another model internally.
	//To avoid potential conflicts and double parsing that model may not be directly loaded in the model manager
	//If two or more models use seperately the same model internally they can communicate their usage through a placeholder model

	construct->model_manager.add_model(new PlaceHolder(model_names::SIM));

	//now when all other models query for the Standard Interaction Model, they will see the model as currently active
	//importantly the PlaceHolder model does not perform any operations when called by the model manager
	//lastly it is important that other entities know if this model is a real model or a placeholder
	//all models have the variable valid which will be false if created using the PlaceHolder class

	if (!construct->model_manager.get_model_by_name(model_names::SIM)->valid) {
		std::cout << "The Standard Interaction Model loaded is a placeholder." << std::endl;
	}

	//by having a central random generation we can reliably reproduce results
	//if a seed is not given in the input xml, the random chosen seed value is printed at the beginning of the command line output
	//many of the usual distributions are available

	random->uniform();
	random->uniform(-1, 1);
	random->integer(10);
	random->exponential(0.5f);
	random->poisson_number(4.2f);
	random->randombool();

	//in addition there are an assortment of additional function that operate on and give results based on pdfs or cdfs
	std::vector<float> pdf;
	for (float i = 1; i < 10; i++) pdf.push_back(i);

	//swaps elements such that each element is in a random position
	random->vector_shuffle(pdf);

	//selects an index based on the weights in the pdf
	unsigned int selection = random->find_dist_index(pdf);

	//creates a list of indexes in the pdf with the index corresponding to the largest weight being most likely to be at the beginning of the vector
	std::vector<unsigned int> ordered_list = random->order_by_pdf(pdf);


#endif // DO_NOT_FLAG



}

//These functions are optional and can be removed.
void Template::initialize(void) {}
void Template::think(void) {}
void Template::update(void) {}
void Template::communicate(InteractionMessageQueue::iterator msg) {}
void Template::cleanup() {}

