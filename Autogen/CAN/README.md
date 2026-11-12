# CANfigurator

#### By Aarnav, Caroline, Kartik, Nandini

## Source of Truth: The DBCs

The CAN spec lives in `DBC/`, one DBC per bus: `GRCAN_Primary.dbc`, `GRCAN_Data.dbc`, `GRCAN_Charger.dbc` and `GRCAN_Testing.dbc`. Testing has no messages yet. Edit them with a DBC editor or by hand. Nothing generates them.

The C headers in `Inc/` are still generated from `Doc/GRCAN.CANdo` until a DBC-based generator replaces it. **Until then, make every CAN change in both the DBC and CANdo.**

### Conventions

- Each route is its own `BO_` frame, named `<Sender>_<Message>_to_<Receiver>` and at most 32 characters. All frames of one message share its layout.
- A GR message's CAN ID is `sender GR ID << 20 | MSG ID << 8 | receiver GR ID`. IDs above `0x7FF` are extended and set `VFrameFormat`.
- Floating-point signals are marked with `SIG_VALTYPE_` (`1` = float, `2` = double).
- Text is one 8-bit unsigned signal per character, marked `GRValueType` `ASCII`. There is no terminator: a shorter frame carries fewer characters.
- Custom attributes hold what used to live only in CANdo:

| Attribute     | Set on       | Meaning                                                     |
| ------------- | ------------ | ----------------------------------------------------------- |
| `DBName`      | file         | Bus name                                                    |
| `GRBusID`     | file         | Bus ID                                                      |
| `GRID`        | `BU_` node   | GR ID. Third-party nodes have `0`, as in CANdo              |
| `GRMsgName`   | `BO_` frame  | Message name, the same on every frame of one message        |
| `GRMsgID`     | `BO_` frame  | MSG ID for GR messages, `-1` for third-party messages       |
| `GRValueType` | `SG_` signal | `ASCII` for a text character, else `Number` (the default)   |

When you add a frame, set `GRMsgName`, and `GRMsgID` if it's a GR message. When you add a node, set `GRID`.

### Not in the DBCs

- IMD isolation info, voltage, IT-system, request and response. CANdo gives them no length or signals.
- `mag_status` in Suspension IMU Mag Data. It sits past the end of the 24-byte frame.

---

## Workflow: How to Apply Changes

**Do not run the parsers locally.** The generated headers are produced by a GitHub Actions workflow.

1. Make the change in the DBC in `DBC/`.
2. Make the same change in `Doc/GRCAN.CANdo`. For a visual editor, use the [CANdo web viewer](https://gaucho-racing.github.io/Firmware/).
3. Commit and push to GitHub.
4. The CI workflow runs all parsers on every commit in any pull request and commits the regenerated headers automatically.
5. Pull the latest changes to get the updated headers locally.

---

## The Parsers

All parsers live in `Src/` and read `Doc/GRCAN.CANdo` as input. You should never need to run them manually.

- `**CANparser.pl`** — Reads the `Custom CAN ID` section. Produces `GRCAN_CUSTOM_ID.h`, a single enum mapping each third-party message name to its CAN ID.
- `**GRparser.pl**` — Reads the `GR ID` section. Produces `GRCAN_NODE_ID.h`, a single enum of device/node network addresses.
- `**MSGparser.pl**` — Reads the `Message ID` section. Produces `GRCAN_MSG_ID.h`, a single enum of every team-defined message name to its MSG ID.
- `**STRUCTparser.pl**` — Reads the `Message ID` section. Produces `GRCAN_MSG_DATA.h`, packed C structs for each message with typed fields, byte-level layout, and description comments.
- `**BusParser.pl**` — Reads the `Bus ID` section. Produces `GRCAN_BUS_ID.h`, an enum of logical CAN bus identifiers.


| Parser            | Section(s) Read                                   | Key Fields Consumed                                               | Output              |
| ----------------- | ------------------------------------------------- | ----------------------------------------------------------------- | ------------------- |
| `CANparser.pl`    | `Custom CAN ID`                                   | Message name, `CAN ID`                                            | `GRCAN_CUSTOM_ID.h` |
| `GRparser.pl`     | `GR ID`                                           | Device name, hex ID value                                         | `GRCAN_NODE_ID.h`   |
| `MSGparser.pl`    | `Message ID`                                      | Message name, `MSG ID`                                            | `GRCAN_MSG_ID.h`    |
| `STRUCTparser.pl` | `Message ID`                                      | Message name, fields, `bit_start`, `data type`, `comment:` blocks | `GRCAN_MSG_DATA.h`  |
| `BusParser.pl`    | `Bus ID`                                          | Bus name, numeric ID                                              | `GRCAN_BUS_ID.h`    |


---

## Modifying `GRCAN.CANdo`

`Doc/GRCAN.CANdo` generates the C headers until a DBC-based generator replaces it. The DBCs in `DBC/` are the source of truth, so keep the two in sync. CANdo is a YAML file with six top-level sections, consumed by the Perl parsers during the build.

### Top-Level Sections


| Section         | Consumed By                                                                 | Generated Output              |
| --------------- | --------------------------------------------------------------------------- | ----------------------------- |
| `Bus ID`        | `BusParser.pl` -> `GRCAN_BUS_ID.h`                                          | Logical bus ID enum           |
| `routing`       | *(not consumed by any parser; routing lives in the DBCs)*                   | None                          |
| `byte order`    | *(shared config, not consumed by any parser directly)*                      | Endianness reference          |
| `Message ID`    | `MSGparser.pl` -> `GRCAN_MSG_ID.h`, `STRUCTparser.pl` -> `GRCAN_MSG_DATA.h` | MSG ID enum, packed C structs |
| `Custom CAN ID` | `CANparser.pl` -> `GRCAN_CUSTOM_ID.h`                                       | Custom CAN ID enum            |
| `GR ID`         | `GRparser.pl` -> `GRCAN_NODE_ID.h`                                          | Device/node ID enum           |


---

### 1. `Bus ID` Section (parsed by `BusParser.pl`)

A flat mapping of logical bus names to their numeric identifiers.

```yaml
Bus ID:
  Testing: 0
  Primary: 1
  Data: 2
  Charger: 3
```

**To add a new bus:** add a line with a unique name and an unused integer ID.

---

### 2. `routing` Section

Defines which messages each device sends on each CAN bus, and to which receiver. Use `can_id_override` for messages that use a non-standard (externally defined) CAN ID.

```yaml
routing:
  messages:
    <Device Name>:
      <CAN Bus>:           # must match a name in the Bus ID section (e.g. Primary, Data, Charger)
        <Receiver Name>:
          - msg: <Message Name>
          - msg: <Message Name>
            can_id_override: 0x1806E5F4   # optional, for external protocols
```

**To add a new route:** nest a `- msg:` entry under the appropriate Device -> Bus -> Receiver path. The message name must match an entry in `Message ID` or `Custom CAN ID`.

---

### 3. `byte order` Section

A single value that sets the byte order for the entire bus. Currently `little_endian`.

```yaml
byte order: little_endian
```

---

### 4. `Message ID` Section (parsed by `MSGparser.pl` and `STRUCTparser.pl`)

Defines every CAN message the team controls: its ID, length, and fields. This is the largest and most commonly edited section.

#### Message Structure

```yaml
Message ID:
  <Message Name>:
    MSG ID: <hex ID>         # e.g. 0x003
    MSG LENGTH: <bytes>      # total message length in bytes
    <Field Name>:
      bit_start: <N>
      comment: <description>
      data type: <type>
      units: <unit string>          # optional
      scaled min: <number>          # optional
      scaled max: <number>          # optional
      map equation: "<equation>"    # optional
```

#### Data Types


| Type  | Meaning                                    | C Equivalent |
| ----- | ------------------------------------------ | ------------ |
| `b`   | Boolean (1 bit)                            | bit field    |
| `u4`  | Unsigned 4-bit                             | nibble       |
| `u8`  | Unsigned 8-bit                             | `uint8_t`    |
| `u16` | Unsigned 16-bit                            | `uint16_t`   |
| `u32` | Unsigned 32-bit                            | `uint32_t`   |
| `i16` | Signed 16-bit                              | `int16_t`    |
| `s8`  | Signed 8-bit                               | `int8_t`     |
| `s`   | String (variable length, up to MSG LENGTH) | `char[]`     |


#### Field Attributes

- `**bit_start**` (REQUIRED REQUIRED REQUIRED REQUIRED) — the bit offset of the field within the message. Must be the first sub-key under each field name. Parsers calculate message size based on bit starts — nothing will work without it.
- `**data type**` (required) — one of the types listed above.
- `**comment:**` (optional) — description of the field. `STRUCTparser.pl` extracts this as a doxygen-style comment in the generated struct. Supports multi-line continuation:
  ```yaml
      comment: First line of description
        Continuation line joined to the same comment.
  ```
- `**units**` (optional) — human-readable unit (e.g. `Volts`, `RPM`, `'%'`, `Celsius`, `Bool`, `Enum`, `ms`).
- `**scaled min` / `scaled max**` (optional) — the real-world range after the map equation is applied.
- `**map equation**` (optional) — the formula to convert the raw value to engineering units (e.g. `"0.01x"`, `"0.1x-3276.8"`).

#### Reserved Fields

Mark unused bit ranges as `Reserved`:

```yaml
    Reserved:
      bit_start: 23-31
```

If you have a data type for a field not divisible by 8 (ex: not a u8, u16, u32, etc..), then make sure you have padding to ensure the next field starts at a bit value that IS divisble by 8. Make sure to follow general memory padding conventions within bytes as well.
(Ex: only start a u4 on bit 0 or 4 of a byte, u2 on anything space divisible by 2, etc.)

Use `comment:` on a Reserved field to document why the padding exists and what bits are occupied.

No sub-properties are needed. The parsers will skip or collapse reserved ranges.

#### Adding a New Message

1. Choose an unused `MSG ID` (hex).
2. Add an entry under `Message ID` following the structure above.
3. Add a corresponding route in the `routing` section.
4. Rebuild — the parsers auto-generate the updated headers.

#### Adding a Field to an Existing Message

1. Pick the next available bit offset (must not overlap with existing fields).
2. Add the field with `bit_start: <N>` and its sub-properties.
3. Update `MSG LENGTH` if the message now occupies more bytes.

---

### 5. `Custom CAN ID` Section (parsed by `CANparser.pl`)

Defines CAN messages that use externally defined IDs (e.g. third-party motor controllers, chargers). These IDs are **not** auto-assigned by the team.

```yaml
Custom CAN ID:
  <Message Name>:
    CAN ID: <decimal or hex ID>   # e.g. 116, A16
    Length: <bytes>
    signals:
      - name: "<Signal Name>"
        bit_start: <N>
        # <description comment>
      - name: "<Signal Name>"
        bit_start: <N>
```

**Key differences from `Message ID`:**

- Uses `CAN ID` (not `MSG ID`) and the value can be decimal or bare hex (e.g. `A16`).
- Uses `Length` (not `MSG LENGTH`).
- Fields are listed under a `signals:` array with `name:` and `bit_start:` keys, rather than direct field names with `bit_start:` as a sub-key.

**Parser behavior:** `CANparser.pl` reads each entry's `CAN ID`, sanitizes the message name into a C identifier, and emits it as an enum value in `GRCAN_CUSTOM_ID.h`.

#### Adding a Custom CAN ID Entry

1. Add a new block under `Custom CAN ID` with `CAN ID`, `Length`, and `signals`.
2. The `CAN ID` value will be formatted as hex (`0x...`) in the generated enum if it contains non-decimal characters, or kept as decimal otherwise.

---

### 6. `GR ID` Section (parsed by `GRparser.pl`)

A flat mapping of device/node names to their unique network identifiers. Used for addressing in the GR protocol layer.

```yaml
GR ID:
  <Device Name>: "<hex ID>"    # e.g. "0x02"
```

**Parser behavior:** `GRparser.pl` reads each key-value pair and emits a C enum in `GRCAN_NODE_ID.h`. The name is sanitized (non-alphanumeric characters become underscores).

#### Adding a New Device

Add a single line with the device name and a unique hex ID string:

```yaml
  My New Board: "0x2A"
```
