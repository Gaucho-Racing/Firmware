# grcan

The validated, generated view of our CAN spec, as a Go package.
Generators read this package and never read the DBCs.

    DBCs -> parser -> grcan -> generators

- `types.go`: the shape of the data. Written by hand.
- `spec_gen.go`: the spec itself. Generated from the DBCs.

**Status:** `spec_gen.go` holds filler data until the DBC parser exists.

**Maps are unordered.** Go iterates them in random order.
Sort the keys before producing any output, or generated files will change on every run.

## The package
- Never edit `spec_gen.go` by hand.
- CI regenerates it and fails if it differs from what's committed.
- All data comes from the DBCs.
- `Schema.Version` changes when the format changes.

## Fields
- An empty string means unset.
- A `nil` pointer or map means unset or not applicable.
- Pointers are used only where zero is a real value, like `MsgID` 0x000 or `GRID` 0.

## Names
- Bus, node, message and signal names are valid identifiers.
- Signal names are unique within a message.
- A route's `Bus`, `Sender` and `Receiver` always exist in `Buses` and `Nodes`.

## Messages and routes
- Each message appears once, however many routes it has.
- All routes of a message share its layout and length.
- `MsgID == nil` means third-party.
- For GR messages, `CANID` = `sender << 20 | MsgID << 8 | receiver`.
- `CANID` excludes the DBC's extended flag. See `Extended`.
- `(Bus, CANID)` is unique.
- `Length` is legal for every bus the message is on.
- `BRS` is only true on FD buses.

## Signals
- Signals fit within the message's `Length`.
- Signals never overlap.
- `StartBit` is where the signal starts, counted from byte 0, bit 0.
- `Order == BigEndian` means flip the bytes after reading.
- Big-endian signals are byte-aligned, or exactly 1 bit.
- `Type == Float` is always 32 or 64 bits.
- Real value = `raw * Factor + Offset`.
- `Min` and `Max` are both set, or both `nil`.

## Reserved signals
- Marked `Reserved: true`.
- Not real data. Skip them, or treat them as padding.

## Not supported
- Mux. The parser rejects any DBC that uses it.

## Not guaranteed
- That the spec is correct. A wrong but self-consistent layout still passes.
- Arrays. Repeated signals like cells are flat.
- Named bit flags, unless the DBC defines them as 1-bit signals.
- Mapache's existing telemetry names.
- Messages not in the DBCs, like Mapache's TCM messages.
