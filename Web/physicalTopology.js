// Purpose: Physical CAN bus topology enforcement module.
// Loads and parses Web/can_topology.json — the human-editable source of truth
// for which devices are physically wired to which CAN bus.
// All exemption logic (Debugger, ALL) lives here and nowhere else.
// No other file knows about storage, parsing, or fetch internals.
// Callers use only the four public methods below.
// Exposed as: window.PhysicalTopology

(function () {
	"use strict";

	// Nodes that are always allowed on any bus — never topology-checked.
	const _EXEMPT = new Set(["Debugger", "ALL"]);

	// Map<busPort string, Set<nodeName string>>
	let _topology = new Map();
	let _loaded = false;

	// ==================== Parser ====================
	// Pure function: JSON text → Map<bus, Set<name>>

	function _parse(text) {
		const result = new Map();
		const data = JSON.parse(text);
		for (const [bus, nodes] of Object.entries(data)) {
			if (Array.isArray(nodes)) result.set(bus, new Set(nodes));
		}
		return result;
	}

	// ==================== Public API ====================

	window.PhysicalTopology = {
		// Fetch and parse can_topology.json. Call once at startup.
		// Resolves even on failure — isLoaded() will return false in that case.
		load: async function () {
			try {
				const resp = await fetch("can_topology.json");
				if (!resp.ok) return;
				const text = await resp.text();
				_topology = _parse(text);
				_loaded = true;
			} catch (_) {
				// Silently no-op: fetch unavailable or malformed JSON
				// (e.g. file:// local mode, or hand-edit syntax error).
			}
		},

		// True only after a successful load().
		isLoaded: function () {
			return _loaded;
		},

		// Is nodeName physically wired to busPort?
		// Always returns true for exempt nodes (Debugger, ALL) or if not loaded.
		isOnBus: function (nodeName, busPort) {
			if (!_loaded) return true;
			if (_EXEMPT.has(nodeName)) return true;
			const busSet = _topology.get(busPort);
			if (!busSet) return false;
			return busSet.has(nodeName);
		},

		// All node names registered for busPort in the topology file.
		// Returns [] if not loaded or bus unknown.
		getNodesForBus: function (busPort) {
			if (!_loaded) return [];
			const busSet = _topology.get(busPort);
			return busSet ? [...busSet] : [];
		},
	};
})();
