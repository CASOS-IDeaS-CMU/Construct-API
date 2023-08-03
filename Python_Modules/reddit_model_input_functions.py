from construct_input_builder import ConstructBuilder
import numpy as np
import random
import csv
import sys
import os

def add_attributeless_nodeset(construct, name: str, num_nodes: int):
    nodeset_xml = construct.add_nodeset(name)
    for i in range(num_nodes):
        construct.add_node(nodeset_xml, name + '_' + str(i))

def add_agent_node(construct, name: str, agent_xml, user_session_behaviors, reddit_user_type, receive_ktrust="true", moderation_delay=0, moderation_threshold=0):
    assert(reddit_user_type == "user" or reddit_user_type == "moderator")

    node_xml = construct.add_node(agent_xml, name)
    construct.add_node_attribute(node_xml, "Reddit add follower density", str(1))
    construct.add_node_attribute(node_xml, "Reddit auto follow", "true")
    construct.add_node_attribute(node_xml, "Reddit charisma", str(0.5))
    construct.add_node_attribute(node_xml, "Reddit post density", 1)
    construct.add_node_attribute(node_xml, "Reddit post probability", str(user_session_behaviors[0])) #probability to make a post
    construct.add_node_attribute(node_xml, "Reddit quote probability", str(user_session_behaviors[3])) #probability to down vote on post
    construct.add_node_attribute(node_xml, "Reddit reading density", str(10))  #Poisson(value) for # of posts to read
    construct.add_node_attribute(node_xml, "Reddit user type", reddit_user_type)
    construct.add_node_attribute(node_xml, "Reddit reply probability", str(user_session_behaviors[1]))  #probability to comment on post
    construct.add_node_attribute(node_xml, "Reddit repost probability", str(user_session_behaviors[2])) #probability to up vote on post
    construct.add_node_attribute(node_xml, "can send knowledge", "true")
    construct.add_node_attribute(node_xml, "can send knowledge trust", "true")
    construct.add_node_attribute(node_xml, "can receive knowledge", "true")
    construct.add_node_attribute(node_xml, "can receive knowledge trust", "false")

    if(reddit_user_type == "moderator"):
        construct.add_node_attribute(node_xml, "moderation_delay", str(moderation_delay))
        construct.add_node_attribute(node_xml, "moderation_threshold", str(moderation_threshold))
        construct.add_node_attribute(node_xml, "Reddit can receive knowledge trust", "false")
    else:
        construct.add_node_attribute(node_xml, "Reddit can receive knowledge trust", receive_ktrust)

def add_subreddit_node(construct, name: str, subreddit_xml, banned_threshold=0):
    node_xml = construct.add_node(subreddit_xml, name)
    construct.add_node_attribute(node_xml, "banned_threshold", banned_threshold)

def add_bool_links(construct, xml_network, data_network, src_prefix, target_prefix):
    for src_node in data_network:
        for target_node in data_network[src_node]:
            construct.add_network_link(xml_network, src_prefix + str(src_node), target_prefix + str(target_node), value="true")

def add_float_links(construct, xml_network, data_network, src_prefix, target_prefix):
    for src_node in data_network:
        for target_node in data_network[src_node]:
            construct.add_network_link(xml_network, src_prefix + str(src_node), target_prefix + str(target_node), value=str(round(data_network[src_node][target_node], 4)))

def validate_input():
    if(len(sys.argv) != 2): 
        print("Wrong number of arguments received. Expected: python generate_input_file_simple.py [seed]")
        exit()
    seed = int(sys.argv[1])
    return seed