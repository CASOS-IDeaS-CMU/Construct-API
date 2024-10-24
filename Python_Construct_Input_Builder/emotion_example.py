import construct_input_builder as cb
import numpy as np

emotions = ["sad", "fear", "disgust", "anger", "surprise", "happiness"]
emotion_network_output_filename = "emotion_network.xml"
twitter_events_output_filename = "emotion_event.json"
input_xml_filename = "construct.xml"

# how strongly should activity depend on emotional valence
activity_dependence_scale_factor = 2.0

activities = [{
        "name": "Twitter post density",
        "base": 0.139,              #sad	fear	disg	anger	surp	hap
        "first order": np.array([	-0.1,	-0.1,	0.1,	0.2,	0.1,	0.1 ]) * activity_dependence_scale_factor,
        "second order": None,
        "transform": "exponential"
    }, {
        "name": "Twitter read density",
        "base": 3.41
        #,"first order": None, #np.array([	-0.1,	0.1,	-0.2,	0.1,	0.2,	0.0 ]) * activity_dependence_scale_factor,
        #"second order": None,
        #"transform": "exponential
    }, {
        "name": "Twitter repost probability",
        "base": 0.0476,             #sad	fear	disg	anger	surp	hap
        "first order": np.array([	-0.1,	-0.1,	0.1,	0.2,	0.1,	0.1 ]) * activity_dependence_scale_factor,
        "second order": None,
        "transform": "sigmoid"
    }, { 
        "name": "Twitter reply probability",
        "base": 0.00452,            #sad	fear	disg	anger	surp	hap
        "first order": np.array([	-0.1,	-0.1,	0.1,	0.2,	0.1,	0.1 ]) * activity_dependence_scale_factor,
        "second order": None,
        "transform": "sigmoid"
    }, { 
        "name": "Twitter quote probability",
        "base": 0.00225,            #sad	fear	disg	anger	surp	hap
        "first order": np.array([	-0.1,	-0.1,	0.1,	0.2,	0.1,	0.1 ]) * activity_dependence_scale_factor,
        "second order": None,
        "transform": "sigmoid"
    }]
agent_count = 100

emot_broadcast_probabilities = np.array([0.1, 0.1, 0.1, 0.1, 0.1, 0.1])
attractors = np.array([0.2, 0.2, 0.1, 0.2, 0.1, 0.5])
decay_rates = np.array([3.0, 2.0, 1.0, 1.0, 0.25, 2.0])
emot_reading = [ 
    #sad    fear    disg    anger   surp    hap
    [2.0,	0.5,	0.5,	0.0,	1.0,	-2.0],  #sad 
    [0.0,	3.5,	0.5,	0.5,	1.0,	-2.0],  #fear
    [0.0,	0.0,	3.0,	1.0,	1.0,	-2.0],  #disg
    [0.0,	0.0,	1.0,	5.0,	1.0,	-2.0],  #anger
    [0.0,	0.0,	0.0,	0.0,	10.0,	0.0],   #surp
    [4.0,	-0.8,	-0.8,	-0.8,	1.0,	5.0]    #hap
]

deltat = 0.5
total_time = 96.0

agent_count = 100
knowledge_count = 1
time_count = int(total_time / deltat)
reading_scale = 0.0075

construct_xml = cb.ConstructBuilder()
construct_xml.add_parameter("verbose runtime", "true")

time_ns = construct_xml.add_nodeset("time")
for i in range(time_count):
    construct_xml.add_node(time_ns, f"time_{i}")
    
knowledge_ns = construct_xml.add_nodeset("knowledge")
for i in range(knowledge_count):
    construct_xml.add_node(knowledge_ns, f"knowledge_{i}")
    
emotion_ns = construct_xml.add_nodeset("emotion")
for i in emotions:
    construct_xml.add_node(emotion_ns, i)
    
agent_ns = construct_xml.add_nodeset("agent")
for i in range(agent_count):
    agent = construct_xml.add_node(agent_ns, f"agent_{i}")
    construct_xml.add_node_attribute(agent, {
        "can send knowledge": "true",
        "can receive knowledge": "true",
        "can send emotion": "true",
        "can receive emotion": "true"
        })
    

    
    
construct_xml.add_network("knowledge network", "bool", "agent", "knowledge", src_rep="sparse", trg_rep="sparse", default_value="true")
emot_net = construct_xml.add_network("emotion network", "float", "agent", "emotion")
for agent_index in range(agent_count):
    for emot_index, attractor in attractors.enumerate():
        construct_xml.add_network_link(emot_net, agent_index, emot_index, value = np.random.uniform(-.1, .1) + attractors)

emot_bc_bias = construct_xml.add_network("emotion broadcast bias network", "float", "agent", "emotion")
for agent_index in range(agent_count):
    for emot_index, value in emot_broadcast_probabilities.enumerate():
        construct_xml.add_network_link(emot_bc_bias, agent_index, emot_index, value = value)
construct_xml.add_network("emotion broadcast first order network", "float", "emotion", "emotion", src_rep="sparse", trg_rep="sparse")

emot_reading_net = construct_xml.add_network("emotion reading first order network", "float", "emotion", "emotion")
for (row, col), value in np.ndenumerate(emot_reading):
    construct_xml.add_network_link(emot_reading_net, row, col, value = value * reading_scale)

emot_attractors_net = construct_xml.add_network("emotion attractors network", "float", "agent", "emotion")
for agent_index in range(agent_count):
    for emot_index, value in attractors.enumerate():
        construct_xml.add_network_link(emot_attractors_net, agent_index, emot_index, value = value)
        
emot_decay_rate_net = construct_xml.add_network("emotion regulation network", "float", "agent", "emotion")
for agent_index in range(agent_count):
    for emot_index, value in decay_rates.enumerate():
        construct_xml.add_network_link(emot_decay_rate_net, agent_index, emot_index, value = value * deltat)

activity_ns = construct_xml.add_nodeset("activity")
for act, info in activities.items():
    activity_node = construct_xml.add_node(activity_ns, act)
    if "transform" in info:
        construct_xml.add_node_attribute(activity_node, {"enable emotional dependence": info["transform"]})
        
activity_net = construct_xml.add_network("agent activity weights network", "float", "agent", "activity")
base_activity = construct_xml.add_network("base activity rate network", "float", "agent", "activity")

for index, info in activities.enumerate():
    if "transform" in info:
        for agent_index in range(agent_count):
            construct_xml.add_network_link(base_activity, agent_index, index, value=info["base"])
        if "first order" in info and info["first order"] != None:
            net = construct_xml.add_network("first order " + info["name"] + " emotion network", "float", "agent", "emotion")
            for agent_index in range(agent_count):
                for emot, value in info["first order"].enumerate():
                    construct_xml.add_network_link(net, agent_index, emot, value=value)
        if "second order" in info and info["second order"] != None:
            net = construct_xml.add_network("second order " + info["name"] + " emotion network", "float", "agent", "emotion", "emotion")
            for agent_index in range(agent_count):
                for (row, col), value in np.ndenumerate(info["second order"]):
                    construct_xml.add_network_link(net, agent_index, row, col, value)
                    
construct_xml.add_model("Emotion Model")
construct_xml.add_model("Twitter Model", {"interval time duration": f"{deltat}", "maximum post inactivity": "24.0"})

construct_xml.add_output("dynetml", {"network names":"emotion network", "timeperiods": "all", "output file": emotion_network_output_filename})
construct_xml.add_output("events", {
                                    "model name": "Twitter Model", 
                                    "output file": twitter_events_output_filename, 
                                    "start time": "2024-09-11T00:00:00.000Z",
                                    "time conversion": f"{deltat * 3600}"})

construct_xml.write(input_xml_filename)
            
        