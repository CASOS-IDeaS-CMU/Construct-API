#ifndef SOCIAL_MEDIA_HEADER_GUARD
#define SOCIAL_MEDIA_HEADER_GUARD
#include "pch.h"

class Social_Media_no_followers;
struct Social_Media_with_followers;

//these functions are defined in Supp_Library.cpp
namespace dynet {
    void load_users(Social_Media_no_followers* media);
    void load_users(Social_Media_with_followers* media);
}

class CONSTRUCT_LIB Social_Media_no_followers : public virtual Model
{


    //model parameter name who's value gets entered into Social_Media_with_followers::dt
    const std::string interval_time_duration = "interval time duration";

    //model parameter name who's value gets entered into Social_Media_with_followers::age
    const std::string maximum_post_inactivity = "maximum post inactivity";
public:

    struct CONSTRUCT_LIB media_event {

        //this goes through the entire chain of events recursively and updates the last_used
        //to the most recent time. Source call is only on the root event.
        void update_last_used(float time);

        enum class event_type : char
        {
            post,
            repost,
            quote,
            reply
        };

        ~media_event();

        //this constructor is only for creating post events
        media_event(
            unsigned int        _user,
            unsigned int        _knowledge_index,
            float               _ktrust,
            float               _time_stamp
        );

        //this constructor will create a reply/quote to the submitted parent event
        media_event(
            event_type         _type,
            unsigned int        _user,
            float               _ktrust,
            float               _time_stamp,
            media_event* _event
        );

        //this constructor will create a repost which uses the same knowledge and trust as the submitted parent event 
        media_event(
            unsigned int _user,
            float        _time_stamp,
            media_event* _event
        );

        //default constructor does no operations
        media_event();

        media_event(const media_event& other);

        media_event& operator=(const media_event& other);

        //gets the size of the tree of events with this event at its root (minimum size of 1)
        unsigned int child_size(void);

        //the pointer of the event this event is replying to.
        //if equal to this, the event has no parent and is a post.
        media_event* parent_event = this;

        //the pointer of the event that is at the root of the event tree that this event is apart of.
        //if equal to this, the event is the root event for the tree.
        media_event* root_event = this;

        //set of indexes the event contains
        std::unordered_map<InteractionItem::item_keys, unsigned int> indexes;

        //set of values the event contains
        std::unordered_map<InteractionItem::item_keys, float> values;

        //list of reposts that have shared this event
        std::vector<media_event*> reposts;

        //list of replies to this event
        std::vector<media_event*> replies;

        //list of quotes to this event
        std::vector<media_event*> quotes;

        //list of users mentioned by this event
        std::vector<unsigned int> mentions;

        //agent index of the user that posted the event
        unsigned int user = 0;

        //the time that this event was created
        float time_stamp = -1;

        //the last time any event was added to the event tree this event is apart of
        //all events in the same event tree have equal values for last_used
        float last_used = -1;

        //how trending an event is
        float score = 1;

        bool operator== (const media_event& a) const { return score == a.score; }
        bool operator!= (const media_event& a) const { return score != a.score; }
        bool operator<= (const media_event& a) const { return score <= a.score; }
        bool operator>= (const media_event& a) const { return score >= a.score; }
        bool operator< (const media_event& a) const { return score < a.score; }
        bool operator> (const media_event& a) const { return score > a.score; }

        //what type of event this is i.e. "post","repost", "quote", or "reply"
        event_type type = event_type::post;

        void update_root(media_event* new_root);

        void check_consistency(void) const;
    };

    //class that contains all settings for a user as well as functions that dictates how each user interacts
    struct CONSTRUCT_LIB media_user {

        virtual ~media_user() { ; }

        // called before reply, quote, or repost, allows the user to parse an event before responding to it
        virtual void read(media_event* read_event) = 0;

        // when called, allows the user to reply to the submitted event
        virtual void reply(media_event* read_event) = 0;

        // when called, allows the user to quote the submitted event
        virtual void quote(media_event* read_event) = 0;

        // when called, allows the user to repost the submitted event
        virtual void repost(media_event* read_event) = 0;

        //generate post events for the time step, called during the think function
        virtual void generate_post_events(void) = 0;

        //number of events read each time step
        virtual unsigned int get_read_count(void) = 0;
    };

    struct CONSTRUCT_LIB default_media_user : public virtual media_user {

        default_media_user(Social_Media_no_followers* _media, Nodeset::iterator node);

        //the social media that this user is interacting with
        Social_Media_no_followers* media;

        //this user's agent index
        unsigned int id;

        //probability density to post pdtw * dt = average number of events in a time period
        float pdp;

        //probability to reply when an event or reply is read
        float pr;

        //probability to repost when an event is read
        float prp;

        //probability to quote when an event is read
        float pqu;

        //probability density to read events (time in hours) pdread*dt=average number of read messages in a time period.
        float pdread;

        //this reads the post given and performs any actions before the post is potentially responded to
        void read(media_event* _event);

        //this adds a reply to the post with probability equal to media_user::pr
        //if an event is created default_media_user::add_mentions is called on that event
        void reply(media_event* _event);

        //this adds a quote to the post with probability equal to media_user::prp
        //if an event is created media_user::add_mentions is called on that event
        void quote(media_event* _event);

        //this adds a repost to the post with probability equal to default_media_user::pqu
        //if an event is created default_media_user::add_mentions is called on that event
        void repost(media_event* _event);

        //user adds a number of post events based on default_media_user::pdp
        void generate_post_events(void);

        //number of events read each time step
        unsigned int get_read_count(void);

        //mentions are added to the event if the event is a post by randomly selecting a followee
        virtual void add_mentions(media_event* post);

        //gets the trust of the knowledge index
        //if the "Knowledge Trust %Model" is not active, -1 is returned.
        virtual float get_trust(unsigned int knowledge_index);

        // chooses which knowledge index to add to an event
        virtual unsigned int get_knowledge_selection(void);
    };

    class CONSTRUCT_LIB event_container : public std::list<media_event> {
        using std::list<media_event>::insert;
        using std::list<media_event>::push_back;
        using std::list<media_event>::emplace_back;
        using std::list<media_event>::emplace;
        using std::list<media_event>::swap;
        using std::list<media_event>::sort;
        using std::list<media_event>::reverse;
        using std::list<media_event>::merge;
    public:
        
        // by default removed_events at the beginning of the container have larger values of timestamp
        // thus the root event of a reply tree should be at the highest index of any other event in the reply tree
        std::list<media_event> removed_events;

        iterator erase(const const_iterator _Where) noexcept {
            removed_events.push_back(*_Where);
            removed_events.back().check_consistency();
            return list::erase(_Where);
        }

        iterator erase(const const_iterator _First, const const_iterator _Last) noexcept {
            for (auto temp = _First; temp != _Last; temp++) removed_events.push_back(*temp);
            return list::erase(_First, _Last);
        }

        void clear() noexcept {
            for (auto it = begin(); it != end(); ++it) {
                removed_events.push_back(*it);
            }
            list::clear();
        }
    };

    void check_list_order() const;

    //pointer to the "agent" nodeset
    const Nodeset* agents = ns_manager->get_nodeset(nodeset_names::agents);
    const Nodeset* knowledge = ns_manager->get_nodeset(nodeset_names::knowledge);

    //graph name - "knowledge trust network"
    //agent x knowledge
    Graph<float>* ktrust_net = nullptr;

    //graph name - "knowledge trust transactive memory network"
    //agent x agent x knowledge
    Graph<std::map<unsigned int, float> >* kttm = nullptr;

    //this is the medium used for all messages created by this model
    //it is intended to have unlimited complexity and avoid models that interact based on medium
    CommunicationMedium medium;

    //this key is added to messages created by this model for items that contain the feed index
    InteractionItem::item_keys event_key = InteractionItem::item_keys::twitter_event;

    // list of all current active events, all users can access this list
    // new events should be added to the front of this list
    event_container list_of_events;

    //contains each user's feed of events pseudo-sorted by priority, also contains user-centric event info like whether a event has been read
    std::vector<std::vector<media_event*> > users_feed;

    // how many posts each agent reads each time step
    std::vector<unsigned int> read_count;

    //the maximum time a post can exist without its tree being added to
    float age;

    //the time duration between time steps
    float dt;

    //current time period
    unsigned int& time;

    //prefix for some of the node attributes names parsed by the media_user class
    std::string media_name;

    //graphs already used by other models in construct

    //graph name - "knowledge network"
    //agent x knowledge
    const Graph<bool>& knowledge_net = graph_manager->load_required(graph_names::knowledge, agents, knowledge);

    //graph name - "agent active time network"
    //agent x timeperiod
    const Graph<bool>& active = graph_manager->load_optional(graph_names::active, true, agents, sparse, ns_manager->get_nodeset(nodeset_names::time), sparse);

    //list of users
    std::vector<media_user*> users;

    //Loads all nodesets and graphs for this model and checks to ensure all required node attributes are present
    //Loads the parameters "interval time duration" into dt and "maximum post inactivity" into age
    //Uses the API function create_social_media_user to populate Social_Media_with_followers::users
    Social_Media_no_followers(const std::string& _media_name, const dynet::ParameterMap& parameters, Construct* _construct);

    //delete all pointers in stored in the Social_Media_with_followers::users data structure
    virtual ~Social_Media_no_followers();

    virtual void load_users() {
        dynet::load_users(this);
    }

    //agents read events in their feeds starting with the first event
    //reading an event will create a message with all relavant knowledge and trust information in items along with the event's feed index
    //messages are sent from the read event's author to the reading user and uses a CommunicationMedium with maximum complexity
    void think(void);

    //adds the Knowledge Parsing %Model, and attempts to find and save the pointer for the Knowledge Trust %Model if it has been added to the model list
    void initialize(void);

    //only parses messages that have an attribute equal to Social_Media_no_followers::event_key for the feed position index corresponding to a media_event pointer
    //that pointer is then given to media_user::read and if the user already knows the knowledge the event is passed to media_user::(reply, quote, repost)
    void communicate(InteractionMessageQueue::iterator msg);

    //feeds are updated, the social media will recommend users to follow, and users can decide to unfollow other users
    void cleanup(void);

    //appends the array of InteractionItems based on the submitted event and the intended receiver of the message
    virtual void append_message(media_event* _event, InteractionMessage& msg);

    //updates each user's feeds with the new events created during the time step while also discarding read events from the feed
    //events are ordered by direct replies or mentions, events of followers, and all other events
    //within each category events are sorted based on media_event::score which is set to media_event::child_size * media_event::time_stamp
    //after the events have been organized stochastic shuffling is done on 10% of the feed to avoid a fully deterministic feed
    virtual void update_feeds(void);
};


struct CONSTRUCT_LIB Social_Media_with_followers : public virtual Social_Media_no_followers
{
    //class that contains all settings for a user as well as functions that dictates how each user interacts
    struct CONSTRUCT_LIB media_user : virtual public Social_Media_no_followers::media_user {

        //returns true if this user decides to follow an agent when called
        virtual bool follow_user(unsigned int alter_agent_index) = 0;

        //Returns true if this user decides to unfollow an agent when called
        virtual bool unfollow_user(unsigned int alter_agent_index) = 0;

        // An alter has decided to follow this user and can decide to reciprocate that following
        virtual bool respond_to_follow(unsigned int alter_agent_index) = 0;

        // How many alters should be considered each time step for recommendations
        virtual unsigned int consider_recommendations(void) = 0;

        // gets how charismatic a user is
        virtual float get_charisma() = 0;
    };

    struct CONSTRUCT_LIB default_media_user : public Social_Media_no_followers::default_media_user, public media_user  {

        default_media_user(Social_Media_with_followers* _media, Nodeset::iterator node);

        //the social media that this user is interacting with
        Social_Media_with_followers* media;

        //probability density to recommend followers (time in hours) pdaf * dt = average number of recommendations in a time period.
        float pdaf;

        //scale factor to determine number of removed followees
        float rf;

        //determines how likable someone's event is going to be.
        //people with more likable posts get more followers
        float charisma;

        //If true, this user, when added as a followee by another user, will automatically reciprocate followings
        bool auto_follow;

        //mentions are added to the event if the event is a post by randomly selecting a followee
        virtual void add_mentions(media_event* post);

        //returns true if this user decides to follow an agent when called
        bool follow_user(unsigned int alter_agent_index);

        //Returns true if this user decides to unfollow an agent when called
        bool unfollow_user(unsigned int alter_agent_index);

        // An alter has decided to follow this user and can decide to reciprocate that following
        bool respond_to_follow(unsigned int alter_agent_index);

        // How many alters should be considered each time step for recommendations
        unsigned int consider_recommendations(void);

        float get_charisma();
    };

    std::vector < std::vector<unsigned int> > responses;

    //graph name - deteremined by the media
    //agent x agent
    // if (follower_net->examine(i,j)) // agent i is following agent j
    Graph<bool>* follower_net = nullptr;

    //list of users
    std::vector<media_user*> users;

    bool disable_follower_recommendations = false;

    //Loads all nodesets and graphs for this model and checks to ensure all required node attributes are present
    //Loads the parameters "interval time duration" into dt and "maximum post inactivity" into age
    //Uses the API function create_social_media_user to populate Social_Media_with_followers::users
	Social_Media_with_followers(const std::string& _media_name, const dynet::ParameterMap& parameters, Construct* _construct);

    virtual void load_users() {
        dynet::load_users(this);
    }

    void communicate(InteractionMessageQueue::iterator msg);

    //feeds are updated, the social media will recommend users to follow, and users can decide to unfollow other users
    void cleanup(void);

    //computes the Jaccard Similarity in the follower network between the two agent indexes
    float follower_jaccard_similarity(unsigned int agent_i, unsigned int agent_j) const;

    //function gives each user recommended users to follow
    virtual void add_followees(void);

    //each users decides whether to unfollow any other user
    virtual void remove_followees(void);

    //updates each user's feeds with the new events created during the time step while also discarding read events from the feed
    //events are ordered by direct replies or mentions, events of followers, and all other events
    //within each category events are sorted based on media_event::score which is set to media_event::child_size * media_event::time_stamp
    //after the events have been organized stochastic shuffling is done on 10% of the feed to avoid a fully deterministic feed
    virtual void update_feeds(void);
};

struct CONSTRUCT_LIB Facebook : public virtual Social_Media_with_followers {
    Facebook(const dynet::ParameterMap& parameters, Construct* construct);
};


struct CONSTRUCT_LIB Twitter : public virtual Social_Media_with_followers {
    Twitter(const dynet::ParameterMap& parameters, Construct* construct);
};

// TWITTER_SIM_HH_H
#endif



