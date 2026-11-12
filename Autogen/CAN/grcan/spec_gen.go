// TEMPLATE: filler data showing the format. The DBC parser will replace this file
// with the real spec, and with a "Code generated ... DO NOT EDIT." header.
package grcan

var CAN = Spec{
	Schema: Schema{Version: 1},
	Buses: map[string]Bus{
		"Bus1": {ID: 0, FD: false},
		"Bus2": {ID: 1, FD: true, Comment: "A CAN FD bus"},
	},
	Nodes: map[string]Node{
		"ALL":            {GRID: Ptr[uint8](0), Comment: "Every GR node"},
		"Node1":          {GRID: Ptr[uint8](1)},
		"Node2":          {GRID: Ptr[uint8](2)},
		"ThirdPartyNode": {Comment: "A node we did not make"},
	},
	Messages: map[string]Message{
		"GR_Message": {
			MsgID:  Ptr[uint16](0x001),
			Length: 8,
			Routes: []Route{
				{Bus: "Bus1", Sender: "Node1", Receiver: "Node2", CANID: 0x100102, Extended: true},
				{Bus: "Bus2", Sender: "Node1", Receiver: "ALL", CANID: 0x100100, Extended: true, BRS: true},
			},
			Signals: map[string]Signal{
				"Scaled_Signal": {
					StartBit: 0, Length: 16, Order: LittleEndian, Type: Unsigned,
					Factor: 0.01, Offset: 0, Min: Ptr(0.0), Max: Ptr(655.35),
					Unit: "Volts", Comment: "A normal scaled signal",
				},
				"Enum_Signal": {
					StartBit: 16, Length: 8, Order: LittleEndian, Type: Unsigned,
					Factor: 1, Comment: "A signal with labels",
					Values: map[int64]string{0: "Off", 1: "On"},
				},
				"Reserved_1": {
					Reserved: true, StartBit: 24, Length: 40, Order: LittleEndian, Type: Unsigned, Factor: 1,
				},
			},
		},
		"Third_Party_Message": {
			Length: 8,
			Routes: []Route{
				{Bus: "Bus1", Sender: "ThirdPartyNode", Receiver: "Node1", CANID: 0x123, Extended: false},
			},
			Signals: map[string]Signal{
				"Big_Endian_Signal": {
					StartBit: 0, Length: 16, Order: BigEndian, Type: Signed,
					Factor: 0.1, Unit: "Amps",
				},
			},
		},
	},
}
