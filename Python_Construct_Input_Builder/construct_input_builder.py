from bs4 import BeautifulSoup, element

class ConstructBuilder:

    def __init__(self) -> None:
        self.soup = BeautifulSoup(features = 'xml')

        self.parameters = self.soup.new_tag('construct_parameters')
        self.soup.append(self.parameters)

        self.nodesets = self.soup.new_tag('nodesets')
        self.soup.append(self.nodesets)

        self.networks = self.soup.new_tag('networks')
        self.soup.append(self.networks)

        self.models = self.soup.new_tag('models')
        self.soup.append(self.models)

        self.outputs = self.soup.new_tag('outputs')
        self.soup.append(self.outputs)

    def add_parameter(self, construct_tag: element.Tag, name: str, value: str) -> None:
        param = self.soup.new_tag('parameter')
        param.attrs['name'] = name
        param.attrs['value'] = value
        construct_tag.append(param)

    def add_construct_parameter(self, name: str, value: str) -> None:
        self.add_parameter(self.parameters, name, value)

    def add_nodeset(self, nodeset_name: str) -> element.Tag:
        nodeset = self.soup.new_tag('nodeset')
        nodeset.attrs['name'] = nodeset_name
        self.nodesets.append(nodeset)
        return nodeset

    def add_node(self, nodeset: element.Tag, node_name: str) -> element.Tag:
        node = self.soup.new_tag('node')
        node.attrs['name'] = node_name
        nodeset.append(node)
        return node

    def add_node_attribute(self, node: element.Tag, attributes: dict[str, str] = {}) -> None:
        for att_name, att_val in attributes.items():
            att = self.soup.new_tag('attribute')
            att.attrs['name'] = att_name
            att.attrs['value'] = att_val
            node.append(att)

    def add_network(self, network_name: str, edge_type: str, source_nodeset: str, target_nodeset: str, slice_nodeset: str | None = None, src_rep = 'dense', trg_rep = 'dense', slc_rep = 'dense', default_value = '0') -> element.Tag:
        network = self.soup.new_tag('network')
        network.attrs['name'] = network_name
        network.attrs['edge_type'] = edge_type
        network.attrs['default'] = default_value

        src = self.soup.new_tag('source')
        src.attrs['nodeset'] = source_nodeset
        src.attrs['representation'] = src_rep
        network.append(src)

        trg = self.soup.new_tag('target')
        trg.attrs['nodeset'] = target_nodeset
        trg.attrs['representation'] = trg_rep
        network.append(trg)

        if slice_nodeset is not None:
            slc = self.soup.new_tag('slice')
            slc.attrs['nodeset'] = slice_nodeset
            slc.attrs['representation'] = slc_rep
            network.append(slc)

        self.networks.append(network)
        return network

    def add_network_link(self, network: element.Tag, src_index: str, trg_index: str, slc_index: str | None = None, value: str | None = None) -> None:
        link = self.soup.new_tag('link')
        link.attrs['src_index'] = src_index
        link.attrs['trg_index'] = trg_index
        if slc_index is not None:
            link.attrs['slc_index'] = slc_index
        if value is not None:
            link.attrs['value'] = value
        network.append(link)

    def add_network_generator(self, network: element.Tag, gen_type: str, params: dict[str, str] = {}, 
            src_begin: str | None = None, src_end: str | None = None, 
            trg_begin: str | None = None, trg_end: str | None = None, 
            slc_begin: str | None = None, slc_end: str | None = None):
        gen = self.soup.new_tag('generator')
        gen.attrs['type'] = gen_type

        for name, value in params.items():
            self.add_parameter(gen, name, value)

        def gen_parameter(att_name, att_val):
            if att_name is not None:
                self.add_parameter(gen, att_name, att_val)

        gen_parameter('src_begin', src_begin)
        gen_parameter('trg_begin', trg_begin)
        gen_parameter('slc_begin', slc_begin)
        gen_parameter('src_end', src_end)
        gen_parameter('trg_end', trg_end)
        gen_parameter('slc_end', slc_end)

        network.append(gen)

    def add_model(self, model_name: str, params: dict[str, str] = {}) -> None:
        model = self.soup.new_tag('model')
        model.attrs['name'] = model_name

        for name, value in params.items():
            self.add_parameter(model, name, value)
        
        self.models.append(model)

    def add_output(self, output_name: str, params: dict[str, str] = {}) -> None:
        output = self.soup.new_tag('output')
        output.attrs['type'] = output_name

        for name, value in params.items():
            self.add_parameter(output, name, value)

        self.outputs.append(output)

    def write(self, file_name: str) -> None:
        with open(file_name, 'w') as f:
            f.write(str(self.soup.prettify()))

if (__name__ == '__main__'):
    print('Creating a default Construct Example')

    xml_builder = ConstructBuilder()
    xml_builder.add_construct_parameter('verbose initialization', 'true')
    xml_builder.add_construct_parameter('verbose runtime', 'true')

    agent_nodeset = xml_builder.add_nodeset('agent')
    for i in range(50):
        agent_node = xml_builder.add_node(agent_nodeset, f'agent_{i}')
        xml_builder.add_node_attribute(agent_node, {'can send knowledge':'true', 'can receive knowledge':'true'})

    knowledge_nodeset = xml_builder.add_nodeset('knowledge')
    for i in range(20):
        xml_builder.add_node(knowledge_nodeset, f'knowledge_{i}')

    time_nodeset = xml_builder.add_nodeset('time')
    for i in range(10):
        xml_builder.add_node(time_nodeset, f'time_{i}')

    comm_nodeset = xml_builder.add_nodeset('medium')
    comm_node = xml_builder.add_node(comm_nodeset, 'default')
    xml_builder.add_node_attribute(comm_node, {'maximum message complexity':'1', 'maximum percent learnable':'1', 'time to send':'0'})

    knowledge_network = xml_builder.add_network('knowledge network', 'bool', 'agent', 'knowledge', trg_rep = 'sparse')
    xml_builder.add_network_generator(knowledge_network, 'random binary', {'density':'0.2'})

    soi_network = xml_builder.add_network('interaction sphere network', 'bool', 'agent', 'knowledge', trg_rep = 'sparse')
    xml_builder.add_network_generator(soi_network, 'random binary', {'density':'0.2'})

    xml_builder.add_model('Standard Interaction Model')
    xml_builder.add_output('dynetml', {'network names':'knowledge network', 'output file': 'construct_output.xml', 'time periods': 'all'})

    xml_builder.write('construct_example.xml')
    print('Construct example created in construct_example.xml')

