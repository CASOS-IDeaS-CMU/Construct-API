# Isabel Murdock, 8/2/2023
#
# This script calculates and prints the average percentage of users who know true and misinfo knowledge items at the end
# of the simulation runs stored in the "Network/Outputs" folder. It takes the average across all knowledge items and runs. 
#
# To use this script, the folder you specify below should contain a subfolder called "NetworkOutputs/" which contains the 
# network xml files produced by the simulation runs you wish to analyze.


########## Use Parameters Below to Change the Outputs Analyzed ##########

# update to be the folder which contains the simulation output files (network xml files)
folder = "folder_name"

# if want to exclude moderators from the analysis, update to be the starting agent index of the moderators
# if using the input scripts, the starting index of the moderators should be equal to the number of users in the simulation
index_mods_start = 1000

# number of timesteps in each simulation run
last_time_step = 14*24*12

########################################################################

import matplotlib.pyplot as plt
import numpy as np
import pandas as pd

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
    timestep_to_check_after = max(1, last_time_step - 24*12)
    
    misinfo_k = set() 

    for meta_network in xml_data:
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
                    
                    if(network_id == "knowledge type network"):
                        if(int(meta_network_time_period) == 0):
                            for link in list(network):
                                k_index = int(link.attrib['source'].split('_')[-1])
                                misinfo_k.add(k_index)
                    
                    if(network_id == "knowledge network"):
                        if(int(meta_network_time_period) > timestep_to_check_after):
                            misinfo_know = {}
                            info_know = {}
                            for link in list(network):
                                agent_index = int(link.attrib['source'].split('_')[-1])
                                if(agent_index >= num_agents_skip and agent_index < index_mods_start):
                                    k_index = int(link.attrib['target'].split('_')[-1])
                                    if(k_index in misinfo_k):
                                        if(k_index not in misinfo_know): misinfo_know[k_index] = 0
                                        misinfo_know[k_index] += 1
                                    else:
                                        if(k_index not in info_know): info_know[k_index] = 0
                                        info_know[k_index] += 1

    seconds = timer() - timer_start
    print('Finished parsing XML in ' + str(seconds) + ' seconds')

    return (info_know, misinfo_know)


all_knowledge_densities = []
all_knowledge_trust_densities = []  
all_info_nums = []
all_misinfo_nums = []
num_agents_skip = 0
    
print("Reading in network files:")
network_files = list(Path(folder + "/NetworkOutputs/").glob('*.xml'))
for file in network_files:
    print(file)
    file_index = int((str(file).split('_')[-1]).split('.')[0])
    (info_know, misinfo_know) = parse_xml(file, None)
    for k in info_know:
        all_info_nums.append(info_know[k])
    for k in misinfo_know:
        all_misinfo_nums.append(misinfo_know[k])

print("Average % of users who know a given true knowledge item:")
print((sum(all_info_nums)/len(all_info_nums)) * (100/(index_mods_start-num_agents_skip)))

print("Average % of users who know a given misinfo knowledge item:")
print((sum(all_misinfo_nums)/len(all_misinfo_nums)) * (100/(index_mods_start-num_agents_skip)))