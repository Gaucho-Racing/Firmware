// Purpose: Detect array-shaped sibling fields in a CAN message and fold/unfold
// them around the existing flat YAML schema. No backend change: the .CANdo file
// still stores N hand-expanded entries (e.g. "Cell 0 Voltage", "Cell 0 Temp", ...);
// this module just lets the viewer and editor present them as one group.
//
// A field "Cell 0 Voltage" parses into { prefix: "Cell", index: 0, suffix: "Voltage" }.
// A set of >=3 fields with the same (prefix, suffix), contiguous indices, identical
// dataType, and arithmetic bit_start stride qualifies as one ARRAY COLUMN. Multiple
// columns sharing the same prefix and same index range are merged into one GROUP
// (the cell-data table: Voltage + Temp over Cell 0..31).
//
// Dual-mode: browser (window.GrcanArrayPattern) or Node.js (module.exports) for tests.

/* global window, module */
(function (factory) {
	if (typeof module !== "undefined" && module.exports) {
		module.exports = factory();
	} else {
		window.GrcanArrayPattern = factory();
	}
})(function () {
	"use strict";

	const MIN_ELEMENTS = 3;

	// "Cell 0 Voltage"      → { prefix: "Cell",            index: 0,  suffix: "Voltage" }
	// "Tire Temp Frame 23"  → { prefix: "Tire Temp Frame", index: 23, suffix: "" }
	// "Ping Group 2"        → { prefix: "Ping Group",      index: 2,  suffix: "" }
	// "Foo"                 → null
	function parseFieldName(name) {
		if (typeof name !== "string") return null;
		const m = name.match(/^(.+?)\s+(\d+)(?:\s+(.+))?$/);
		if (!m) return null;
		const prefix = m[1].trim();
		const index = parseInt(m[2], 10);
		const suffix = (m[3] || "").trim();
		if (!prefix || !Number.isInteger(index) || index < 0) return null;
		return { prefix, index, suffix };
	}

	// Normalise the input signal shape. Callers from viewer-side (logic.js
	// byteMappings) pass `{fieldName, bitStart, dataType, comment}`; callers from
	// the edit form (parseMessageFromRaw) pass `{fieldName, bitStart, rawDataType,
	// comment, units, scaledMin, scaledMax, mapEquation}`; the candoDocument model
	// uses `{name, bitStart, dataType, ...}`. Pull all of them into one canonical
	// shape so the rest of the module doesn't have to care.
	function _coerce(sig) {
		if (!sig) return null;
		const name = sig.name != null ? sig.name : sig.fieldName;
		const rawBit = sig.bitStart;
		let bitStart = null;
		if (typeof rawBit === "number") {
			bitStart = rawBit;
		} else if (typeof rawBit === "string") {
			const cleaned = rawBit.replace(/,/g, "").trim();
			const single = cleaned.match(/^(\d+)$/);
			const range = cleaned.match(/^(\d+)\s*-\s*\d+$/);
			if (single) bitStart = parseInt(single[1], 10);
			else if (range) bitStart = parseInt(range[1], 10);
		}
		const dataType = sig.dataType != null ? sig.dataType : sig.rawDataType;
		return {
			ref: sig,
			name: name || "",
			bitStart: bitStart,
			dataType: dataType || "",
			comment: sig.comment != null ? sig.comment : "",
			units: sig.units != null ? sig.units : "",
			scaledMin: sig.scaledMin != null ? sig.scaledMin : "",
			scaledMax: sig.scaledMax != null ? sig.scaledMax : "",
			mapEquation: sig.mapEquation != null ? sig.mapEquation : "",
		};
	}

	function _eq(a, b) {
		// Treat null / undefined / empty string as the same "not set" value so we
		// don't falsely reject a fold just because one element has comment: "" and
		// another has comment: null.
		const aN = a == null || a === "" ? "" : String(a);
		const bN = b == null || b === "" ? "" : String(b);
		return aN === bN;
	}

	// Given a list of fields that share (prefix, suffix), find the longest
	// contiguous run of indices where dataType is identical, bit_start values
	// form an arithmetic progression, and units/scaling/comment are uniform.
	// Returns the column descriptor or null if no qualifying run exists.
	function _buildColumn(prefix, suffix, members) {
		if (members.length < MIN_ELEMENTS) return null;
		const sorted = members.slice().sort((a, b) => a.index - b.index);

		// Find a contiguous index window with consistent type/units/scaling.
		let bestStart = -1,
			bestEnd = -1;
		for (let i = 0; i < sorted.length; ) {
			const head = sorted[i];
			if (head.bitStart == null || !head.dataType) {
				i++;
				continue;
			}
			let j = i;
			while (j + 1 < sorted.length) {
				const cur = sorted[j];
				const nxt = sorted[j + 1];
				if (nxt.index !== cur.index + 1) break;
				if (nxt.bitStart == null) break;
				if (nxt.dataType !== head.dataType) break;
				if (!_eq(nxt.units, head.units)) break;
				if (!_eq(nxt.scaledMin, head.scaledMin)) break;
				if (!_eq(nxt.scaledMax, head.scaledMax)) break;
				if (!_eq(nxt.mapEquation, head.mapEquation)) break;
				if (!_eq(nxt.comment, head.comment)) break;
				j++;
			}
			if (j - i + 1 >= MIN_ELEMENTS) {
				// bit_start must form a constant-stride arithmetic progression.
				const stride = sorted[i + 1].bitStart - sorted[i].bitStart;
				let arithmetic = true;
				for (let k = i; k < j; k++) {
					if (sorted[k + 1].bitStart - sorted[k].bitStart !== stride) {
						arithmetic = false;
						break;
					}
				}
				if (stride > 0 && arithmetic && j - i > bestEnd - bestStart) {
					bestStart = i;
					bestEnd = j;
				}
			}
			i = j + 1;
		}
		if (bestStart < 0) return null;

		const window = sorted.slice(bestStart, bestEnd + 1);
		const head = window[0];
		const stride =
			window.length > 1 ? window[1].bitStart - window[0].bitStart : 0;
		return {
			prefix,
			suffix,
			indexStart: head.index,
			indexEnd: window[window.length - 1].index,
			baseBit: head.bitStart,
			stride,
			dataType: head.dataType,
			comment: head.comment,
			units: head.units,
			scaledMin: head.scaledMin,
			scaledMax: head.scaledMax,
			mapEquation: head.mapEquation,
			elementSignals: window.map((w) => w.ref),
		};
	}

	// Main entry point. Pass in the message's signals in document order. Returns
	// `{ groups, ungrouped }` where `ungrouped` is the signals that didn't fold,
	// in their original order.
	function detect(signals) {
		const coerced = (signals || []).map(_coerce).filter(Boolean);
		// Bucket by (prefix, suffix). Skip anything that doesn't match the pattern.
		const buckets = new Map();
		const orphans = [];
		for (const sig of coerced) {
			const parsed = parseFieldName(sig.name);
			if (!parsed) {
				orphans.push(sig);
				continue;
			}
			const key = parsed.prefix + "\n" + parsed.suffix;
			if (!buckets.has(key)) buckets.set(key, []);
			buckets.get(key).push({ ...sig, index: parsed.index });
		}

		// Build one candidate column per qualifying bucket.
		const columnsByPrefix = new Map();
		const consumed = new Set();
		for (const [key, members] of buckets) {
			const [prefix, suffix] = key.split("\n");
			const col = _buildColumn(prefix, suffix, members);
			if (!col) {
				members.forEach((m) => orphans.push(m));
				continue;
			}
			if (!columnsByPrefix.has(prefix)) columnsByPrefix.set(prefix, []);
			columnsByPrefix.get(prefix).push(col);
			col.elementSignals.forEach((s) => consumed.add(s));
			// Any signals in the bucket that fell OUTSIDE the chosen window need
			// to go back to the orphan list so they still get rendered.
			members.forEach((m) => {
				if (!consumed.has(m.ref)) orphans.push(m);
			});
		}

		// Group columns under the same prefix that share an index range.
		const groups = [];
		for (const [prefix, cols] of columnsByPrefix) {
			cols.sort(
				(a, b) =>
					a.indexStart - b.indexStart ||
					a.baseBit - b.baseBit ||
					a.suffix.localeCompare(b.suffix),
			);
			const used = new Array(cols.length).fill(false);
			for (let i = 0; i < cols.length; i++) {
				if (used[i]) continue;
				used[i] = true;
				const peers = [cols[i]];
				for (let j = i + 1; j < cols.length; j++) {
					if (used[j]) continue;
					if (
						cols[j].indexStart === cols[i].indexStart &&
						cols[j].indexEnd === cols[i].indexEnd
					) {
						used[j] = true;
						peers.push(cols[j]);
					}
				}
				groups.push({
					prefix,
					indexStart: cols[i].indexStart,
					indexEnd: cols[i].indexEnd,
					columns: peers,
				});
			}
		}

		// Preserve original signal order for ungrouped output.
		const ungrouped = coerced
			.filter((s) => !consumed.has(s.ref))
			.map((s) => s.ref);

		return { groups, ungrouped };
	}

	// Re-generate the flat list of signal objects from one column. Used by the
	// edit form when serialising an Array Card back to YAML — the shape returned
	// matches what formMessageDef.js's save path expects (name + bitStart +
	// dataType + comment + units + scaledMin + scaledMax + mapEquation).
	function expandColumn(group, column) {
		const out = [];
		for (let i = group.indexStart; i <= group.indexEnd; i++) {
			const offset = i - group.indexStart;
			const name = column.suffix
				? group.prefix + " " + i + " " + column.suffix
				: group.prefix + " " + i;
			out.push({
				name,
				bitStart: String(column.baseBit + offset * column.stride),
				dataType: column.dataType,
				comment: column.comment || "",
				units: column.units || "",
				scaledMin: column.scaledMin || "",
				scaledMax: column.scaledMax || "",
				mapEquation: column.mapEquation || "",
			});
		}
		return out;
	}

	// Expand every column in a group, in column-major order then index-major
	// within each column. Interleaving (Voltage, Temp, Voltage, Temp, ...) is
	// not preserved — the YAML doesn't care about field order, and the
	// detector treats any order as long as bit_starts line up.
	function expandGroup(group) {
		const out = [];
		for (const col of group.columns) {
			for (const sig of expandColumn(group, col)) out.push(sig);
		}
		return out;
	}

	// Validate one group's bit layout against the message's total length.
	// Returns { ok, errors: string[] }.
	function validateGroup(group, msgLengthBits) {
		const errors = [];
		if (!group || !Array.isArray(group.columns) || group.columns.length === 0) {
			errors.push("Array has no columns");
		}
		if (!Number.isInteger(group.indexStart) || group.indexStart < 0) {
			errors.push("Index start must be a non-negative integer");
		}
		if (
			!Number.isInteger(group.indexEnd) ||
			group.indexEnd < group.indexStart
		) {
			errors.push("Index end must be >= index start");
		}
		const span = group.indexEnd - group.indexStart + 1;
		if (span < MIN_ELEMENTS) {
			errors.push(
				"Array needs at least " + MIN_ELEMENTS + " elements (got " + span + ")",
			);
		}
		for (const col of group.columns || []) {
			if (!col.dataType) {
				errors.push(
					"Column '" + (col.suffix || "(unnamed)") + "': missing data type",
				);
				continue;
			}
			if (!Number.isInteger(col.baseBit) || col.baseBit < 0) {
				errors.push(
					"Column '" +
						(col.suffix || "(unnamed)") +
						"': start bit must be a non-negative integer",
				);
			}
			if (!Number.isInteger(col.stride) || col.stride <= 0) {
				errors.push(
					"Column '" +
						(col.suffix || "(unnamed)") +
						"': stride must be a positive integer",
				);
			}
			if (Number.isInteger(msgLengthBits) && msgLengthBits > 0) {
				const last = col.baseBit + (span - 1) * col.stride;
				if (last < 0 || last >= msgLengthBits) {
					errors.push(
						"Column '" +
							(col.suffix || "(unnamed)") +
							"': last element at bit " +
							last +
							" exceeds MSG LENGTH (" +
							msgLengthBits +
							" bits)",
					);
				}
			}
		}
		return { ok: errors.length === 0, errors };
	}

	return {
		MIN_ELEMENTS,
		parseFieldName,
		detect,
		expandColumn,
		expandGroup,
		validateGroup,
	};
});
