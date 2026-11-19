# bus_id()     GRCAN_BUS_ID.h: every bus, by ID
# node_id()    GRCAN_NODE_ID.h: every GR node, by GRID. Third-party nodes have no GRID, so they're left out
# custom_id()  GRCAN_CUSTOM_ID.h: the CAN ID of every third-party message, by name
# enum()       renders a header that holds one C enum
# doc()        renders a doc comment
#
# Reads only the spec from GRCANparser.validate() and relies only on grcan/README.md.
# Every list is sorted, so the output never depends on the order of the DBCs.

from pathlib import Path

from GRCANparser import check, validate

INC_DIR = Path(__file__).resolve().parents[1] / 'Inc'


def bus_id(spec):
    entries = []
    for name, bus in sorted(spec['Buses'].items(), key=lambda item: (item[1]['ID'], item[0])):
        entries += [doc(bus['Comment'])] if bus['Comment'] else []
        entries.append(f'GRCAN_BUS_{name.upper()} = {bus["ID"]},')
    return enum('Bus ID enum header', 'GRCAN_BUS_ID_H', 'GRCAN_BUS_ID', entries, '/** GR CAN Bus IDs */\n')


def node_id(spec):
    gr_nodes = sorted((node['GRID'], name) for name, node in spec['Nodes'].items() if node['GRID'] is not None)
    return enum('GR ID enum header', 'GR_IDS_H', 'GRCAN_NODE_ID', [f'GRCAN_{name} = 0x{grid:02X},' for grid, name in gr_nodes])


def custom_id(spec):
    entries = []
    for name, message in sorted(spec['Messages'].items()):
        if message['MsgID'] is None:
            can_ids = {route['CANID'] for route in message['Routes']}
            check(len(can_ids) == 1, name, 'a third-party message needs the same CAN ID on every route')
            entries.append(f'{name.upper()}_CAN_ID = 0x{can_ids.pop():X},')
    return enum('Custom CAN ID header', 'CUSTOM_CAN_ID_H', 'GRCAN_CUSTOM_ID', entries)


def enum(banner, guard, name, entries, preamble=''):
    body = ''.join(f'\t{entry}\n' for entry in entries)
    return f'// Auto-generated {banner}\n#ifndef {guard}\n#define {guard}\n\n{preamble}typedef enum {{\n{body}}} {name};\n\n#endif // {guard}\n'


def doc(comment):
    return '/** ' + comment.replace('*/', '* /') + ' */'


if __name__ == '__main__':
    spec = validate()
    for file, render in [('GRCAN_BUS_ID.h', bus_id), ('GRCAN_NODE_ID.h', node_id), ('GRCAN_CUSTOM_ID.h', custom_id)]:
        (INC_DIR / file).write_text(render(spec))
