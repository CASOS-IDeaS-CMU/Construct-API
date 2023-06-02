#include "pch.h" 
#include "Supp_Library.h"
#include "Output.h"
#include "Template.h"
#include "GrandInteraction.h"
#include "Location.h"
#include "SocialMedia.h"
#include "Forget.h"
#include "KnowledgeLearningDifficulty.h"
#include "Mail.h"
#include "Subscription.h"
#include "KnowledgeTrust.h"
#include "Emotions.h"
#include "Interventions.h"



//only edit the contents of each function and include any extra files needed



//both functions should be a sequence of if/esle if statements
//once the pointer is passed through the this function the relevant managers will
//take ownership and deconstruct the classes at the appropriate time
	
//It is expected that models and output will take in a ParameterMap and the Construct pointer, however it is not strictly required.
//Models will require the Construct pointer in order to have access to the loaded nodes, networks/graphs, and the interaction queue.

Model * dynet::create_model(const std::string & model_name, const ParameterMap& parameters, Construct * construct, const std::string& version) {

	assert(std::string(Construct::version) == version);

	if (model_name == "Template Model")
		return new Template(parameters, construct);

	//************************************
	//*    add your custom model here    *
	//************************************


	if (model_name == model_names::SIM)
		return new StandardInteraction(parameters, construct);

	else if (model_name == model_names::SUB)
		return new Subscription(construct);

	else if (model_name == model_names::KDIFF)
		return new KnowledgeLearningDifficulty(construct);

	else if (model_name == model_names::KTM)
		return new KnowledgeTransactiveMemory(parameters, construct);

	else if (model_name == model_names::FORGET)
		return new Forget(construct);

	else if (model_name == model_names::MAIL)
		return new Mail(construct);

	else if (model_name == model_names::TWIT_nf)
		return new Twitter_nf(parameters, construct);

	else if (model_name == model_names::FB_nf)
		return new Facebook_nf(parameters, construct);

	else if (model_name == model_names::TWIT_wf)
		return new Twitter_wf(parameters, construct);

	else if (model_name == model_names::FB_wf)
		return new Facebook_wf(parameters, construct);

	else if (model_name == model_names::EMOT)
		return new Emotions(construct);

	else if (model_name == model_names::TWIT_nf_emot)
		return new Twitter_nf_emotions(parameters, construct);

	else if (model_name == model_names::FB_nf_emot)
		return new Facebook_nf_emotions(parameters, construct);

	else if (model_name == model_names::TWIT_wf_emot)
		return new Twitter_wf_emotions(parameters, construct);

	else if (model_name == model_names::FB_wf_emot)
		return new Facebook_wf_emotions(parameters, construct);

	else if (model_name == model_names::BELIEF)
		return new Beliefs(parameters, construct);

	else if (model_name == model_names::TASK)
		return new Tasks(parameters, construct);

	else if (model_name == model_names::GRAND)
		return new GrandInteraction(parameters, construct);

	else if (model_name == model_names::TRUST)
		return new Trust(parameters, construct);

	else if (model_name == model_names::LOC)
		return new Location(construct);

	return NULL;
}

Output* dynet::create_output(const std::string& output_name, const ParameterMap& parameters, Construct* construct, const std::string& version) {

	assert(std::string(Construct::version) == version);

	if (output_name == output_names::output_graph) {
		return new Output_Graph(parameters, construct);
	}
	else if (output_name == output_names::output_dynetml) {
		return new Output_dynetml(parameters, construct);
	}
	else if (output_name == output_names::output_messages)
		return new Output_Messages(parameters, construct);
	else if (output_name == output_names::output_media_events)
		return new Output_Events(parameters, construct);
	//*************************************
	//*    add your custom output here    *
	//*************************************

	return NULL;
}


void dynet::load_users(Social_Media_no_followers* media) {
	for (auto node = media->agents->begin(); node != media->agents->end(); ++node) {
		//custom users should be added at the vector index instead of the default user

		if (node->attributes->contains("censored"))
			media->users[node->index] = new Intervention1_nf(media, *node);

		else if (node->attributes->contains("one time compel"))
			media->users[node->index] = new Intervention2_nf(media, *node);

		else if (node->attributes->contains("compel"))
			media->users[node->index] = new Intervention3_nf(media, *node);

		else 
			media->users[node->index] = new Social_Media_no_followers::default_media_user(media, *node);
	}
}

void dynet::load_users(Social_Media_with_followers* media) {
	for (auto node = media->agents->begin(); node != media->agents->end(); ++node) {
		//custom users should be added at the vector index instead of the default user
		if (node->attributes->contains("censored"))
			media->users[node->index] = new Intervention1(media, *node);

		else if (node->attributes->contains("one time compel"))
			media->users[node->index] = new Intervention2(media, *node);

		else if (node->attributes->contains("compel"))
			media->users[node->index] = new Intervention3(media, *node);

		else
			media->users[node->index] = new Social_Media_with_followers::default_media_user(media, *node);

		media->Social_Media_no_followers::users[node->index] = media->users[node->index];
	}
}

void dynet::load_users(SM_nf_emotions* media) {
	for (auto node = media->agents->begin(); node != media->agents->end(); ++node) {
		media->users[node->index] = new SM_nf_emotions::default_media_user(media, *node);
	}
}

void dynet::load_users(SM_wf_emotions* media) {
	for (auto node = media->Social_Media_no_followers::agents->begin(); node != media->Social_Media_no_followers::agents->end(); ++node) {
		media->Social_Media_with_followers::users[node->index] = new SM_wf_emotions::default_media_user(media, *node);
		media->Social_Media_no_followers::users[node->index] = media->Social_Media_with_followers::users[node->index];
	}
}