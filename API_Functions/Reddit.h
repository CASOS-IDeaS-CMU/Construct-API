#ifndef REDDIT_HEADER_GUARD
#define REDDIT_HEADER_GUARD
#include "SocialMedia.h"

namespace model_names {
    const std::string REDDIT = "Reddit Interaction Model";                          // "Reddit Interaction Model"
}

namespace graph_names {
    const std::string subreddit_mem = "subreddit membership network";               // "subreddit membership network"
    const std::string k_type = "knowledge type network";					        // "knowledge type network"
    const std::string active_times = "user active time network";					// "user active time network"
    const std::string banned_user = "banned user network";					        // "banned user network"
}

namespace nodeset_names {
    const std::string subreddits = "subreddit";					                    // "subreddit"
    const std::string k_types = "knowledge_type";				    	            // "knowledge types"
    const std::string moderators = "moderator";                                     // "moderators
}

struct Reddit :public Social_Media_no_followers
{
public:
    Reddit(dynet::ParameterMap parameters, Construct* construct);

    //graph name - "subreddit membership network"
    //agent x subreddit
    //specifies the subreddit subscriptions for each agent
    const Graph<bool>& subreddit_network = graph_manager->load_required(graph_names::subreddit_mem, nodeset_names::agents, nodeset_names::subreddits);
    
    //graph name - "user active time network"
    //agent x time
    //specifies the timesteps that each agent is "logged on"
    const Graph<bool>& agent_active_network = graph_manager->load_optional(graph_names::active_times, nodeset_names::agents, nodeset_names::time);

    //graph name - "banned user network"
    //agent x subreddit
    //edge weight reflects the number of times a user's content is banned/removed in a given subreddit
    Graph<unsigned int>& banned_agent_network = graph_manager->load_optional(graph_names::banned_user, 0u, nodeset_names::agents, false, nodeset_names::subreddits, false);

    //graph name - "knowledge type network"
    //knowledge x knowledge_type
    //specifies whether a knowledge is misinformation or true information 
    const Graph<bool>& knowledge_type_network = graph_manager->load_required(graph_names::k_type, nodeset_names::knowledge, nodeset_names::k_types);

    //agents determine how many posts to read from their feeds starting with the first event
    //deciding to read an event creates a message with all relevant knowledge and trust information in items along with the event's feed index
    //messages are sent from the read event's author to the reading user and uses a CommunicationMedium with maximum complexity
    void think(void) override;

    //updates each user's feeds with the new events created during the time step while also discarding read events from the feed
    //events are ordered by comments made on the user's posts/comments first and then all other posts made in the user's subreddits
    //within each category events are sorted based on media_event::score which is calculated based on Reddit's previously public hot score equation
    //posts and comments that have been banned/removed by the moderators are excluded from the feeds
    void update_feeds(void) override;

    //creates the users/moderators as specified by the "Reddit user type" attribute of each agent in the input xml file
    void load_users(void) override;

    struct reddit_user : public Social_Media_no_followers::default_media_user
    {
        reddit_user(Reddit* _media, const Node& node);

        //user makes a new post based on their "Reddit post probability"
        void generate_post_events(void) override;

        //user reads the media_event and potentially the comments associated with it
        //they call reddit_user::upvote(), reddit_user::downvote(), reddit_user::comment(), and 
        //reddit_user::update_ktrust() on the media_event to interact with the post/comment and 
        //update their trust of the media_event's knowledge bit
        void read(Social_Media_no_followers::media_event* _event) override;

        //if the user is not banned from making posts/comments in the media_event's subreddit,
        //this adds a comment to the media_event with probability equal to reddit_user::pr
        void comment(Social_Media_no_followers::media_event* _event);
        
        //this upvotes on the media_event with probability equal to reddit_user::prp
        void upvote(Social_Media_no_followers::media_event* _event);
        
        //this downvotes on the media_event with probability equal to reddit_user::pqu
        void downvote(Social_Media_no_followers::media_event* _event);
        
        //if the user's "Reddit can receive knowledge trust" is true,
        //this updates the user's trust of the media_event's knowledge bit based on their 
        //current trust of the knowledge bit and the trust contained in the media_event
        void update_ktrust(Social_Media_no_followers::media_event* _event);

        //number of events read each time step (drawn from Poisson(reddit_user::pdread))
        unsigned int get_read_count(void) override;

        //for this user replying, quoting, and reposting is disabled 
        void reply(Social_Media_no_followers::media_event* _event) {}
        void quote(Social_Media_no_followers::media_event* _event) {}
        void repost(Social_Media_no_followers::media_event* _event) {}

    };

    struct reddit_moderator : public Social_Media_no_followers::default_media_user
    {
        reddit_moderator(Reddit* _media, const Node& node);

        //bans/removes media_events from the moderator's subreddits that contain a piece of knowledge that is labeled as 
        //misinformation, was posted moderation_delay timesteps prior, and has a knowledge trust value of >= moderation_threshold
        //also updates the banned_agent_network
        void generate_post_events(void);

        //currently not used, but could be implemented to moderators' trust in the knowledge could evolve over time
        void update_ktrust(Social_Media_no_followers::media_event* _event);

        //currently not used, but could limit the number of events reviewed each time step
        unsigned int get_read_count(void) override;

        //for moderators, replying, quoting, reposting, and reading is disabled 
        void reply(Social_Media_no_followers::media_event* _event) {}
        void quote(Social_Media_no_followers::media_event* _event) {}
        void repost(Social_Media_no_followers::media_event* _event) {}
        void read(Social_Media_no_followers::media_event* _event) {}
    };

private:
    //list of integers that stores the starting size of each subreddit
    std::vector<unsigned int> subreddit_sizes;
};


#endif
