#include "pch.h"
#include "SocialMedia.h"

struct Reddit : public virtual Social_Media_no_followers {
    Reddit(const dynet::ParameterMap& parameters, Construct& _construct);

    //graph name - "subreddit membership network"
    //agent x subreddit
    //specifies the subreddit subscriptions for each agent
    const Graph<bool>& subreddit_network = graph_manager.load_required(graph_names::subreddit_mem, nodeset_names::agents, nodeset_names::subreddits);

    //graph name - "banned user network"
    //agent x subreddit
    //edge weight reflects the number of times a user's content is banned/removed in a given subreddit
    Graph<unsigned int>& banned_agent_network = graph_manager.load_optional(graph_names::banned_user, 0u, nodeset_names::agents, false, nodeset_names::subreddits, false);

    //graph name - "knowledge type network"
    //knowledge x knowledge type
    //specifies whether a knowledge is misinformation or true information 
    const Graph<bool>& knowledge_type_network = graph_manager.load_required(graph_names::k_type, nodeset_names::knowledge, nodeset_names::k_types);


    //adds the Knowledge Trust %Model, and attempts to find and save the pointer for the Knowledge Trust Transactive Memory %Model if it has been added to the model list
    //initializes reddit users and moderators as the users based on the Reddit user type attribute for each agent
    void initialize(void) override;

    int get_feed_priority(const media_event& _event, unsigned int user) override;

    void random_event_swapping(unsigned int) override {}

    void update_event_scores() override;

    void append_message(media_event* _event, InteractionMessage& msg) override;

#ifdef CUSTOM_REDDIT_USERS
    media_user* load_user(const Node& node);
#endif


    void load_users(const std::string& version) override {
        assert(Construct::version == version);

        for (auto& node : agents) {
#ifndef CUSTOM_REDDIT_USERS
            media_user* user = NULL;
            if (node["Reddit user type"] == "moderator") {
                user = new Reddit::reddit_moderator(this, node);
            }
            else if (node["Reddit user type"] == "user") {
                user = new Reddit::reddit_user(this, node);
            }
            else {
                assert(false);
            }
#endif
            users[node.index] =
#ifdef CUSTOM_REDDIT_USERS
                load_user(node);
#else
                user;
#endif
        }
    }

    //class that contains all settings for a user as well as functions that dictates how each user interacts
    struct reddit_user : public Social_Media_no_followers::media_user
    {
        reddit_user(Reddit* reddit, const Node& node);

        Reddit& media() {
            Reddit* temp = dynamic_cast<Reddit*>(media_ptr);
            // if the media couldn't be up casted the desired class this assertion will be raised.
            // If you're confused why you probably have a diamond inheritence that makes casting non-trivial
            assert(temp);
            return *temp;
        }

        //this user's agent index
        unsigned int id;

        //probability density to read events (time in hours) pdread*dt=average number of read messages in a time period.
        float pdread;

        //probability to create a post to a subreddit
        float pp;

        //probability to reply when an event or reply is read
        float pr;

        //probability to add an upvote to an event
        float up;

        //probability to add a downvote to an event
        float dp;

        //user makes a new post based on their "Reddit post probability"
        void generate_post_events(void);

        //initialize the reddit-specific fields of a media event
        void init_reddit_event_fields(media_event* _event, float ktrust, unsigned int subreddit, unsigned int prev_ban);

        //user reads the media_event and potentially the comments associated with it
        //calls reddit_user::upvote(), reddit_user::downvote(), reddit_user::comment(), and reddit_user::update_ktrust() 
        //on the media_event to interact with the post/comment and decide whether to read follow on comments
        void read(media_event* _event);

        //if the user is not banned from making posts/comments in the media_event's subreddit,
        //this adds a comment to the media_event with probability equal to reddit_user::pr
        void comment(media_event* _event);

        //this upvotes on the media_event with probability equal to reddit_user::up
        void upvote(media_event* _event);

        //this downvotes on the media_event with probability equal to reddit_user::dp
        void downvote(media_event* _event);

        //if the user's "Reddit can receive knowledge trust" is true,
        //this updates the user's trust of the media_event's knowledge bit based on their 
        //current trust of the knowledge bit and the trust contained in the media_event
        void update_ktrust(media_event* _event);

        //gets the trust of the knowledge index
        //if the "Knowledge Trust %Model" is not active, -1 is returned.
        virtual float get_trust(unsigned int knowledge_index);

        unsigned int get_read_count() override;

        //since commenting and voting is handled by the read() function in the Reddit model,
        //the inherited replying, quoting, and reposting functions are overwritten to do nothing

        void reply(media_event* _event) {}
        void quote(media_event* _event) {}
        void repost(media_event* _event) {}
        

    };

    //class that contains all settings for a moderator as well as functions that dictates how each moderator interacts
    struct reddit_moderator : public Social_Media_no_followers::media_user
    {
        reddit_moderator(Reddit* reddit, const Node& node);

        Reddit& media() {
            Reddit* temp = dynamic_cast<Reddit*>(media_ptr);
            // if the media couldn't be up casted the desired class this assertion will be raised.
            // If you're confused why you probably have a diamond inheritence that makes casting non-trivial
            assert(temp);
            return *temp;
        }

        float delay;

        float threshold;

        //this user's agent index
        unsigned int id;

        //bans/removes media_events from the moderator's subreddits that contain a piece of knowledge that is labeled as 
        //misinformation, was posted moderation_delay timesteps prior, and has a knowledge trust value of >= moderation_threshold
        //also updates the banned_agent_network
        void generate_post_events(void);

        //currently not used, but could limit the number of events reviewed each time step
        unsigned int get_read_count(void) { return 0; };

        //for moderators, posting, replying, quoting, reposting, and reading is disabled 
        // upvoting and downvoting is also disabled.
        //currently not used, but could be implemented for moderators to post warnings or comments

        void reply(media_event* _event) {}
        void quote(media_event* _event) {}
        void repost(media_event* _event) {}
        void read(media_event* _event) {}
    };

    std::vector<unsigned int> subreddit_banned_threshold;

private:
    //list of integers that stores the starting size of each subreddit
    std::vector<unsigned int> subreddit_sizes;
};