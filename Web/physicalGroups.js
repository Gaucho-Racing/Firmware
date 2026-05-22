// Purpose: Functional grouping for the Graph View physical-bus renderer.
// Derives groups automatically from node name prefixes — no external JSON needed.
// Nodes sharing the same prefix (e.g. "Fan Controller 1", "Fan Controller 2")
// are placed in a named group. Nodes with no shared prefix are NOT lumped into
// "Other" — instead each becomes its own single-node group, ordered by their
// position in the document and distributed alternately top/bottom (up-down-up…).
// All groups (prefix and singleton) are interleaved top/bottom in the order they
// first appear, so the visual layout stays close to declaration order.
// Exposed as: window.PhysicalGroups.

(function () {
	"use strict";

	// Nodes always placed on the bus spine.
	const _BUS_SPINE = ["Debugger", "ALL"];

	// Extract the shared prefix from a node name by stripping a trailing number
	// ("Fan Controller 1" → "Fan Controller", "SAMM_Mag_1" → "SAMM_Mag") or a
	// short uppercase positional abbreviation ("TireTemp_FL" → "TireTemp").
	// Returns the full name unchanged when nothing can be stripped.
	function _extractPrefix(name) {
		// Trailing space/underscore + digits
		let m = name.match(/^(.+?)[\s_]+\d+$/);
		if (m) return m[1];
		// Trailing underscore/space + 1–3 uppercase letters (positional suffix)
		m = name.match(/^(.+?)[_\s]+[A-Z]{1,3}$/);
		if (m) return m[1];
		return name;
	}

	// Build an ordered list of display groups from an array of node names.
	// Groups appear in the order their first member was seen in `names`
	// (i.e. document/ID order is preserved). Nodes with a unique prefix each
	// become their own single-node group — no "Other" bucket is created.
	function _buildGroups(names) {
		// First pass: assign every name to a prefix bucket.
		const byPrefix = new Map(); // prefix → [name, ...]
		for (const name of names) {
			const prefix = _extractPrefix(name);
			if (!byPrefix.has(prefix)) byPrefix.set(prefix, []);
			byPrefix.get(prefix).push(name);
		}

		// Second pass: emit groups in first-seen order, keeping the original
		// name sequence within each prefix bucket.
		const seenPrefixes = new Set();
		const groups = [];
		for (const name of names) {
			const prefix = _extractPrefix(name);
			if (seenPrefixes.has(prefix)) continue;
			seenPrefixes.add(prefix);
			const members = byPrefix.get(prefix);
			// Use the prefix as the group label when there are siblings;
			// use the bare node name when it stands alone.
			const label = members.length > 1 ? prefix : name;
			groups.push({ name: label, nodes: members });
		}
		return groups;
	}

	window.PhysicalGroups = {
		// No-op: groups are derived dynamically, no file to fetch.
		load: async function () {},

		isLoaded: function () {
			return true;
		},

		// Partition present nodes into { top, bottom, bus } for the layout engine.
		// Bus-spine nodes (Debugger, ALL) go on the spine. Remaining groups are
		// distributed alternately top/bottom in document order: first group → top,
		// second → bottom, third → top, etc.
		getGroupsForBus: function (_busPort, presentNodes) {
			const busNodes = [];
			const assigned = new Set();

			for (const spineId of _BUS_SPINE) {
				if (presentNodes.includes(spineId)) {
					busNodes.push(spineId);
					assigned.add(spineId);
				}
			}

			const remaining = presentNodes.filter((n) => !assigned.has(n));
			const groups = _buildGroups(remaining);

			const top = [];
			const bottom = [];
			groups.forEach((g, i) => {
				if (i % 2 === 0) top.push(g);
				else bottom.push(g);
			});

			return { top, bottom, bus: busNodes };
		},
	};
})();
