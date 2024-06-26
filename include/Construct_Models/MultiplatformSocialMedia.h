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