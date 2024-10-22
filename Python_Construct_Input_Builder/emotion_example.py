import construct_input_builder as cb
import numpy as np

emotions = ["sad", "fear", "disgust", "anger", "surprise", "happiness"]

# how strongly should activity depend on emotional valence
activity_dependence_scale_factor = 2.0

activities = {"Twitter post density": {
                    "base": 0.139,              #sad	fear	disg	anger	surp	hap
                    "first order": np.array([	-0.1,	-0.1,	0.1,	0.2,	0.1,	0.1 ]) * activity_dependence_scale_factor,
                    "second order": None,
                    "transform": "exponential"}, 
              "Twitter read density": {
                    "base": 3.41},
                    #"first order": None, #np.array([	-0.1,	0.1,	-0.2,	0.1,	0.2,	0.0 ]) * activity_dependence_scale_factor,
                    #"second order": None}, 
              "Twitter repost probability": {
                    "base": 0.0476,             #sad	fear	disg	anger	surp	hap
                    "first order": np.array([	-0.1,	-0.1,	0.1,	0.2,	0.1,	0.1 ]) * activity_dependence_scale_factor,
                    "second order": None,
                    "transform": "sigmoid"}, 
              "Twitter reply probability": {
                    "base": 0.00452,            #sad	fear	disg	anger	surp	hap
                    "first order": np.array([	-0.1,	-0.1,	0.1,	0.2,	0.1,	0.1 ]) * activity_dependence_scale_factor,
                    "second order": None,
                    "transform": "sigmoid"}, 
              "Twitter quote probability": {
                    "base": 0.00225,            #sad	fear	disg	anger	surp	hap
                    "first order": np.array([	-0.1,	-0.1,	0.1,	0.2,	0.1,	0.1 ]) * activity_dependence_scale_factor,
                    "second order": None,
                    "transform": "sigmoid"}
              }
agent_count = 100

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
    
activity_ns = construct_xml.add_nodeset("activity")
for act, info in activities.items():
    activity_node = construct_xml.add_node(activity_ns, act)
    if "transform" in info:
        construct_xml.add_node_attribute(activity_node, {"enable emotional dependence": info["transform"]})
    
    
construct_xml.add_network("knowledge network", "bool", "agent", "knowledge", src_rep="sparse", trg_rep="sparse", default_value="true")
emot_net = construct_xml.add_network("emotion network", "float", "agent", "emotion", default_value="0.5")
emot_bc_bias = construct_xml.add_network("emotion broadcast bias network", "float", "agent", "emotion", src_rep="sparse", trg_rep="sparse")
construct_xml.add_network("emotion broadcast first order network", "float", "emotion", "emotion", src_rep="sparse", trg_rep="sparse")

emot_reading_net = construct_xml.add_network("emotion reading first order network", "float", "emotion", "emotion")
emot_reg_net = construct_xml.add_network("emotion regulation first order network", "float", "emotion", "emotion")
emot_reg_bias = construct_xml.add_network("emotion regulation bias network", "float", "agent", "emotion")
activity_net = construct_xml.add_network("agent activity weights network", "float", "agent", "activity")


read_net = construct_xml.add_network("first order Twitter read density emotion network", "float", "agent", "emotion")
rt_net = construct_xml.add_network("first order Twitter repost probability emotion network", "float", "agent", "emotion")
rp_net = construct_xml.add_network("first order Twitter reply probability emotion network", "float", "agent", "emotion")
qu_net = construct_xml.add_network("first order Twitter quote probability emotion network", "float", "agent", "emotion")

for i in range(agent_count):
    for act, info in activities.items():
        if "transform" in info:
            net = construct_xml.add_network("first order " + act + " emotion network", "float", "agent", "emotion")
            construct_xml.add_l
        