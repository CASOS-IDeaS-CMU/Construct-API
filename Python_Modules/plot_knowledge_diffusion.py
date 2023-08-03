# Isabel Murdock, 8/2/2023
#
# This script plots the diffusion of knowledge (fraction of users who learn a given knowledge item), across multiple simulation
# runs and all of the  knowledge items from each run. The 50th, 75th, 90th, and 100th percentiles are plotted and diffusion is 
# plotted for the first 24 hours after each knowledge item is posted for the first time. The code can be modified to change these
# display settings.
#
# To use this script, the folder you specify below (line 18) should contain 2 subfolders:
#   - "PostOutputs/" which should contain the post csv files produced by the simulation runs you wish to use for the plot
#   - "NetworkOutputs/" which should contain the network xml files produced by the simulation runs you wish to use for the plot
# The network xml files must contain the knowledge networks in order for them to be plotted. The xml files may also include
# other networks in addition to the knowledge networks.


########## Use Parameters Below to Change the Outputs Analyzed ##########

# update to be the folder which contains the simulation output files (post csv files & network xml files)
folder = "folder_name"

# if want to exclude moderators from the analysis, update to be the starting agent index of the moderators
# if using the input scripts, the starting index of the moderators should be equal to the number of users in the simulation
index_mods_start = 1000

# number of knowledge items in the simulations
num_k = 100

########################################################################

import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import json

from timeit import default_timer as timer
from lxml import etree as etree_lxml
from pathlib import Path


def sample_xml(file, opts):
    with open(file, opts) as xml:
        return xml.read()


def parse_xml(file, k_start_time = None):
    timer_start = timer()

    xml_as_bytes = sample_xml(file, 'rb')
    xml_data = etree_lxml.fromstring(xml_as_bytes)

    timestep_to_networks_dict = {}

    knowledge_density = {}
    knowledge_density_by_k = {}
    
    num_seed_users_by_k = {}
    for k in range(num_k):
        num_seed_users_by_k[k] = 0

    for meta_network in xml_data:
        assert(meta_network.tag == 'MetaNetwork')
        meta_network_id = meta_network.attrib['id']
        meta_network_time_period = meta_network.attrib['timePeriod']

        timestep_to_networks_dict[meta_network_time_period] = {'id': meta_network_id, 'nodesets': {}, 'networks':{}}            

        for entity in list(meta_network):
            if (entity.tag == 'nodes'): 
                for nodeset in list(entity):
                    nodeset_id = nodeset.attrib['id']
                    timestep_to_networks_dict[meta_network_time_period]['nodesets'][nodeset_id] = set()
                    for node in list(nodeset): timestep_to_networks_dict[meta_network_time_period]['nodesets'][nodeset_id].add(node.attrib['id'])
            elif (entity.tag == 'networks'):
                for network in list(entity):
                    source_nodeset = network.attrib['source']
                    target_nodeset = network.attrib['target']
                    network_id = network.attrib['id']

                    # find the average user x knowledge network density at each time step
                    if(k_start_time == None):
                        if(network_id == "knowledge network"):
                            num_agents = len(list(timestep_to_networks_dict[meta_network_time_period]['nodesets']['agent'])[num_agents_skip:index_mods_start])                   
                            num_knowledges = len(timestep_to_networks_dict[meta_network_time_period]['nodesets']['knowledge'])
                            num_links = 0
                            for link in list(network):
                                agent_index = int(link.attrib['source'].split('_')[-1])
                                if(agent_index >= num_agents_skip and agent_index < index_mods_start):
                                    num_links += 1
                            knowledge_density[int(meta_network_time_period)] = num_links/(num_agents*num_knowledges)
                    
                    # find the change in the fraction of users who know each piece of knowledge based on the time since the knowledge was first posted
                    else:
                        if(network_id == "knowledge network"):
                            
                            # collect the number of seed users for each knowledge item
                            if(int(meta_network_time_period) == 0):
                                for link in list(network):
                                    agent_index = int(link.attrib['source'].split('_')[-1])
                                    if(agent_index >= num_agents_skip and agent_index < index_mods_start):
                                        knowledge_index = int(link.attrib['target'].split('_')[-1])
                                        num_seed_users_by_k[knowledge_index] += 1    
                            
                            num_users = len(list(timestep_to_networks_dict[meta_network_time_period]['nodesets']['agent'])[num_agents_skip:index_mods_start])                   
                            knowledges = set()
                            k_densities = {}
                            for k in k_start_time:
                                if(k_start_time[k] <= int(meta_network_time_period) and k_start_time[k] < 2016 and int(meta_network_time_period) <= k_start_time[k] + 2016): #7 days
                                    knowledges.add(k)
                                    k_densities[k] = 0
                            num_knowledges = len(knowledges)

                            for link in list(network):
                                agent_index = int(link.attrib['source'].split('_')[-1])
                                if(agent_index >= num_agents_skip and agent_index < index_mods_start):
                                    knowledge_index = int(link.attrib['target'].split('_')[-1])
                                    if(knowledge_index in knowledges): k_densities[knowledge_index] += 1

                            for k in k_densities:
                                k_densities[k] = (k_densities[k] - num_seed_users_by_k[k])/num_users
                                start_time_diff = int(meta_network_time_period) - k_start_time[k]
                                if(k not in knowledge_density_by_k): knowledge_density_by_k[k] = {}
                                knowledge_density_by_k[k][start_time_diff] = k_densities[k]

    print('Finished parsing XML in ' + str(timer() - timer_start) + ' seconds')
    return (knowledge_density_by_k)


def get_starting_timestamps(df):
    start_index = {}
    for index, row in df.iterrows():
        k = int(row['knowledge'])
        timestep = int(row['timestep created'])
        if(k not in start_index):
            start_index[k] = timestep
        elif(timestep < start_index[k]):
            start_index[k] = timestep
    return start_index


def percentile_plot(all_densities, filename):
    time_step_keys = set()
    values_count = 0
    for trial in all_densities:
        for key in trial:
            time_step_keys.add(key)
            values_count += 1

    time_period_to_densities = {}
    added_values_count = 0
    window_width = 12*6 
    max_time = int(12*24)
    
    for time_step in list(range(0, max_time)):
        time_period_to_densities[time_step] = []

    for time_step in time_step_keys:
        sliding_window_start = int(max(0, time_step - 24))
        sliding_window_end = int(min(time_step + (window_width - 24), max_time))
        all_times_in_window = list(range(sliding_window_start, sliding_window_end))
        for trial in all_densities:
            if(time_step in trial):
                for time in all_times_in_window:
                    time_period_to_densities[time].append(trial[time_step] * 100) #to convert density to %

    time_period_to_avgs = {}
    time_period_to_stddev = {}
    time_period_to_percentiles = {0: {}, 10: {}, 25: {}, 50: {}, 75: {}, 90: {}, 100: {}}
    ordered_timesteps = list(time_period_to_densities.keys())
    ordered_timesteps.sort()
    for time_step in ordered_timesteps:
        if(len(time_period_to_densities[time_step]) > 0): 
            time_period_to_avgs[time_step] = np.average(time_period_to_densities[time_step])
            time_period_to_stddev[time_step] = np.std(time_period_to_densities[time_step])
            for percentile in [0, 10, 25, 50, 75, 90, 100]:
                time_period_to_percentiles[percentile][time_step] = np.percentile(time_period_to_densities[time_step], percentile)
    
    x = list(time_period_to_avgs.keys())
    y_est = np.array(list(time_period_to_avgs.values()))
    y_err = np.array(list(time_period_to_stddev.values()))

    fig, ax = plt.subplots()
    fig.set_figwidth(3)
    fig.set_figheight(4)
    
    #convert x to hours from timesteps:
    for index,val in enumerate(x):
        x[index] = val/12
        
    alphas = {0: 0.1, 25: 0.8, 50: 0.7, 75: 0.5, 90: 0.3, 100: 0.1}
    for percentile1, percentile2, label in [(0, 100, "100%"), (0, 90, "90%"), (0, 75, "75%"), (0, 50, "50%")]:
        y1 = np.array(list(time_period_to_percentiles[percentile1].values()))
        y2 = np.array(list(time_period_to_percentiles[percentile2].values()))
        ax.fill_between(x, y1, y2, alpha=alphas[percentile2], color="grey", label=label)
    
    plt.xlabel('Hours After Knowledge is First Posted')
    plt.ylabel('% of Users with the Knowledge')
    legend = ax.legend()
    plt.title("Simulation")
    plt.savefig(filename +'.jpg', dpi=600, transparent=True, bbox_inches='tight')
    plt.show()



all_knowledge_densities = []
num_agents_skip = 0

post_files = list(Path(folder + "/PostOutputs/").iterdir())
k_starting_index = {}
print("Reading in post files:")
for post_file in post_files:
    print(post_file)
    file_index = int((str(post_file).split('_')[-1]).split('.')[0])
    df = pd.read_csv(post_file, names=["current timestep", "timestep created", "user", "last used", "prev banned", "subreddit", "knowledge", "ktrust", "upvotes", "downvotes", "banned", "parent_event", "root_event", "id"], usecols=[1, 3, 5, 7, 9, 11, 13, 15, 17, 19, 21, 23, 25, 27])
    k_start_dict = get_starting_timestamps(df)
    k_starting_index[file_index] = k_start_dict
    

network_files = list(Path(folder + "/NetworkOutputs/").glob('*.xml'))
print("\nReading in network files:")
for file in network_files:
    print(file)
    file_index = int((str(file).split('_')[-1]).split('.')[0])
    (knowledge_density) = parse_xml(file, k_starting_index[file_index])
    for k in knowledge_density:
        all_knowledge_densities.append(knowledge_density[k])

percentile_plot(all_knowledge_densities, "knowledge_net_density_" + folder)
  
with open(folder + "_all_knowledge_densities.txt", 'w') as convert_file:
     convert_file.write(json.dumps(all_knowledge_densities))