# Isabel Murdock, 7/26/2023
#
# Generates an input xml file for running the Reddit model in Construct. This is a simpler version of 
# the generate_input_file.py script since it uses Construct's built-in network generators to generate
# the various input networks (e.g., subreddit membership network, knowledge network) and does not 
# implement moderators or good/bad actors. It is meant to give an easier starting point for generating
# Reddit model input files and can produce input files for simple experiments with the Reddit model.
#
# To run: python generate_input_file.py [seed]
# Note: seed should be an integer value

# Use the parameters below to change the applicable simulation inputs.


########## Use Parameters Below to Change the Simulation Inputs ##########

### NODESET PARAMETERS
# user parameters
num_users = 100
user_post_prob = 0.05
user_comment_prob = 0.1
user_upvote_prob = 0.15
user_downvote_prob = 0.05

# subreddit paramters
num_subreddits = 10
subreddit_ban_threshold = 1

# timestep paramters
num_timesteps = 12*24*2

# knowledge paramters 
num_knowledges = 25 


### NETWORK PARAMETERS
# knowledge network
knowledge_density = 0.05

# knowledge trust network
knowledge_trust_density = 0.05  
knowledge_trust_min = 0.1
knowledge_trust_max = 0.9

# user active time network
user_active_density = 0.05

# subreddit membership network
subreddit_membership_density = 0.1

#knowledge type network
misinfo_prob = 0.25


### OUTPUT PARAMETERS
networks_to_output = 'knowledge network,knowledge trust network,knowledge type network,banned user network,user active time network,subreddit membership network'

##########################################################################

from reddit_model_input_functions import *

seed = validate_input()
construct = ConstructBuilder()

# construct parameters
construct.add_construct_parameter("seed", str(seed))
construct.add_construct_parameter("verbose initialization", "false")
construct.add_construct_parameter("verbose runtime", "false")

# models
construct.add_model('Reddit Interaction Model',
                    ['interval time duration', 'maximum post inactivity', 'maximum percent learnable'],
                    [str(1), str(num_timesteps), str(0.7)])
construct.add_model('Knowledge Trust Model', ['relax rate'], ['0.1'])

# nodesets
agent_xml = construct.add_nodeset('agent')
for i in range(num_users): add_agent_node(construct, "agent_" + str(i), agent_xml, [user_post_prob, user_comment_prob, user_upvote_prob, user_downvote_prob], "user")

subreddit_xml = construct.add_nodeset('subreddit')
for i in range(int(num_subreddits)): add_subreddit_node(construct, "subreddit_" + str(i), subreddit_xml, banned_threshold=subreddit_ban_threshold)

knowledge_type_xml = construct.add_nodeset('knowledge_type')
construct.add_node(knowledge_type_xml, 'misinfo')

add_attributeless_nodeset(construct, 'knowledge', num_knowledges)
add_attributeless_nodeset(construct, 'medium', 1)
add_attributeless_nodeset(construct, 'time', num_timesteps)

# networks
knowledge_net = construct.add_network('knowledge network', 'bool', 'agent', 'knowledge', trg_rep = 'sparse', default_value="false")
construct.add_network_generator(knowledge_net, 'random binary', ['density'], [str(knowledge_density)]) 

# the knowledge trust network is optional
trust_net = construct.add_network('knowledge trust network', 'float', 'agent', 'knowledge', trg_rep = 'sparse', default_value='0.5')
construct.add_network_generator(trust_net, 'random uniform', ['density', 'min', 'max'], [str(knowledge_trust_density), str(knowledge_trust_min), str(knowledge_trust_max)])

active_net = construct.add_network('user active time network', 'bool', 'agent', 'time', trg_rep = 'sparse')
construct.add_network_generator(active_net, 'random binary', ['density'], [str(user_active_density)])

subreddit_net = construct.add_network('subreddit membership network', 'bool', 'agent', 'subreddit', trg_rep ='sparse')
construct.add_network_generator(subreddit_net, 'random binary', ['density'], [str(subreddit_membership_density)])

knowledge_type_net = construct.add_network('knowledge type network', 'bool', 'knowledge', 'knowledge_type', trg_rep = 'dense')
construct.add_network_generator(knowledge_type_net, 'random binary', ['density'], [str(misinfo_prob)])

#outputs
construct.add_output('dynetml',['network names', 'output file', 'timeperiods'],
                     [networks_to_output, 'networks_output_{0}.xml'.format(seed), 'all'])
construct.add_output('posts',['output file'], ['posts_output_{0}.csv'.format(seed)])

print("Writing construct input xml file to: " + "construct_input_{0}.xml".format(seed))
construct.write("construct_input_{0}.xml".format(seed))