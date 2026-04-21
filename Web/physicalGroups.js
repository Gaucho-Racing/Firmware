// Purpose: Functional grouping metadata for the Graph View physical-bus
// renderer. Loads and parses Web/can_groups.json — human-editable mapping
// of device names to functional groups (POWER/HV, THERMAL CONTROL, …).
// Each group declares a side: "top", "bottom", or "bus".
// Exposed as: window.PhysicalGroups.

(function () {
	"use strict";

	// Nodes always placed on the bus spine, regardless of can_groups.json.
	const _BUS_SPINE = ["Debugger", "ALL"];

	// Array of { name, side, nodes: Set<string> }, preserving declaration order.
	let _groups = [];
	let _loaded = false;

	// ==================== Parser ====================
	// Pure function: JSON text → Array<{name, side, nodes: Set<string>}>

	function _parse(text) {
		const data = JSON.parse(text);
		const result = [];
		const groups = Array.isArray(data.groups) ? data.groups : [];
		for (const group of groups) {
			if (!group || typeof group.name !== "string") continue;
			if (group.side !== "top" && group.side !== "bottom" && group.side !== "bus") continue;
			result.push({
				name: group.name,
				side: group.side,
				nodes: new Set(Array.isArray(group.nodes) ? group.nodes : []),
			});
		}
		return result;
	}

	// ==================== Public API ====================

	window.PhysicalGroups = {
		// Fetch and parse can_groups.json. Resolves even on failure;
		// isLoaded() tells you whether it succeeded.
		load: async function () {
			try {
				const resp = await fetch("can_groups.json");
				if (!resp.ok) return;
				const text = await resp.text();
				_groups = _parse(text);
				_loaded = true;
			} catch (_) {
				// Silently no-op: fetch unavailable or malformed JSON
				// (e.g. file:// local mode, or hand-edit syntax error).
			}
		},

		isLoaded: function () {
			return _loaded;
		},

		// Intersect the groups with the nodes actually present on a bus.
		// Returns { top: [{name, nodes[]}], bottom: [{name, nodes[]}], bus: [ids] }.
		// Debugger / ALL are forced onto the bus spine. Unassigned nodes go
		// into an implicit "Other" group at the bottom.
		getGroupsForBus: function (_busPort, presentNodes) {
			const present = new Set(presentNodes);
			const assigned = new Set();
			const top = [];
			const bottom = [];
			const busNodes = [];

			if (_loaded) {
				for (const group of _groups) {
					const members = [];
					for (const nodeName of group.nodes) {
						if (present.has(nodeName) && !assigned.has(nodeName)) {
							members.push(nodeName);
							assigned.add(nodeName);
						}
					}
					if (members.length === 0) continue;
					if (group.side === "top") {
						top.push({ name: group.name, nodes: members });
					} else if (group.side === "bottom") {
						bottom.push({ name: group.name, nodes: members });
					} else if (group.side === "bus") {
						for (const m of members) busNodes.push(m);
					}
				}
			}

			for (const spineId of _BUS_SPINE) {
				if (present.has(spineId) && !assigned.has(spineId)) {
					busNodes.push(spineId);
					assigned.add(spineId);
				}
			}

			const others = [];
			for (const nodeName of presentNodes) {
				if (!assigned.has(nodeName)) others.push(nodeName);
			}
			if (others.length > 0) {
				bottom.push({ name: "Other", nodes: others });
			}

			return { top, bottom, bus: busNodes };
		},
	};
})();
