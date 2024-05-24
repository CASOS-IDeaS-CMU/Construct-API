#include "pch.h" 
#include "Supp_Library.h"

#include "Template.h"
#include "GrandInteraction.h"
#include "Location.h"
#include "Forget.h"
#include "KnowledgeLearningDifficulty.h"
#include "Mail.h"
#include "Subscription.h"
#include "KnowledgeTrust.h"
#include "SocialMedia.h"
#include "Emotions.h"
#include "Reddit.h"
#include "Output.h"
#include "MultiplatformSocialMedia.h"

//only edit the contents of each function and include any extra files needed



// each function here corresponds to a specific social media platform type
// to add custom media users to a model edit the appropriate function
namespace dynet {
	
	Social_Media_no_followers::media_user* load_user(Social_Media_no_followers* media, const Node& node) {
		// add custom users here

		// example of adding a custom media user
		// return new my_custom_user(media, node)

		return media->get_default_media_user(node);
	}

	Social_Media_no_followers::media_user* load_user(Social_Media_with_followers* media, const Node& node) {
		// add custom users here

		return media->get_default_media_user(node);
	}

	Social_Media_no_followers::media_user* load_user(SM_nf_emotions* media, const Node& node) {
		// add custom users here

		return media->get_default_media_user(node);
	}

	Social_Media_no_followers::media_user* load_user(SM_wf_emotions* media, const Node& node) {
		// add custom users here

		return media->get_default_media_user(node);
	}

	Social_Media_no_followers::media_user* load_user(Reddit* media, const Node& node) {
		// add custom users here
		
		return media->get_default_media_user(node);
	}

}

//both functions should be a sequence of if/esle if statements
//once the pointer is passed through the this function the relevant managers will
//take ownership and deconstruct the classes at the appropriate time
	
//It is expected that models and output will take in a ParameterMap and a Construct reference, however it is not strictly required.
//Models will require a Construct reference in order to have access to the loaded nodes, networks/graphs, other models, and the interaction queue.

Model * dynet::create_model(const std::string & model_name, const ParameterMap& parameters, Construct& construct, const std::string& version) {

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
		return dynet::load_users(new Twitter_nf(parameters, construct),
			static_cast<Social_Media_no_followers::media_user * (*)(Social_Media_no_followers*, const Node&)>(&dynet::load_user));

	else if (model_name == model_names::FB_nf)
		return dynet::load_users(new Facebook_nf(parameters, construct),
			static_cast<Social_Media_no_followers::media_user * (*)(Social_Media_no_followers*, const Node&)>(&dynet::load_user));

	else if (model_name == model_names::TWIT_wf)
		return dynet::load_users(new Twitter_wf(parameters, construct),
			static_cast<Social_Media_no_followers::media_user * (*)(Social_Media_with_followers*, const Node&)>(&dynet::load_user));

	else if (model_name == model_names::FB_wf)
		return dynet::load_users(new Facebook_wf(parameters, construct),
			static_cast<Social_Media_no_followers::media_user* (*)(Social_Media_with_followers*, const Node&)>(&dynet::load_user));

	else if (model_name == model_names::EMOT)
		return new Emotions(construct);

	else if (model_name == model_names::TWIT_nf_emot)
		return dynet::load_users(new Twitter_nf_emotions(parameters, construct), 
			static_cast<Social_Media_no_followers::media_user* (*)(SM_nf_emotions*,const Node&)>(&dynet::load_user));

	else if (model_name == model_names::FB_nf_emot)
		return dynet::load_users(new Facebook_nf_emotions(parameters, construct),
			static_cast<Social_Media_no_followers::media_user* (*)(SM_nf_emotions*, const Node&)>(&dynet::load_user));

	else if (model_name == model_names::TWIT_wf_emot)
		return dynet::load_users(new Twitter_wf_emotions(parameters, construct),
			static_cast<Social_Media_no_followers::media_user* (*)(SM_wf_emotions*, const Node&)>(&dynet::load_user));

	else if (model_name == model_names::FB_wf_emot)
		return dynet::load_users(new Facebook_wf_emotions(parameters, construct),
			static_cast<Social_Media_no_followers::media_user* (*)(SM_wf_emotions*, const Node&)>(&dynet::load_user));

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

	else if (model_name == model_names::REDDIT)
		return dynet::load_users(new Reddit(parameters, construct), 
			static_cast<Social_Media_no_followers::media_user* (*)(Reddit*,const Node&)>(&dynet::load_user));

	else if (model_name == model_names::MODERATION)
		return new Social_Media_Moderation(parameters, construct);

	else if (model_name == model_names::MULTI_MOD)
		return new Multiplatform_Manager(parameters, construct);

	//*************************************
	//*    add your custom model here     *
	//*************************************

	return NULL;
}

Output* dynet::create_output(const std::string& output_name, const ParameterMap& parameters, Construct& construct, const std::string& version) {

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