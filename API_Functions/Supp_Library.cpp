
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



//only edit the contents of each function and include any extra files needed

namespace dynet {

	//both functions should be a sequence of if/esle if statements
	//once the pointer is passed through the this function the relevant managers will
	//take ownership and deconstruct the classes at the appropriate time
	
	//It is expected that models and output will take in a ParameterMap and the Construct pointer, however it is not strictly required.
	//Models will require the Construct pointer in order to have access to the loaded nodes, networks/graphs, and the interaction queue.

	Model * create_model(const std::string & model_name, const ParameterMap& parameters, Construct * construct) {

		if (model_name == "Template Model")
			return new Template(parameters, construct);

		//************************************
		//*    add your custom model here    *
		//************************************


		else if (model_name == model_names::SIM)
			return new StandardInteraction(parameters, construct);

		//Mostly tested - 7/19/21 model can become a memory hog so a full test can't be completed.
		else if (model_name == model_names::SUB)
			return new Subscription(construct);

		//Fully tested - 7/19/21
		else if (model_name == model_names::KDIFF)
			return new KnowledgeLearningDifficulty(construct);

		//Fully tested - 7/16/21
		else if (model_name == model_names::KTM)
			return new KnowledgeTransactiveMemory(parameters, construct);

		//Fully tested - 7/16/21
		else if (model_name == model_names::FORGET)
			return new Forget(construct);

		//Fully tested - 7/20/21
		else if (model_name == model_names::MAIL)
			return new Mail(construct);

		//Fully tested - 7/26/21 : I'm constantly working on this model however.
		else if (model_name == model_names::TWIT)
			return new Twitter(parameters, construct);

		//Same as above
		else if (model_name == model_names::FB)
			return new Facebook(parameters, construct);

		//Fully tested - 7/20/21
		else if (model_name == model_names::BELIEF)
			return new Beliefs(parameters, construct);

		//Fully tested - 7/20/21
		else if (model_name == model_names::TASK)
			return new Tasks(parameters, construct);
		//Mostly tested - 7/23/21 - the belief calculations are pretty complex and I'm not spending the time double checking an overly complex equation
		else if (model_name == model_names::GRAND)
			return new GrandInteraction(parameters, construct);

		else if (model_name == model_names::TRUST)
			return new KnowledgeTrust(parameters, construct);

		else if (model_name == model_names::LOC)
			return new Location(construct);

		else if (model_name == model_names::EMOT)
			return new Emotions(construct);

		else if (model_name == model_names::FB_nf_emot)
			return new Facebook_nf_emotions(parameters, construct);

		else if (model_name == model_names::FB_wf_emot)
			return new Facebook_wf_emotions(parameters, construct);

		else if (model_name == model_names::TWIT_nf_emot)
			return new Twitter_nf_emotions(parameters, construct);

		else if (model_name == model_names::TWIT_wf_emot)
			return new Twitter_wf_emotions(parameters, construct);

		return NULL;
	}

	Output* create_output(const std::string& output_name, const ParameterMap& parameters, Construct* construct) {

		if (output_name == output_names::output_graph) {
			return new Output_Graph(parameters, construct);
		}
		else if (output_name == output_names::output_dynetml) {
			return new Output_dynetml(parameters, construct);
		}
		else if (output_name == output_names::output_messages)
			return new Output_Messages(parameters, construct);

		//*************************************
		//*    add your custom output here    *
		//*************************************

		return NULL;
	}
}

void dynet::load_users(Social_Media_no_followers* media) {
	for (auto node = media->agents->begin(); node != media->agents->end(); ++node) {
		//custom users should be added at the vector index instead of the default user
		media->users[node->index] = new Social_Media_no_followers::default_media_user(media, node);
	}
}

void dynet::load_users(Social_Media_with_followers* media) {
	for (auto node = media->agents->begin(); node != media->agents->end(); ++node) {
		//custom users should be added at the vector index instead of the default user
		media->users[node->index] = new Social_Media_with_followers::default_media_user(media, node);
		media->Social_Media_no_followers::users[node->index] = media->users[node->index];
	}
}

void dynet::load_users(SM_nf_emotions* media) {
	for (auto node = media->agents->begin(); node != media->agents->end(); ++node) {
		media->users[node->index] = new SM_nf_emotions::default_media_user(media, node);
	}
}

void dynet::load_users(SM_wf_emotions* media) {
	for (auto node = media->Social_Media_no_followers::agents->begin(); node != media->Social_Media_no_followers::agents->end(); ++node) {
		media->Social_Media_with_followers::users[node->index] = new SM_wf_emotions::default_media_user(media, node);
		media->Social_Media_no_followers::users[node->index] = media->Social_Media_with_followers::users[node->index];
	}
}