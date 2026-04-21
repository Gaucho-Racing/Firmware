# GRCAN Web Viewer/Editor

Browser-based viewer and editor for the `GRCAN.CANdo` file used by Gaucho Racing. Loads CANdo from any branch, tag, or commit via the GitHub API, renders it in a 3-panel hierarchy (Node -> Bus -> Message), and supports in-memory editing with diff review before download.

Edits are **not** saved to a backend. They exist in browser memory until the user downloads the modified file.

## File Overview

### Core Runtime

| File | Purpose |
|---|---|
| `index.html` | App shell, 3-panel layout, script load order |
| `logic.js` | GitHub API fetchers and CANdo text parsers (`window.GrcanApi`) |
| `candoDocument.js` | Semantic document model with cross-section invariants (`window.GrcanDocument`) |
| `editor.js` | In-memory mutation engine, raw text state (`window.GrcanEditor`) |
| `viewer.js` | Main controller: rendering, navigation, edit/delete wiring |
| `physicalTopology.js` | Parses `can_topology.txt` to enforce physical bus-to-node constraints |
| `physicalGroups.js` | Parses `can_groups.txt` — functional groupings used by the Graph View renderer (`window.PhysicalGroups`) |
| `layoutPhysicalBus.js` | Pure SVG layout for the physical-bus Graph View (`window.LayoutPhysicalBus`) |
| `graphView.js` | Physical-bus SVG graph visualization (`window.GrcanGraphView`) |
| `diffViewer.js` | Side-by-side text diff modal shown before download |
| `background.js` | Decorative animated canvas background |

### Editor Forms

| File | Purpose |
|---|---|
| `formUtils.js` | Shared modal/form builders, validators, SVG icon constants |
| `formMessageDef.js` | Add/edit Message ID definitions |
| `formRoutingAdd.js` | Add routing entries |
| `formRoutingEdit.js` | Edit existing routing entries |
| `formNodeEdit.js` | Rename a node/device |
| `formBusEdit.js` | Rename a bus under a node |
| `formBusAdd.js` | Add a new bus to a node |
| `formCustomCanId.js` | Edit Custom CAN ID entries |
| `formSuperAdd.js` | Wizard for creating multiple linked objects at once |
| `formConfirmDelete.js` | Reusable delete confirmation modal |

### Styles

| File | Purpose |
|---|---|
| `index.css` | Base shell styling |
| `viewer.css` | Panel/list/view styles |
| `editor.css` | Modal/form/icon/diff/editor state styles |
| `graphView.css` | Graph view overlay styles |

### Data

| File | Purpose |
|---|---|
| `can_topology.txt` | Physical CAN bus topology: which nodes are wired to which bus. Node names must match `GR ID` entries in `GRCAN.CANdo`. |
| `can_groups.txt` | Functional groupings (POWER/HV, THERMAL CONTROL, …) used by the Graph View renderer. Each group declares a side: `top`, `bottom`, or `bus`. Node names must match `GR ID` entries. |

### Vendored (retained for rollback, no longer loaded)

| File | Purpose |
|---|---|
| `cytoscape.min.js` | Previous graph library. The current Graph View renders pure SVG and does not load this. |

## Script Load Order

Scripts are loaded in `index.html` in strict dependency order. `viewer.js` expects `window.GrcanApi`, `window.GrcanEditor`, and all form-augmented methods to exist. Reordering will break runtime symbol availability.

1. `physicalTopology.js`
2. `physicalGroups.js`
3. `logic.js`
4. `formUtils.js`
5. `editor.js`
6. `candoDocument.js`
7. `formMessageDef.js`
8. `formCustomCanId.js`
9. `formRoutingAdd.js`
10. `formNodeEdit.js`
11. `formBusEdit.js`
12. `formBusAdd.js`
13. `formConfirmDelete.js`
14. `formSuperAdd.js`
15. `diffViewer.js`
16. `viewer.js`
17. `layoutPhysicalBus.js`
18. `graphView.js`
19. `background.js`

## Tests

Tests live in `tests/` and run under Node.js:

- `candoDocument.test.js` — semantic document model tests
- `logic.test.js` — parser/API utility tests
- `manual/` — manual test scenarios

## Editing `can_topology.txt`

This file defines which devices are physically connected to each CAN bus. The format is:

```
CAN1:
  ECU
  BCU
  ...

CAN2:
  ECU
  SAMM_Mag_1
  ...
```

- Node names must exactly match `GR ID` entries in `GRCAN.CANdo`.
- `Debugger` and `ALL` are always exempt and should not be listed.
- Lines starting with `#` are comments.
