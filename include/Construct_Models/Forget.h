#ifndef FORGET_HEADER_GUARD
#define FORGET_HEADER_GUARD
#include "pch.h" 

struct CONSTRUCT_LIB Forget : public Model
{
	const Nodeset* agents;

	//graph name - "knowledge network"
	//agent x knowledge
	Graph<bool>* knowledge_net = 0;

	//graph name - "knowledge strength network"
	//agent x knowledge
	Graph<float>* knowledge_strength = 0;

	//graph name - "knowledge forgetting rate network"
	//agent x knowledge
	const Graph<float>* knowledge_forget_rate = 0;

	//graph name - "knowledge forgetting prob network"
	//agent x knowledge
	const Graph<float>* knowledge_forget_prob = 0;

	//graph name - "knowledge trust network"
	//agent x knowledge
	Graph<float>* ktrust_net = 0;

	//sparse matrix of users who used a piece of knowledge
	//gets reset each time step
	//not intended to get input any input will be cleared at the beginning of each time step
	Graph<bool>* unused_knowledge = 0;

	Forget(Construct* construct);
	void think(void);
	void communicate(InteractionMessageQueue::iterator msg);
	void cleanup(void);

};
#endif



