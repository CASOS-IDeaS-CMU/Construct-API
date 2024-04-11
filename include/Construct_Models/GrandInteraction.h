#pragma once
#include "pch.h"

//Models this model will pull from
#include "StandardInteraction.h"
#include "Beliefs.h"
#include "Tasks.h"
#include "KnowledgeTransactiveMemory.h"

namespace model_parameters {
	const std::string beliefs_enabled = "beliefs enabled"; //"beliefs enabled"
	const std::string beliefTM_enabled = "belief transactive memory enabled"; //"belief transactive memory enabled"
	const std::string tasks_enabled = "tasks enabled"; //"tasks enabled"
	const std::string ktm_enabled = "knowledge transactive memory enabled"; //"knowledge transactive memory enabled"
	const std::string belief_roc = "belief rate of change"; //"belief rate of change"
}

struct GrandInteraction : public StandardInteraction
{
	KnowledgeTransactiveMemory* TMK = 0;

	Beliefs* BM = 0;

	Tasks* TASK = 0;
	//the following is for belief transactive memory
	//for more information on btm see DOI : 10.1109 / TSMCC.2012.2230255
	bool btm_enabled = false;
	//graph name - "belief message complexity network"
	//agent x timeperiod
	const Graph<unsigned int>* belief_message_complexity = 0;
	//graph name - "transactive belief message complexity network"
	//agent x timeperiod
	const Graph<unsigned int>* btm_message_complexity = 0;
	//graph name - "belief transactive memory network"
	//agent x agent x belief
	Graph<std::map<unsigned int, float> >* btm = 0;
	//graph name - "group belief network"
	//agentgroup x belief
	Graph<float>* _group_beliefs = 0;

	float alpha = 0;

	std::vector<float> generalized_other_beliefs;
	std::vector<unsigned int> group_sizes;


	//graph name - "agent group membership network"
	//agent x agentgroup
	Graph<bool>* group_membership = 0;

	GrandInteraction(const dynet::ParameterMap& parameters, Construct& construct);

	void initialize(void) override;

	void communicate(const InteractionMessage& msg) override;

	void cleanup(void) override;

	float get_k_sim(unsigned int initiator, unsigned int receiver) override;

	float get_k_exp(unsigned int initiator, unsigned int receiver) override;

	float get_belief_sim(unsigned int initiator, unsigned int receiver);

	float get_additions(unsigned int agent_i, unsigned int agent_j) override;

	std::vector<InteractionItem> get_interaction_items(unsigned int initiator, unsigned int receiver, const CommunicationMedium* comm) override;

	virtual void update_group_belief(void);
};