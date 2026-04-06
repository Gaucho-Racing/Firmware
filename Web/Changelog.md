# Changelog

## 2026-04-06 (patch 2)

### Stricter topology enforcement across all entry points

**Add Route form (`formRoutingAdd.js`) — save-time hard blocks:**
- Receiver field: if the typed node is not registered in GR ID → blocks with "Node does not exist"
- Receiver field: if the node exists but is not physically on the selected bus → blocks with `"<Node>" is not physically on <bus>`
- Bus field: if the sender device exists and is not physically wired to the selected bus → blocks with `"<Device>" is not physically wired to <bus>` (covers the case where a locked bus was manually set to an invalid value)

**Add Bus form (`formBusAdd.js`) — highest-level guard:**
- Bus dropdown is now filtered to only show buses the device is physically wired to (same pattern as the route form's bus filter)
- Save handler hard-blocks with `"<Device>" is not physically wired to <bus>` even if somehow an invalid bus is submitted

All new blocks are skipped when `PhysicalTopology` isn't loaded, preserving graceful degradation.

## 2026-04-06 (patch)

### Fix: bus filter silently overriding locked bus in Add Route form
- When "Add Route" was opened from a locked-bus context (e.g. clicking add on an existing CAN3 block for DGPS), the topology bus filter excluded CAN3 from the option list. `makeSelect` received `"CAN3"` as the selected value with no matching option, so the browser defaulted to `"CAN1"`. The select was then disabled at the wrong bus, causing routes to be written to CAN1 instead of CAN3.
- Fix: bus filtering now only applies when `busPort` is not pre-provided (i.e. the user has free choice of bus). When the bus is already locked, all three options are always included so the disabled select renders the correct value.

## 2026-04-06

### Physical CAN topology enforcement
- Added `can_topology.txt` — a human-editable plain-text file (CANdo-style indented format with `#` comments) that declares which nodes are physically wired to which CAN bus. Edit this file when hardware layout changes; no coding knowledge required.
- Added `physicalTopology.js` → `window.PhysicalTopology` — a fully self-contained module that owns all topology logic (fetch, parse, cache). Other files call only `isLoaded()`, `isOnBus()`, and `getNodesForBus()`; they have zero knowledge of internals.
- `formRoutingAdd.js`: receiver autocomplete dropdown is now filtered to only show nodes physically on the selected bus. If the user manually types a known node that isn't on the selected bus, an inline warning appears: `"<Node>" is not physically on <CAN port>.`
- `formRoutingAdd.js`: bus dropdown is filtered to only show buses the sender device is physically wired to (when device is pre-selected and topology is loaded).
- `candoDocument.js`: added V8 `PHYSICAL_BUS_VIOLATION` warning to `validate()` — flags existing routes where sender or receiver is not physically on the routed bus. Reported in the console on load alongside other violations.
- `Debugger` and `ALL` are always exempt from topology checks.
- All topology checks silently no-op if `can_topology.txt` fails to load (e.g. local file mode), so the UI degrades gracefully.

## 2026-04-05

### Orange "Custom" chip for Custom CAN ID nodes
- Nodes with GR ID `"0x00"` (Charger, DTI Inverter, Energy Meter, IMD) now display an
  orange **"Custom"** chip instead of the misleading purple `0x00` badge in the Nodes panel.
- Added `.item-accent-custom` CSS class in `viewer.css` with orange color scheme.
- Updated `appendNodeIdAccent()` in `viewer.js` to detect `"0x00"` and apply custom styling.

### Prune unrouted messages at download
- Messages with no routing entries are automatically removed from the downloaded `.CANdo` file.
- Added `_getRoutedMessageNames()` helper and `pruneUnrouted` flag to `_serialize()` in `candoDocument.js`.
- `getSerializedText()` now calls `_serialize(true)` to prune on download; mutation path is unchanged.
- Added 5 tests covering Message ID pruning, Custom CAN ID pruning, ALL-receiver routes, count validation, and no-side-effect contract.
