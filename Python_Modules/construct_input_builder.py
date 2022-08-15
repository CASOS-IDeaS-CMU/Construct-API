from typing import List
from bs4 import BeautifulSoup, element

class ConstructBuilder:
    soup: BeautifulSoup
    parameters: element.Tag
    nodesets: element.Tag
    networks: element.Tag
    models: element.Tag
    outputs: element.Tag

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

    def add_node(self, nodeset: str, node_name: str) -> element.Tag:
        node = self.soup.new_tag('node')
        node.attrs['name'] = node_name
        nodeset.append(node)
        return node

    def add_node_attribute(self, node: element.Tag, att_name: str, att_val: str) -> None:
        att = self.soup.new_tag('attribute')
        att.attrs['name'] = att_name
        att.attrs['value'] = att_val
        node.append(att)

    def add_network(self, network_name: str, edge_type: str, source_nodeset: str, target_nodeset: str, slice_nodeset = '', src_rep = 'dense', trg_rep = 'dense', slc_rep = 'dense', default_value = '0') -> element.Tag:
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

        if slice_nodeset and not slice_nodeset.isspace():
            slc = self.soup.new_tag('slice')
            slc.attrs['nodeset'] = slice_nodeset
            slc.attrs['representation'] = slc_rep
            network.append(slc)

        self.networks.append(network)
        return network

    def add_network_link(self, network: element.Tag, src_index: str, trg_index: str, slc_index = '', value = '') -> None:
        link = self.soup.new_tag('link')
        link.attrs['src_index'] = src_index
        link.attrs['trg_index'] = trg_index
        if slc_index and not slc_index.isspace():
            link.attrs['slc_index'] = slc_index
        if value and not value.isspace():
            link.attrs['value'] = value
        network.append(link)

    def add_network_generator(self, network: element.Tag, gen_type: str, param_names: List[str], param_values: List[str], src_begin = '', src_end = '', trg_begin = '', trg_end = '', slc_begin = '', slc_end = ''):
        gen = self.soup.new_tag('generator')
        gen.attrs['type'] = gen_type

        if (src_begin and not src_begin.isspace()) or (src_end and not src_end.isspace()):
            rows = self.soup.new_tag('rows')
            if src_begin and not src_begin.isspace():
                rows.attrs['begin'] = src_begin
            else:
                rows.attrs['begin'] = 'first'
            if src_end and not src_end.isspace():
                rows.attrs['end'] = src_end
            else:
                rows.attrs['end'] = 'last'
            gen.append(rows)

        if (trg_begin and trg_begin.isspace()) or (trg_end and not trg_end.isspace()):
            cols = self.soup.new_tag('cols')
            if trg_begin and not trg_begin.isspace():
                cols.attrs['begin'] = trg_begin
            else:
                cols.attrs['begin'] = 'first'
            if trg_end and not trg_end.isspace():
                cols.attrs['end'] = trg_end
            else:
                cols.attrs['end'] = 'last'
            gen.append(cols)

        if (slc_begin and slc_begin.isspace()) or (slc_end and not slc_end.isspace()):
            slcs = self.soup.new_tag('slcs')
            if slc_begin and not slc_begin.isspace():
                slcs.attrs['begin'] = slc_begin
            else:
                slcs.attrs['begin'] = 'first'
            if slc_end and not slc_end.isspace():
                slcs.attrs['end'] = slc_end
            else:
                slcs.attrs['end'] = 'last'
            gen.append(slcs)

        for (name, value) in zip(param_names, param_values):
            self.add_parameter(gen, name, value)

        network.append(gen)

    def add_model(self, model_name: str, param_names = [], param_values = []) -> None:
        model = self.soup.new_tag('model')
        model.attrs['name'] = model_name

        for (name, value) in zip(param_names, param_values):
            self.add_parameter(model, name, value)
        
        self.models.append(model)

    def add_output(self, output_name: str, param_names = [], param_values = []) -> None:
        output = self.soup.new_tag('output')
        output.attrs['type'] = output_name

        for (name, value) in zip(param_names, param_values):
            self.add_parameter(output, name, value)

        self.outputs.append(output)

    def write(self, file_name: str) -> None:
        with open(file_name, 'w') as f:
            f.write(str(self.soup.prettify()))




