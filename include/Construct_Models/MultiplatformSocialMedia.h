#pragma once
#include "pch.h"
#include "SocialMedia.h"
//#include "Reddit.h"

namespace model_parameters {
    const std::string platforms = "platform model names"; //platform model names
    const std::string ban_thresholds = "ban thresholds"; //ban thresholds
    const std::string moderation_thresholds = "moderation thresholds"; //moderation thresholds
    const std::string platform_switching_preference = "platform switching preference"; //platform switching preference
    const std::string platform_switching_probability = "platform switching probability"; //platform switching probability
}

namespace dynet {
    struct parameter_size_mismatch : public construct_exception {
        parameter_size_mismatch(const std::string& primary_parameter_name, unsigned int primary_size, const std::string& secondary_parameter_name, unsigned int secondary_size) :
        construct_exception("Parameter vector size of " + std::to_string(primary_size) + " for parameter \"" + primary_parameter_name + "\" does not match " +
                            "the vector size of " + std::to_string(secondary_size) + " for parameter \"" + secondary_parameter_name + "\"") {}
    };
}

namespace node_attributes {
    const std::string moderated = "moderated"; //moderated
}

struct Social_Media_Moderation : public Model {
    struct Platform {
        Social_Media_no_followers* platform = nullptr;
        std::vector<unsigned int> removal_count;
        float moderation_threshold = 0.0f;
        unsigned int ban_threshold = 0u;
    };

    const Nodeset& knowledge = ns_manager.get_nodeset(nodeset_names::knowledge);
    std::vector<std::string> platform_names;
    std::vector<Platform> moderated_platforms;

    Social_Media_Moderation(const dynet::ParameterMap& parameters, Construct& construct);

    void initialize() override;

    virtual void moderation_response(unsigned int user_index, Social_Media_no_followers* platform) {}
    
    bool intercept(InteractionItem& item, unsigned int sender, unsigned int receiver, const CommunicationMedium* medium) override;

    virtual bool should_moderate_user(InteractionItem& item, unsigned int sender, Social_Media_no_followers* platform, float moderation_threshold);
};

struct Multiplatform_Manager : public Social_Media_Moderation {

    std::vector<float> platform_preference_weights;
    std::vector<Graph<bool>*> platform_active_networks;
    std::vector<float> platform_switch_probabilities;

    Multiplatform_Manager(const dynet::ParameterMap& parameters, Construct& construct);

    void initialize() override;

    void moderation_response(unsigned int user_index, Social_Media_no_followers* platform) override;

};
//*python->model_end_include
//
//<summary>
//Specializes the Reddit model for multiplatform activity and responses to moderation.
//<para>
//Provides a wrapper for the Reddit model to facillitate logging on to multiple platforms and changing users' platform preferences based on moderation.
//</para>
//</summary>
//*/
//struct Multiplatform_Reddit : public virtual Reddit {
//    Multiplatform_Reddit(const dynet::ParameterMap& parameters, Construct& _construct);
//
//    //the integer id for this platform - should be unique for each of the multiplatform models and be greater than 0
//    unsigned int platform_id;
//
//    //the number of multiplatform models that are being used and that users may become active on
//    unsigned int num_platforms;
//
//    //the probability that a user switches to one of the other platforms for a given active timestep in the future when they are moderated
//    float platform_switch_pr;
//
//    //graph name - "active agents by platform"
//    //agent x timeperiod
//    //edge weights are integers that specify which platform the user is active on
//    Graph<int>* active_agents_by_platform;
//    
//
//    //loads the active agents by platform network and initializes the model using Reddit::initialize(), then uses the active 
//    //agents by platform network to set the values for the Reddit model's active agents network
//    void initialize(void) override;
//
//    //uses the active agents by platform network to determine if the user is active on this platform for this timestep
//    //needed since users' platform usage can change in response to moderation
//    bool platform_active(unsigned int user_index, unsigned int current_time) override;
//
//    //for all future active steps of the given user when they would have logged onto this platform, they switch to one of the
//    //other platforms with probability equal to platform switch probability.
//    void moderation_response(unsigned int user_index, unsigned int current_time) override;
//};
//
//<summary>
//Specializes the Twitter_wf_wmod model for multiplatform activity and responses to moderation.
//<para>
//Provides a wrapper for the Twitter_wf_wmod model to facillitate logging on to multiple platforms and changing users' platform preferences based on moderation.
//</para>
//</summary>
//*/
//struct Multiplatform_Twitter : public virtual Twitter_wf_wmod {
//    Multiplatform_Twitter(const dynet::ParameterMap& parameters, Construct& _construct);
//
//    //the integer id for this platform - should be unique for each of the multiplatform models and be greater than 0
//    unsigned int platform_id;
//
//    //the number of multiplatform models that are being used and that users may become active on
//    unsigned int num_platforms;
//
//    //the probability that a user switches to one of the other platforms for a given active timestep in the future when they are moderated
//    float platform_switch_pr;
//
//    //graph name - "active agents by platform"
//    //agent x timeperiod
//    //edge weights are integers that specify which platform the user is active on
//    Graph<int>* active_agents_by_platform;
//    
//    //loads the active agents by platform network and initializes the model using Twitter_wf_wmod::initialize(), then uses the active 
//    //agents by platform network to set the values for the Twitter model's active agents network
//    void initialize(void) override;
//
//    //uses the active agents by platform network to determine if the user is active on this platform for this timestep
//    //needed since users' platform usage can change in response to moderation
//    bool platform_active(unsigned int user_index, unsigned int current_time) override;
//
//    //for all future active steps of the given user when they would have logged onto this platform, they switch to one of the
//    //other platforms with probability equal to platform switch probability.
//    void moderation_response(unsigned int user_index, unsigned int current_time) override;
//};
