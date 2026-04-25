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
| `graphView.js` | Cytoscape.js network graph visualization (`window.GrcanGraphView`) |
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

### Vendored

| File | Purpose |
|---|---|
| `cytoscape.min.js` | Cytoscape.js graph library (vendored) |

## Script Load Order

Scripts are loaded in `index.html` in strict dependency order. `viewer.js` expects `window.GrcanApi`, `window.GrcanEditor`, and all form-augmented methods to exist. Reordering will break runtime symbol availability.

1. `logic.js`
2. `formUtils.js`
3. `candoDocument.js`
4. `editor.js`
5. `formMessageDef.js`
6. `formRoutingAdd.js`
7. `formRoutingEdit.js`
8. `formNodeEdit.js`
9. `formBusEdit.js`
10. `formBusAdd.js`
11. `formConfirmDelete.js`
12. `formSuperAdd.js`
13. `formCustomCanId.js`
14. `physicalTopology.js`
15. `diffViewer.js`
16. `graphView.js`
17. `viewer.js`
18. `background.js`

## Tests

Tests live in `tests/` and run under Node.js:

- `candoDocument.test.js` — semantic document model tests
- `logic.test.js` — parser/API utility tests
- `manual/` — manual test scenarios

## Editing `can_topology.txt`

This file defines which devices are physically connected to each CAN bus. Bus headers must match the bus names declared in the `Bus ID:` section of `GRCAN.CANdo`. The format is:

```
Primary:
  ECU
  BCU
  ...

Data:
  ECU
  SAMM_Mag_1
  ...
```

- Bus headers must exactly match `Bus ID` names in `GRCAN.CANdo` (e.g. `Primary`, `Data`, `Charger`).
- Node names must exactly match `GR ID` entries in `GRCAN.CANdo`.
- `Debugger` and `ALL` are always exempt and should not be listed.
- Lines starting with `#` are comments.
