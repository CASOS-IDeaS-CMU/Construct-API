# Isabel Murdock, 7/27/2023
#
# Generates an input xml file for running the Reddit model in Construct. This version of the input generation
# allows for more complicated inputs including good/bad actors and moderators. It also generates custom networks
# for user activity levels, knowledge, subreddit membership, etc. Consequently, it is more complicated to modify
# but offers greater flexibility and more realistic simulations. It is recommended to start with the 
# generate_input_file_simple.py script for learning how the input file is formatted before modifying this script.
#
# To run: python generate_input_file.py [seed]
# Note: seed should be an integer value
#
# Use the parameters below to change the applicable simulation inputs.


from reddit_model_input_functions import *

################# Use Parameters Below to Change the Simulation Inputs at High Level #################

### NODESET PARAMETERS
# agent parameters
num_users = 100
num_good_actors = 5
num_bad_actors = 5
include_moderators = True
# posting, commenting, and voting behaviors are set in get_user_session_behaviors()

# other nodeset parameters
num_subreddits = 10
subreddit_ban_threshold = 1
num_timesteps = 12*24*2
num_knowledges = 25 


### NETWORK PARAMETERS
# knowledge network
knowledge_density = 0.05
#knowledge type network
misinfo_prob = 0.25
# knowledge trust network is set in get_knowledge_and_trust_networks()
# user active time network is set in get_user_timeestep_network()
# subreddit membership network is set in get_user_subreddit_network() and add_moderator_subreddit_network()


### OUTPUT PARAMETERS
networks_to_output = 'knowledge network,knowledge trust network,knowledge type network,banned user network,user active time network,subreddit membership network'
#######################################################################################################


########## Modify Functions Below to Specialize Indiviudal User/Moderator Behaviors/Attributes ##########

### SUBREDDIT MEMBERSHIP FUNCTIONS

# returns: one subreddit (int) such that the probability of a subreddit being selected is proportional to its subreddit_popularity_scores
# parameters: 
#       - subreddit_count (int): # of subreddits to select from
#       - subreddit_popularity_scores (dict): mapping of subreddits to their respective popularity score (e.g., {0: 15, 1: 3, 2: 7, 3: 1, ..})
#       - total_scores (int): sum of all of the popularity scores
def proportional_select_subreddit(subreddit_count, subreddit_popularity_scores, total_scores):
    rand_val = np.random.uniform(0, total_scores)
    running_total = 0
    for subreddit in range(subreddit_count):
        if(rand_val >= running_total and rand_val < (running_total + subreddit_popularity_scores[subreddit])):
            return subreddit
        running_total += subreddit_popularity_scores[subreddit]
    assert(False)

# returns: mapping of users (ints) to the subreddits (set of ints) that they are subsribed to (e.g., {0: {2, 3, 5}, 1: {3, 5, 6}, ...})
# parameters: 
#       - agent_count (int): # of users to produce subreddit subscriptions for
#       - subreddit_count (int): # of subreddits to select from
def get_user_subreddit_network(agent_count: int, subreddit_count: int):
    # assign "popularity scores" to the subreddits - can change this to fit any desired distribution
    subreddit_popularity_scores = {}
    total_popularity_scores = 0
    for subreddit in range(subreddit_count):
       subreddit_popularity_scores[subreddit] = np.random.pareto(1.5) + 1
       total_popularity_scores += subreddit_popularity_scores[subreddit]

    # subscribe users to subreddits based on the num_subreddits_to_subscribe_to and the popularity scores assinged above
    num_subreddits_to_subscribe_to = 4
    subreddit_user_counter = {}
    user_to_subreddit_network = {}
    for user in range(agent_count):
        subreddit_set = set()
        while(len(subreddit_set) < num_subreddits_to_subscribe_to):
            subreddit_set.add(proportional_select_subreddit(subreddit_count, subreddit_popularity_scores, total_popularity_scores))
        for subreddit in subreddit_set:
            if(subreddit not in subreddit_user_counter):
                subreddit_user_counter[subreddit] = 0
            subreddit_user_counter[subreddit] += 1
        user_to_subreddit_network[user] = subreddit_set
    return user_to_subreddit_network

# returns: an updated mapping of agents (ints) to the subreddits (set of ints) that they are subscribed to after adding one 
#          moderator per subreddit to the mapping; since the moderators are just a special type of agent, they are stored in
#          the same mapping/network as the users, with indicies starting after the users and special actors
# parameters: 
#       - subreddit_count (int): # of subreddits to select from
#       - agent_count (int): # of agents already subscribed to subreddits
def add_moderator_subreddit_network(subreddit_count, agent_count, user_to_subreddit_network):
    for subreddit in range(subreddit_count):
        user_to_subreddit_network[agent_count + subreddit] = {subreddit}
    return user_to_subreddit_network


### ACTIVE TIME NETWORK FUNCTIONS

def get_duration_of_session(p):
    return np.random.geometric(p)

# returns: a timestep (int) for the start of a user's log on session such that timesteps at night (12-6am) are the least likely 
#          to be selected and timesteps falling between 6-9am are the most likely to be selected
# parameters: 
#       - start_of_day_timestep (int): timestep of the start of the day that the user is logging on
def get_starting_timestep_of_session(start_of_day_timestep):
    prob = np.random.uniform(0, 1)
    # select time between 12am-6am
    if(prob < 0.1): return start_of_day_timestep + random.choice(list(range(0, 12*6)))
    # select time between 6am-9am
    elif(prob < 0.3): return start_of_day_timestep + random.choice(list(range(12*6, 12*9)))
    # select time between 9am-12am
    elif(prob < 1): return start_of_day_timestep + random.choice(list(range(12*9, 12*24)))
    else: assert(False)

# returns: a timestep (int) for the day that the user is to log on for the given week - Sat-Mon are more likely to be selected
# parameters: 
#       - start_of_week_timestep (int): timestep of the start of the week that the user is logging on
def get_day_of_session(start_of_week_timestep):
    #week starts on Monday
    day_of_week_probabilities = [0.167, 0.125, 0.125, 0.125, 0.125, 0.167, 0.167]
    prob = np.random.uniform(0, 1)
    cumulative_probability = 0
    for day_index in range(len(day_of_week_probabilities)):
        cumulative_probability += day_of_week_probabilities[day_index]
        if(prob <= cumulative_probability):
            return start_of_week_timestep + (day_index*24*12)
    assert(False)

# returns: an updated mapping of users (ints) to sets of timesteps (ints) after adding a new log on session based on the 
#          parameters (e.g., {0: {4, 5}, 1: {3, 10}, 2: {7, 8, 9, 10, 11}, ...})
# parameters: 
#       - day_start (int): timestep of the start of the day that the user is logging on
#       - duration_p (float): parameter for the geometric distribution that determines how long the session will be
#       - time_count (int): last timestep of the simulation
#       - user (int): user that is logging on
#       - user_to_timestep_network (dict): the mapping of users to sets of timesteps they are active during that is updated
def add_active_session_to_network(day_start, duration_p, time_count, user, user_to_timestep_network):
    session_start = get_starting_timestep_of_session(day_start)
    duration = get_duration_of_session(duration_p)
    for timestep in range(session_start, session_start+duration):
        if(timestep < time_count):
            user_to_timestep_network[user].add(timestep)
    return user_to_timestep_network

# returns: a mapping of agents (ints) to the timesteps (set of ints) that they are active (i.e., logged on); this function facilitates 
#          users having different browsing behaviors in terms of session length (e.g., short vs long browsers) and session frequency 
#          (e.g., twice/day to once/month); moderators are active during all time period but this could be modified; timestep = 5 minutes
# parameters: 
#       - agent_count (int): # of agents to assign active timesteps for
#       - time_count (int): # of timesteps in the simulation (i.e., last time step of the simulation)
#       - moderator_count (int): # of moderators (equal to number of subreddits) in the simulation 
def get_active_timestep_network(agent_count: int, time_count: int, moderator_count: int):
    user_to_timestep_network = {}

    for user in range(agent_count):
        user_to_timestep_network[user] = set()

        # assign the user to a session duration class
        duration_class = np.random.uniform(0, 1)
        if(duration_class < 0.5): p = 0.9   #short-browser
        elif(duration_class < 0.75): p = 0.2   #medium-browser
        elif(duration_class < 1): p = 0.05  #long-browser
        else: assert(False)

        # draw a frequency class for the user and use it to set the timesteps the user is active
        freq = np.random.uniform(0, 1)
        # twice a day
        if(freq < 0.2): 
            for day_start in range(0, time_count, 12*24):
                for i in range(2):
                    add_active_session_to_network(day_start, p, time_count, user, user_to_timestep_network)
        # once a day
        elif(freq < 0.5): #0.5
            for day_start in range(0, time_count, 12*24):
                add_active_session_to_network(day_start, p, time_count, user, user_to_timestep_network)
        # twice a week
        elif(freq < 0.6):
            for week_start in range(0, time_count, 12*24*7):
                for i in range(2):
                    day_start = get_day_of_session(week_start)
                    add_active_session_to_network(day_start, p, time_count, user, user_to_timestep_network)
        # once a week
        elif(freq < 0.8):
            for week_start in range(0, time_count, 12*24*7):
                day_start = get_day_of_session(week_start)
                add_active_session_to_network(day_start, p, time_count, user, user_to_timestep_network)
        # once a month
        elif(freq < 1):
            for month_start in range(0, time_count, 12*24*7*4):
                week_start = random.choice(list(range(month_start, month_start + 12*24*7*4, 12*24*7)))
                day_start = get_day_of_session(week_start)
                add_active_session_to_network(day_start, p, time_count, user, user_to_timestep_network)
        else:
            assert(False)

    # each moderator is active for all timesteps
    for moderator_index in range(agent_count, agent_count + moderator_count):
        user_to_timestep_network[moderator_index] = set(list(range(time_count)))

    return user_to_timestep_network


### USER BEHAVIOR FUNCTION

# returns: a mapping of agents (ints) to their probabilities of posting, commenting, and voting (list of floats); mappings are 
#          separated by users, good actors, and bad actors; user behaviors are assigned such that only 10% of users make posts,
#          35% make comments, 60% vote, and the rest only read posts, additionally, voting is more common than posting or commenting 
# parameters: 
#       - user_count (int): # of users to assign behaviors for 
#       - good_actor_count (int): # of good actors to assign behaviors for
#       - bad_actor_count (int): # of bad actors to assign behaviors for
def get_user_session_behaviors(user_count: int, good_actor_count: int, bad_actor_count: int):
    user_to_behavior_vals = {'user': {}, 'good': {}, 'bad': {}}
    for user in range(user_count):
        user_behavior_vals = [0, 0, 0, 0]   #post, comment, upvote, downvote
        if(user < 0.15 * user_count): user_behavior_vals[0] = 0.1
        if(user < 0.35 * user_count): user_behavior_vals[1] = 0.1
        if(user < 0.6 * user_count): user_behavior_vals[2] = 0.15
        if(user < 0.6 * user_count): user_behavior_vals[3] = 0.05
        user_to_behavior_vals['user'][user] = user_behavior_vals

    for good_actor in range(good_actor_count):
        user_to_behavior_vals['good'][good_actor] = [0.1, 0.1, 0.15, 0.05]
    for bad_actor in range(bad_actor_count):
        user_to_behavior_vals['bad'][bad_actor] = [0.1, 0.1, 0.15, 0.05]

    return user_to_behavior_vals


### KNOWLEDGE NETWORK AND KNOWLEDGE TRUST NETWORK FUNCTIONS

# returns: 
#       - the user (int) x knowledge (set of ints) network in the form of a dict
#       - the user (int) x knowledge trust (dict of ints to floats) network in the form of a dict

#          
# parameters: 
#       - user_count (int): # of users to assign behaviors for 
#       - good_actor_count (int): # of good actors to assign behaviors for
#       - bad_actor_count (int): # of bad actors to assign behaviors for
def get_knowledge_and_trust_networks(user_count: int, good_actor_count: int, bad_actor_count: int, knowledge_count: int, k_chance: float, misinfo_prob: float, moderator_count: int):
    knowledge_type_net = {}
    for knowledge in range(knowledge_count):
        knowledge_type_net[knowledge] = set()
        if(knowledge < (misinfo_prob * knowledge_count)): knowledge_type_net[knowledge].add("misinfo")

    knowledge_network_vals = {}
    knowledge_trust_vals = {}

    for user in range(user_count):
        knowledge_network_vals[user] = set()
        knowledge_trust_vals[user] = {}
        for knowledge in range(knowledge_count):
            if(np.random.uniform(0, 1) < k_chance):
                knowledge_network_vals[user].add(knowledge)
                knowledge_trust_vals[user][knowledge] = np.random.uniform(0.1, 0.9)

    #add good actor(s)
    for good_actor in range(user_count, user_count + num_good_actors):
        knowledge_network_vals[good_actor] = set()
        knowledge_trust_vals[good_actor] = {}
        for knowledge in range(knowledge_count):
            knowledge_network_vals[good_actor].add(knowledge)
            if('misinfo' in knowledge_type_net[knowledge]):
                knowledge_trust_vals[good_actor][knowledge] = 0
            else:
                knowledge_trust_vals[good_actor][knowledge] = 1

    #add bad actor(s)
    for bad_actor in range(user_count + num_good_actors, user_count + num_good_actors + num_bad_actors):
        knowledge_network_vals[bad_actor] = set()
        knowledge_trust_vals[bad_actor] = {}
        for knowledge in range(knowledge_count):
            knowledge_network_vals[bad_actor].add(knowledge)
            if('misinfo' in knowledge_type_net[knowledge]):
                knowledge_trust_vals[bad_actor][knowledge] = 1
            else:
                knowledge_trust_vals[bad_actor][knowledge] = 0

    agent_count = user_count + num_good_actors + num_bad_actors 
    for moderator in range(agent_count, agent_count + moderator_count):
        knowledge_network_vals[moderator] = set()
        knowledge_trust_vals[moderator] = {}
        for knowledge in range(knowledge_count):
                knowledge_network_vals[moderator].add(knowledge)
                if('misinfo' in knowledge_type_net[knowledge]):
                    knowledge_trust_vals[moderator][knowledge] = 0
                else:
                    knowledge_trust_vals[moderator][knowledge] = 1

    return (knowledge_network_vals, knowledge_trust_vals, knowledge_type_net)

#######################################################################################################


seed = validate_input()

num_agents = num_users + num_good_actors + num_bad_actors
subreddit_membership_network = get_user_subreddit_network(num_agents, num_subreddits)

moderator_count = num_subreddits
subreddit_membership_network = add_moderator_subreddit_network(num_subreddits, num_agents, subreddit_membership_network)

active_timestep_network = get_active_timestep_network(num_agents, num_timesteps, moderator_count)
user_session_behaviors = get_user_session_behaviors(num_users, num_good_actors, num_bad_actors)
(knowledge_network_vals, knowledge_trust_vals, knowledge_type_vals) = get_knowledge_and_trust_networks(num_users, num_good_actors, num_bad_actors, num_knowledges, knowledge_density, misinfo_prob, moderator_count)


# build the input XML file based on the generated nodesets and networks
construct = ConstructBuilder()

# construct parameters
construct.add_construct_parameter("seed", str(seed))
construct.add_construct_parameter("verbose initialization", "false")
construct.add_construct_parameter("verbose runtime", "false")

# models
construct.add_model('Reddit Interaction Model', ['interval time duration', 'maximum post inactivity', 'maximum percent learnable'], [str(1), str(num_timesteps), str(0.7)])
construct.add_model('Knowledge Trust Model', ['relax rate'], ['0.1'])


# nodesets
agent_xml = construct.add_nodeset('agent')
for i in range(num_users): add_agent_node(construct,  "agent_" + str(i), agent_xml, user_session_behaviors['user'][i], "user")
for i in range(num_good_actors): add_agent_node(construct, "agent_" + str(i + num_users), agent_xml, user_session_behaviors['good'][i], "user", receive_ktrust="false")
for i in range(num_bad_actors): add_agent_node(construct, "agent_" + str(i + num_users + num_good_actors), agent_xml, user_session_behaviors['bad'][i], "user", receive_ktrust="false")

subreddit_counter = 0
for i in range(num_agents, len(subreddit_membership_network)): 
    if(include_moderators): add_agent_node(construct, "agent_" + str(i), agent_xml, [0, 0, 0, 0], "moderator", moderation_delay=6, moderation_threshold=0.5)
    #setting moderation_threshold > 1, means no content will be removed - effectively, there will be no moderators
    else: add_agent_node(construct, "agent_" + str(i+1), agent_xml, [0, 0, 0, 0], "moderator", moderation_delay=1, moderation_threshold=1.1)  

#add subreddits with different banned_thresholds
subreddit_xml = construct.add_nodeset('subreddit')
for i in range(int(num_subreddits/2)): add_subreddit_node(construct, "subreddit_" + str(i), subreddit_xml, banned_threshold=1)
for i in range(int(num_subreddits/2), num_subreddits): add_subreddit_node(construct, "subreddit_" + str(i), subreddit_xml, banned_threshold=3)


knowledge_type_xml = construct.add_nodeset('knowledge_type')
construct.add_node(knowledge_type_xml, 'misinfo')

add_attributeless_nodeset(construct, 'knowledge', num_knowledges)
add_attributeless_nodeset(construct, 'medium', 1)
add_attributeless_nodeset(construct, 'time', num_timesteps)


# networks - rather than using Construct's built in network generators, we use add_bool_links() and add_float_links() to list each link in the networks
knowledge_net = construct.add_network('knowledge network', 'bool', 'agent', 'knowledge', trg_rep = 'sparse', default_value="false")
add_bool_links(construct, knowledge_net, knowledge_network_vals, "agent_", "knowledge_")

trust_net = construct.add_network('knowledge trust network', 'float', 'agent', 'knowledge', trg_rep = 'sparse', default_value="0.5")
add_float_links(construct, trust_net, knowledge_trust_vals, "agent_", "knowledge_")

active_net = construct.add_network('user active time network', 'bool', 'agent', 'time', trg_rep = 'sparse')
add_bool_links(construct, active_net, active_timestep_network, "agent_", "time_")

subreddit_net = construct.add_network('subreddit membership network', 'bool', 'agent', 'subreddit', trg_rep ='sparse')
add_bool_links(construct, subreddit_net, subreddit_membership_network, "agent_", "subreddit_")

knowledge_type_net = construct.add_network('knowledge type network', 'bool', 'knowledge', 'knowledge_type', trg_rep = 'dense')
construct.add_network_generator(knowledge_type_net, 'random binary', ['density'], ['0'])
add_bool_links(construct, knowledge_type_net, knowledge_type_vals, "knowledge_", "")


#outputs
construct.add_output('dynetml', ['network names', 'output file', 'timeperiods'], [networks_to_output, 'networks_output_{0}.xml'.format(seed), 'all'])
construct.add_output('posts',['output file'], ['posts_output_{0}.csv'.format(seed)])


print("Writing construct input xml file to: " + "construct_input_{0}.xml".format(seed))
construct.write("construct_input_{0}.xml".format(seed))