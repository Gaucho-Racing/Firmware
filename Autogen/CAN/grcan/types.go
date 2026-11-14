package grcan

// Spec is the whole CAN spec.
//
// Maps are unordered, and Go iterates them in random order. Sort the keys
// before producing any output, or generated files will change on every run.
type Spec struct {
	Schema   Schema
	Buses    map[string]Bus
	Nodes    map[string]Node
	Messages map[string]Message
}

type Schema struct {
	Version int
	Name    string
}

type Bus struct {
	ID      int
	FD      bool
	Comment string
}

type Node struct {
	GRID    *uint8 // nil if third-party
	Comment string
}

type Message struct {
	MsgID   *uint16 // nil if third-party
	Length  int
	Comment string
	Routes  []Route
	Signals map[string]Signal
}

type Route struct {
	Bus      string
	Sender   string
	Receiver string
	CANID    uint32
	Extended bool
	BRS      bool // CAN FD bit rate switch
}

type Signal struct {
	Reserved bool
	StartBit int
	Length   int
	Order    ByteOrder
	Type     ValueType
	Factor   float64
	Offset   float64
	Min      *float64
	Max      *float64
	Unit     string
	Comment  string
	Values   map[int64]string // nil if no labels
}

type ByteOrder int

const (
	LittleEndian ByteOrder = iota
	BigEndian
)

type ValueType int

const (
	Unsigned ValueType = iota
	Signed
	Float
	ASCII // one text character, stored like a uint8
)

func Ptr[T any](v T) *T { return &v }
