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

	// Surface entries in can_topology.json that don't exist as devices in the
	// .CANdo GR ID block. Drift here = unreachable receivers in the form.
	// Idempotent: warns once per call, no internal state mutation.
	function _validateAgainstDeviceRegistry() {
		const doc = window.GrcanDocument;
		if (!doc || typeof doc.getDeviceNames !== "function") return;
		const known = new Set(doc.getDeviceNames());
		const unknown = [];
		for (const [bus, nodes] of _topology) {
			for (const n of nodes) {
				if (_EXEMPT.has(n)) continue;
				if (!known.has(n)) unknown.push(`${bus}: ${n}`);
			}
		}
		if (unknown.length) {
			console.warn(
				"[PhysicalTopology] can_topology.json lists nodes not in the .CANdo GR ID registry:",
				unknown,
			);
		}
	}

	// ==================== Public API ====================

	window.PhysicalTopology = {
		// Fetch and parse can_topology.json. Call once at startup.
		// Resolves even on failure — isLoaded() will return false in that case.
		load: async function () {
			try {
				// no-cache: re-validate with server every load so a topology JSON
				// edit reaches users on next page open, not on next hard-refresh.
				const resp = await fetch("can_topology.json", { cache: "no-cache" });
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

		// Caller should invoke this once after the .CANdo has been parsed so
		// GrcanDocument.getDeviceNames() returns a populated set. Safe to call
		// even before load() resolves — it short-circuits.
		validate: _validateAgainstDeviceRegistry,
	};
})();
