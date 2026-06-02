// Purpose: Semantic document model for the CANdo format.
// Parses the full file into typed data structures, enforces all cross-section
// invariants (I1–I5 per IMPLEMENTATION_PLAN.md), and exposes atomic operations
// that keep routing + GR ID + Message ID consistent with each other.
// After every mutation, serializes the model back to canonical text and calls
// editor.updateRawText() so editor.js remains the single source of raw text truth.
//
// Sections owned (regenerated on serialize): routing, Message ID, Custom CAN ID, GR ID
// Sections preserved verbatim:               Bus ID, byte order
//
// Dual-mode: browser (window.GrcanDocument) or Node.js (module.exports) for tests.

/* global window, module */
(function (factory) {
	if (typeof module !== "undefined" && module.exports) {
		module.exports = factory();
	} else {
		window.GrcanDocument = factory();
	}
})(function () {
	"use strict";

	// ==================== Module-level model state ====================
	// All mutable; reset by _parse() at the start of every operation.

	let _devices = new Map(); // Map<deviceName, DeviceBlock>
	let _grIds = new Map(); // Map<deviceName, hexId: string>
	let _messageIds = new Map(); // Map<msgName, MessageDef>
	let _busIdsText = ""; // verbatim Bus ID section text
	let _byteOrderText = ""; // verbatim byte order line text
	let _customCanIds = new Map(); // Map<name, {name, canId, length, signals[]}>
	let _busIds = new Map(); // Map<busName, numericId: number> derived from Bus ID section

	// ==================== CAN ID format utilities ====================
	// Custom CAN ID section canonical form: bare uppercase hex, no 0x prefix (e.g. "2416").
	// Routing can_id_override canonical form: 0x-prefixed uppercase hex (e.g. "0x2416").
	// These two utilities are the single point of conversion between the two formats.

	function normalizeCanId(raw) {
		const s = String(raw ?? "")
			.trim()
			.replace(/^0x/i, "")
			.toUpperCase();
		return /^[0-9A-F]+$/.test(s) ? s : null;
	}

	function toCanIdOverride(bareHex) {
		const n = normalizeCanId(bareHex);
		return n ? "0x" + n : null;
	}

	// ==================== Parser ====================

	function _parse(rawText) {
		_devices = new Map();
		_grIds = new Map();
		_messageIds = new Map();
		_busIdsText = "";
		_byteOrderText = "";
		_customCanIds = new Map();
		_busIds = new Map();

		if (!rawText) return;
		const lines = rawText.split("\n");

		// Find 0-indexed start line of each top-level section.
		let routingStart = -1,
			byteOrderStart = -1,
			msgIdStart = -1,
			customCanIdStart = -1,
			grIdStart = -1;

		for (let i = 0; i < lines.length; i++) {
			const l = lines[i];
			if (l.startsWith("routing:")) routingStart = i;
			else if (l.startsWith("byte order:")) byteOrderStart = i;
			else if (l.startsWith("Message ID:")) msgIdStart = i;
			else if (l.startsWith("Custom CAN ID:")) customCanIdStart = i;
			else if (l.startsWith("GR ID:")) grIdStart = i;
		}

		// Verbatim: Bus ID = everything before routing section header.
		if (routingStart > 0) {
			_busIdsText = lines.slice(0, routingStart).join("\n").replace(/\n+$/, "");
			_parseBusIdsSection(lines, 0, routingStart);
		}

		// Verbatim: byte order line through the blank line before Message ID.
		if (byteOrderStart > -1) {
			const end = msgIdStart > -1 ? msgIdStart : lines.length;
			_byteOrderText = lines
				.slice(byteOrderStart, end)
				.join("\n")
				.replace(/\n+$/, "");
		}

		// Owned: Custom CAN ID section through the blank line before GR ID.
		if (customCanIdStart > -1) {
			const end = grIdStart > -1 ? grIdStart : lines.length;
			_parseCustomCanIds(lines, customCanIdStart, end);
		}

		// Owned sections: parse into model.
		if (routingStart > -1) {
			const end =
				byteOrderStart > -1
					? byteOrderStart
					: msgIdStart > -1
						? msgIdStart
						: lines.length;
			_parseRouting(lines, routingStart, end);
		}
		if (msgIdStart > -1) {
			const end = customCanIdStart > -1 ? customCanIdStart : lines.length;
			_parseMsgIds(lines, msgIdStart, end);
		}
		if (grIdStart > -1) {
			_parseGrIds(lines, grIdStart, lines.length);
		}
	}

	// Parses the "Bus ID:" header block into _busIds (name → numeric id).
	// Mirrors parseBusIdsFromText in logic.js so candoDocument has its own
	// authoritative copy without depending on window.GrcanApi.
	// Handles both the old flat format ("  BusName: 0") and the new nested
	// format ("  BusName:\n    id: 0\n    max_dlc: 8").
	function _parseBusIdsSection(lines, start, end) {
		let inSection = false;
		let pendingBusName = null; // for nested format: "  BusName:" then "    id: N"
		for (let i = start; i < end; i++) {
			const line = lines[i];
			if (!inSection) {
				if (line.startsWith("Bus ID:")) inSection = true;
				continue;
			}
			// Stop at next top-level (non-indented, non-blank) line.
			if (/^\S/.test(line) && line.trim() !== "") break;
			// Old flat format: "  BusName: 0"
			// \S after the two leading spaces ensures we don't match deeper-indented lines.
			const flatMatch = line.match(/^  (\S[^:]*):\s*(\d+)\s*(?:#.*)?$/);
			if (flatMatch) {
				_busIds.set(flatMatch[1].trim(), parseInt(flatMatch[2], 10));
				pendingBusName = null;
				continue;
			}
			// New nested format: "  BusName:" (indent 2, no value)
			// \S ensures we don't match deeper-indented child lines like "    id:".
			const busNameMatch = line.match(/^  (\S[^:]*):\s*$/);
			if (busNameMatch) {
				pendingBusName = busNameMatch[1].trim();
				continue;
			}
			// New nested format: "    id: N" (indent 4, child of pending bus name)
			if (pendingBusName !== null) {
				const idMatch = line.match(/^    id:\s*(\d+)\s*(?:#.*)?$/);
				if (idMatch) {
					_busIds.set(pendingBusName, parseInt(idMatch[1], 10));
					pendingBusName = null;
				}
			}
		}
	}

	function _parseRouting(lines, start, end) {
		let curDevice = null,
			curBus = null,
			curReceiver = null;

		for (let i = start; i < end; i++) {
			const line = lines[i];
			if (!line.trim()) continue;
			const indent = line.search(/\S/);
			const content = line.trim();

			if (indent === 4 && content.endsWith(":")) {
				// Device name. Skip the "messages:" sub-key which is at indent 2.
				const name = content.slice(0, -1);
				curDevice = { deviceName: name, buses: new Map() };
				_devices.set(name, curDevice);
				curBus = null;
				curReceiver = null;
			} else if (indent === 6 && content.endsWith(":")) {
				if (!curDevice) continue;
				const busPort = content.slice(0, -1);
				curBus = { busPort, receivers: new Map() };
				curDevice.buses.set(busPort, curBus);
				curReceiver = null;
			} else if (indent === 8 && content.endsWith(":")) {
				if (!curBus) continue;
				const recName = content.slice(0, -1);
				curReceiver = { receiverName: recName, routes: [] };
				curBus.receivers.set(recName, curReceiver);
			} else if (indent === 10 && content.startsWith("- msg:")) {
				if (!curReceiver) continue;
				const msgName = content.slice("- msg:".length).trim();
				let canIdOverride = null;
				if (i + 1 < end) {
					const next = lines[i + 1];
					const ni = next.search(/\S/);
					if (ni === 12 && next.trim().startsWith("can_id_override:")) {
						canIdOverride = next.trim().slice("can_id_override:".length).trim();
						i++;
					}
				}
				curReceiver.routes.push({ msgName, canIdOverride });
			}
		}
	}

	function _parseMsgIds(lines, start, end) {
		let curMsg = null,
			curField = null;
		let _inFieldComment = false;

		function flushField() {
			if (curField && curMsg) {
				curMsg.fields.push(curField);
			}
			curField = null;
			_inFieldComment = false;
		}
		function flushMsg() {
			flushField();
			if (curMsg) _messageIds.set(curMsg.name, curMsg);
			curMsg = null;
		}

		for (let i = start; i < end; i++) {
			const line = lines[i];
			if (!line.trim()) continue;
			const indent = line.search(/\S/);
			const content = line.trim();

			if (indent === 2 && content.endsWith(":")) {
				flushMsg();
				_inFieldComment = false;
				curMsg = {
					name: content.slice(0, -1),
					msgId: "",
					msgLength: "",
					fields: [],
				};
			} else if (indent === 4 && curMsg) {
				_inFieldComment = false;
				if (content.startsWith("MSG ID:")) {
					flushField();
					curMsg.msgId = content.slice("MSG ID:".length).trim();
				} else if (content.startsWith("MSG LENGTH:")) {
					flushField();
					curMsg.msgLength = content.slice("MSG LENGTH:".length).trim();
				} else if (content.endsWith(":")) {
					flushField();
					curField = {
						name: content.slice(0, -1),
						bitStart: "",
						comment: null,
						dataType: null,
						units: null,
						scaledMin: null,
						scaledMax: null,
						mapEquation: null,
					};
				}
			} else if (indent >= 6 && curField) {
				if (indent === 6) {
					// All field keywords live at indent 6 — always reset comment flag here
					_inFieldComment = false;
					if (content.startsWith("bit_start:")) {
						curField.bitStart = content.slice("bit_start:".length).trim();
					} else if (content.startsWith("comment:")) {
						const raw = content.slice("comment:".length).trim();
						const inline = raw === "|" || raw === ">" ? "" : raw;
						curField.comment = inline || null;
						_inFieldComment = true;
					} else if (content.startsWith("#")) {
						// backward compat: old # comment format
						const t = content.slice(1).trim();
						curField.comment = curField.comment
							? curField.comment + "\n" + t
							: t;
					} else if (content.startsWith("data type:")) {
						curField.dataType = content.slice("data type:".length).trim();
					} else if (content.startsWith("units:")) {
						curField.units = content.slice("units:".length).trim();
					} else if (content.startsWith("scaled min:")) {
						curField.scaledMin = content.slice("scaled min:".length).trim();
					} else if (content.startsWith("scaled max:")) {
						curField.scaledMax = content.slice("scaled max:".length).trim();
					} else if (content.startsWith("map equation:")) {
						curField.mapEquation = content
							.slice("map equation:".length)
							.trim()
							.replace(/^["']|["']$/g, "");
					}
				} else if (_inFieldComment) {
					// indent > 6: continuation lines of the comment: block
					curField.comment = curField.comment
						? curField.comment + "\n" + content
						: content;
				}
			}
		}
		flushMsg();
	}

	function _parseGrIds(lines, start, end) {
		for (let i = start + 1; i < end; i++) {
			const line = lines[i];
			if (!line.trim()) continue;
			const indent = line.search(/\S/);
			if (indent !== 2) continue;
			const colonIdx = line.indexOf(":");
			if (colonIdx <= 0) continue;
			const name = line.slice(0, colonIdx).trim();
			if (!name) continue;
			const rawVal = line.slice(colonIdx + 1).trim();
			const hexId = rawVal.replace(/^["']|["']$/g, "");
			_grIds.set(name, hexId);
		}
	}

	function _parseCustomCanIds(lines, start, end) {
		let curEntry = null;
		let curSignal = null;
		let _inSignalComment = false;

		function flushSignal() {
			if (curSignal && curEntry) {
				curEntry.signals.push(curSignal);
			}
			curSignal = null;
			_inSignalComment = false;
		}
		function flushEntry() {
			flushSignal();
			if (curEntry) _customCanIds.set(curEntry.name, curEntry);
			curEntry = null;
		}

		for (let i = start; i < end; i++) {
			const line = lines[i];
			if (!line.trim()) continue;
			const indent = line.search(/\S/);
			const content = line.trim();

			if (indent === 2 && content.endsWith(":")) {
				flushEntry();
				_inSignalComment = false;
				curEntry = {
					name: content.slice(0, -1),
					canId: "",
					length: "",
					signals: [],
				};
			} else if (indent === 4 && curEntry) {
				_inSignalComment = false;
				if (content.startsWith("CAN ID:")) {
					curEntry.canId = content.slice("CAN ID:".length).trim();
				} else if (content.startsWith("Length:")) {
					curEntry.length = content.slice("Length:".length).trim();
				} else if (content === "signals: []") {
					// empty signals array on same line — already empty
				} else if (content === "signals:") {
					// signals array follows on subsequent lines
				}
			} else if (indent === 6 && content.startsWith("- name:") && curEntry) {
				flushSignal(); // also resets _inSignalComment
				let nameVal = content.slice("- name:".length).trim();
				nameVal = nameVal.replace(/^["']|["']$/g, "");
				curSignal = { name: nameVal, bitStart: "", comment: null };
			} else if (indent === 8 && curSignal) {
				_inSignalComment = false;
				if (content.startsWith("bit_start:")) {
					curSignal.bitStart = content.slice("bit_start:".length).trim();
				} else if (content.startsWith("comment:")) {
					const raw = content.slice("comment:".length).trim();
					const inline = raw === "|" || raw === ">" ? "" : raw;
					curSignal.comment = inline || null;
					_inSignalComment = true;
				} else if (content.startsWith("#")) {
					// backward compat: old # format
					const t = content.slice(1).trim();
					curSignal.comment = curSignal.comment
						? curSignal.comment + "\n" + t
						: t;
				}
			} else if (indent > 8 && _inSignalComment && curSignal) {
				// continuation lines of the comment: block
				curSignal.comment = curSignal.comment
					? curSignal.comment + "\n" + content
					: content;
			}
		}
		flushEntry();
	}

	// ==================== Serializer ====================

	function _serializeRouting() {
		let out = "routing:\n  messages:\n";
		for (const device of _devices.values()) {
			if (device.buses.size === 0) continue;
			out += "    " + device.deviceName + ":\n";
			for (const bus of device.buses.values()) {
				out += "      " + bus.busPort + ":\n";
				for (const receiver of bus.receivers.values()) {
					out += "        " + receiver.receiverName + ":\n";
					for (const route of receiver.routes) {
						out += "          - msg: " + route.msgName + "\n";
						if (route.canIdOverride) {
							out +=
								"            can_id_override: " + route.canIdOverride + "\n";
						}
					}
				}
			}
		}
		return out;
	}

	function _serializeMessageIds(routedOnly = null) {
		let out = "Message ID:\n";
		for (const msg of _messageIds.values()) {
			if (routedOnly && !routedOnly.has(msg.name)) continue;
			out += "  " + msg.name + ":\n";
			out += "    MSG ID: " + msg.msgId + "\n";
			out += "    MSG LENGTH: " + msg.msgLength + "\n";
			for (const field of msg.fields) {
				out += "    " + field.name + ":\n";
				out += "      bit_start: " + field.bitStart + "\n";
				if (field.comment) {
					if (field.comment.includes("\n")) {
						out += "      comment:\n";
						const lines = field.comment.split("\n");
						if (lines[lines.length - 1] === "") lines.pop();
						for (const line of lines) {
							out += "        " + line + "\n";
						}
					} else {
						out += "      comment: " + field.comment + "\n";
					}
				}
				if (field.dataType !== null) {
					out += "      data type: " + field.dataType + "\n";
				}
				if (field.units) out += "      units: " + field.units + "\n";
				if (field.scaledMin !== null && field.scaledMin !== "")
					out += "      scaled min: " + field.scaledMin + "\n";
				if (field.scaledMax !== null && field.scaledMax !== "")
					out += "      scaled max: " + field.scaledMax + "\n";
				if (field.mapEquation)
					out += '      map equation: "' + field.mapEquation + '"\n';
			}
		}
		return out;
	}

	function _serializeCustomCanIds(routedOnly = null) {
		let out = "Custom CAN ID:\n";
		for (const entry of _customCanIds.values()) {
			if (routedOnly && !routedOnly.has(entry.name)) continue;
			out += "  " + entry.name + ":\n";
			out += "    CAN ID: " + entry.canId + "\n";
			out += "    Length: " + entry.length + "\n";
			if (entry.signals.length === 0) {
				out += "    signals: []\n";
			} else {
				out += "    signals:\n";
				for (const sig of entry.signals) {
					out += '      - name: "' + sig.name + '"\n';
					out += "        bit_start: " + sig.bitStart + "\n";
					if (sig.comment) {
						if (sig.comment.includes("\n")) {
							out += "        comment:\n";
							const lines = sig.comment.split("\n");
							if (lines[lines.length - 1] === "") lines.pop();
							for (const line of lines) {
								out += "          " + line + "\n";
							}
						} else {
							out += "        comment: " + sig.comment + "\n";
						}
					}
				}
			}
		}
		return out;
	}

	function _serializeGrIds() {
		let out = "GR ID:\n\n";
		for (const [name, hexId] of _grIds.entries()) {
			out += "  " + name + ': "' + hexId + '"\n';
		}
		return out;
	}

	function _getRoutedMessageNames() {
		const routed = new Set();
		for (const device of _devices.values()) {
			for (const bus of device.buses.values()) {
				for (const receiver of bus.receivers.values()) {
					for (const route of receiver.routes) {
						routed.add(route.msgName);
					}
				}
			}
		}
		return routed;
	}

	function _serialize(pruneUnrouted = false) {
		const routedOnly = pruneUnrouted ? _getRoutedMessageNames() : null;
		const parts = [
			_busIdsText,
			_serializeRouting(),
			_byteOrderText,
			_serializeMessageIds(routedOnly),
			_serializeCustomCanIds(routedOnly),
			_serializeGrIds(),
		];
		// Strip trailing newlines from each part, join with exactly one blank line,
		// add a single trailing newline.
		return parts.map((p) => p.replace(/\n+$/, "")).join("\n\n") + "\n";
	}

	// ==================== Validator ====================

	function _getCustomCanIdNames() {
		return new Set(_customCanIds.keys());
	}

	function validate() {
		_ensureParsed();
		const results = [];
		const customCanIds = _getCustomCanIdNames();

		// V1: MISSING_GR_ID
		for (const name of _devices.keys()) {
			if (!_grIds.has(name)) {
				results.push({
					severity: "error",
					code: "MISSING_GR_ID",
					message: `Device "${name}" is in routing but has no GR ID entry`,
					context: { device: name },
				});
			}
		}

		// V2: ORPHAN_GR_ID
		for (const name of _grIds.keys()) {
			if (!_devices.has(name)) {
				results.push({
					severity: "warning",
					code: "ORPHAN_GR_ID",
					message: `GR ID entry "${name}" has no corresponding device in routing`,
					context: { device: name },
				});
			}
		}

		// V3: BROKEN_MSG_REF
		for (const device of _devices.values()) {
			for (const bus of device.buses.values()) {
				for (const receiver of bus.receivers.values()) {
					for (const route of receiver.routes) {
						if (
							!_messageIds.has(route.msgName) &&
							!customCanIds.has(route.msgName)
						) {
							results.push({
								severity: "error",
								code: "BROKEN_MSG_REF",
								message: `Route in "${device.deviceName}" > ${bus.busPort} references unknown message "${route.msgName}"`,
								context: {
									device: device.deviceName,
									bus: bus.busPort,
									msg: route.msgName,
								},
							});
						}
					}
				}
			}
		}

		// V4: UNKNOWN_RECEIVER
		for (const device of _devices.values()) {
			for (const bus of device.buses.values()) {
				for (const receiverName of bus.receivers.keys()) {
					if (!_grIds.has(receiverName)) {
						results.push({
							severity: "warning",
							code: "UNKNOWN_RECEIVER",
							message: `Receiver "${receiverName}" in "${device.deviceName}" > ${bus.busPort} is not in GR ID`,
							context: {
								device: device.deviceName,
								bus: bus.busPort,
								receiver: receiverName,
							},
						});
					}
				}
			}
		}

		// V5: DUPLICATE_MSG_ID
		const seenMsgIds = new Map();
		for (const msg of _messageIds.values()) {
			const norm = msg.msgId.toLowerCase();
			if (seenMsgIds.has(norm)) {
				results.push({
					severity: "error",
					code: "DUPLICATE_MSG_ID",
					message: `MSG ID ${msg.msgId} used by both "${seenMsgIds.get(norm)}" and "${msg.name}"`,
					context: {
						first: seenMsgIds.get(norm),
						second: msg.name,
						id: msg.msgId,
					},
				});
			} else {
				seenMsgIds.set(norm, msg.name);
			}
		}

		// V6: DUPLICATE_GR_ID
		const seenGrIds = new Map();
		for (const [name, hexId] of _grIds.entries()) {
			const norm = hexId.toLowerCase();
			if (seenGrIds.has(norm)) {
				results.push({
					severity: "warning",
					code: "DUPLICATE_GR_ID",
					message: `GR ID ${hexId} shared by "${seenGrIds.get(norm)}" and "${name}"`,
					context: { first: seenGrIds.get(norm), second: name, id: hexId },
				});
			} else {
				seenGrIds.set(norm, name);
			}
		}

		// V7: EMPTY_DEVICE_BLOCK
		for (const device of _devices.values()) {
			if (device.buses.size === 0) {
				results.push({
					severity: "warning",
					code: "EMPTY_DEVICE_BLOCK",
					message: `Device "${device.deviceName}" has no bus entries in routing`,
					context: { device: device.deviceName },
				});
			}
		}

		// V8: PHYSICAL_BUS_VIOLATION
		// Only runs when PhysicalTopology has successfully loaded can_topology.json.
		const _topo = (typeof window !== "undefined" ? window : {})
			.PhysicalTopology;
		if (_topo && _topo.isLoaded()) {
			for (const device of _devices.values()) {
				for (const bus of device.buses.values()) {
					if (!_topo.isOnBus(device.deviceName, bus.busPort)) {
						results.push({
							severity: "warning",
							code: "PHYSICAL_BUS_VIOLATION",
							message: `Device "${device.deviceName}" is not physically wired to ${bus.busPort}`,
							context: {
								device: device.deviceName,
								bus: bus.busPort,
							},
						});
					}
					for (const receiverName of bus.receivers.keys()) {
						if (!_topo.isOnBus(receiverName, bus.busPort)) {
							results.push({
								severity: "warning",
								code: "PHYSICAL_BUS_VIOLATION",
								message: `Receiver "${receiverName}" in "${device.deviceName}" > ${bus.busPort} is not physically on ${bus.busPort}`,
								context: {
									device: device.deviceName,
									bus: bus.busPort,
									receiver: receiverName,
								},
							});
						}
					}
				}
			}
		}

		return results;
	}

	// ==================== Editor bridge ====================

	function _getEditor() {
		const g =
			typeof window !== "undefined"
				? window
				: typeof global !== "undefined"
					? global
					: {};
		return g.GrcanEditor || null;
	}

	function _ensureParsed() {
		const editor = _getEditor();
		if (editor && typeof editor.getRawText === "function") {
			_parse(editor.getRawText());
		}
		// else: test environment — use state already set by _parseForTest()
	}

	// Wraps a mutation fn: parse from editor → run fn → serialize back to editor.
	// fn() returns an OpResult. If ok is false, setRawText is NOT called.
	function _withEditor(fn) {
		const editor = _getEditor();
		if (!editor) {
			return { ok: false, error: "GrcanEditor not available" };
		}
		_parse(editor.getRawText());
		const result = fn();
		if (result.ok !== false) {
			const newText = _serialize();
			editor.updateRawText(newText);
			// If the post-mutation canonical text matches the canonical original,
			// the user has undone all their changes — clear the changed indicators.
			_parse(editor.getOriginalRawText());
			const canonicalOriginal = _serialize();
			_parse(newText); // restore internal state to current
			if (newText === canonicalOriginal) {
				editor.resetEditState();
			}
		}
		return result;
	}

	// ==================== Operations ====================

	function addDevice(name, grId) {
		return _withEditor(() => {
			if (!name || !name.trim())
				return { ok: false, error: "Device name is required" };
			if (name.trim().toUpperCase() === "ALL")
				return {
					ok: false,
					error:
						'"ALL" is a reserved broadcast receiver and cannot be used as a device name',
				};
			if (!grId || !/^0x[0-9a-fA-F]+$/i.test(grId.trim()))
				return {
					ok: false,
					error: "GR ID must be a hex value (e.g. 0x2B)",
				};
			const n = name.trim(),
				g = grId.trim();
			if (_devices.has(n))
				return { ok: false, error: `Device "${n}" already exists in routing` };
			if (_grIds.has(n))
				return {
					ok: false,
					error: `A GR ID entry for "${n}" already exists`,
				};
			_devices.set(n, { deviceName: n, buses: new Map() });
			_grIds.set(n, g);
			return { ok: true, warnings: [] };
		});
	}

	function deleteDevice(name) {
		return _withEditor(() => {
			if (!name) return { ok: false, error: "Device name is required" };
			const warnings = [];
			_devices.delete(name);
			_grIds.delete(name);

			// Receiver ref sweep — snapshot keys first to avoid mutation-during-iteration.
			for (const device of [..._devices.values()]) {
				for (const [busPort, bus] of [...device.buses.entries()]) {
					if (bus.receivers.has(name)) {
						bus.receivers.delete(name);
						warnings.push(
							`Removed receiver reference to "${name}" from ${device.deviceName} > ${busPort}`,
						);
						if (bus.receivers.size === 0) {
							device.buses.delete(busPort);
						}
					}
				}
			}
			return { ok: true, warnings };
		});
	}

	function renameDevice(oldName, newName) {
		return _withEditor(() => {
			if (!oldName || !newName)
				return { ok: false, error: "Both names are required" };
			if (oldName === newName) return { ok: true, warnings: [] };
			if (!_devices.has(oldName))
				return { ok: false, error: `Device "${oldName}" not found` };
			if (_devices.has(newName))
				return {
					ok: false,
					error: `Device "${newName}" already exists`,
				};
			if (_grIds.has(newName))
				return {
					ok: false,
					error: `GR ID entry for "${newName}" already exists`,
				};

			// Rebuild _devices map preserving insertion order.
			const newDevices = new Map();
			for (const [k, v] of _devices) {
				if (k === oldName) {
					v.deviceName = newName;
					newDevices.set(newName, v);
				} else {
					newDevices.set(k, v);
				}
			}
			_devices = newDevices;

			// Rebuild _grIds map preserving insertion order.
			const newGrIds = new Map();
			for (const [k, v] of _grIds) {
				newGrIds.set(k === oldName ? newName : k, v);
			}
			_grIds = newGrIds;

			// Receiver ref sweep across all remaining devices.
			for (const device of _devices.values()) {
				for (const bus of device.buses.values()) {
					if (bus.receivers.has(oldName)) {
						const block = bus.receivers.get(oldName);
						block.receiverName = newName;
						const newReceivers = new Map();
						for (const [k, v] of bus.receivers) {
							newReceivers.set(k === oldName ? newName : k, v);
						}
						bus.receivers = newReceivers;
					}
				}
			}

			return { ok: true, warnings: [] };
		});
	}

	function updateGrId(name, newGrId) {
		return _withEditor(() => {
			if (!name || !name.trim())
				return { ok: false, error: "Node name is required" };
			const n = name.trim();
			if (!newGrId || !/^0x[0-9a-fA-F]+$/i.test(newGrId.trim()))
				return {
					ok: false,
					error: "GR ID must be a hex value (e.g. 0x2B)",
				};
			_grIds.set(n, newGrId.trim());
			return { ok: true, warnings: [] };
		});
	}

	function addBus(deviceName, busPort) {
		return _withEditor(() => {
			deviceName = (deviceName || "").trim();
			busPort = (busPort || "").trim();
			if (!deviceName) return { ok: false, error: "Device name is required" };
			if (!_isValidBusPort(busPort)) {
				const valid = getBusNames();
				return {
					ok: false,
					error:
						valid.length > 0
							? `Bus must be one of: ${valid.join(", ")}`
							: "No buses are declared in the Bus ID section",
				};
			}

			const warnings = [];
			let device = _devices.get(deviceName);
			if (!device) {
				// Node exists in GR ID catalog but has no routing block yet — create stub.
				_devices.set(deviceName, { deviceName, buses: new Map() });
				device = _devices.get(deviceName);
			}
			if (!_grIds.has(deviceName))
				warnings.push(`Device "${deviceName}" has no GR ID entry.`);
			if (device.buses.has(busPort))
				return { ok: false, error: "Bus already exists for this node" };

			device.buses.set(busPort, { busPort, receivers: new Map() });
			return { ok: true, warnings };
		});
	}

	function addRoute(deviceName, busPort, receiverName, msgName, canIdOverride) {
		return _withEditor(() => {
			const warnings = [];
			if (!deviceName) return { ok: false, error: "Device name is required" };
			if (deviceName.trim().toUpperCase() === "ALL")
				return {
					ok: false,
					error:
						'"ALL" is a reserved broadcast receiver and cannot be used as a sender device',
				};
			if (!_isValidBusPort(busPort)) {
				const valid = getBusNames();
				return {
					ok: false,
					error:
						valid.length > 0
							? `Bus must be one of: ${valid.join(", ")}`
							: "No buses are declared in the Bus ID section",
				};
			}
			if (!receiverName)
				return { ok: false, error: "Receiver name is required" };
			if (!msgName) return { ok: false, error: "Message name is required" };

			const customCanIds = _getCustomCanIdNames();
			if (!_messageIds.has(msgName) && !customCanIds.has(msgName)) {
				return {
					ok: false,
					error: `Message "${msgName}" not found in Message ID or Custom CAN ID`,
				};
			}
			if (!_grIds.has(deviceName))
				warnings.push(
					`Device "${deviceName}" has no GR ID entry. Use addDevice first.`,
				);
			if (!_grIds.has(receiverName))
				warnings.push(`Receiver "${receiverName}" is not a known device.`);

			const ovr = canIdOverride || null;

			// Idempotency check.
			const existDev = _devices.get(deviceName);
			if (existDev) {
				const existBus = existDev.buses.get(busPort);
				if (existBus) {
					const existRec = existBus.receivers.get(receiverName);
					if (
						existRec &&
						existRec.routes.some(
							(r) =>
								r.msgName === msgName &&
								normalizeCanId(r.canIdOverride) === normalizeCanId(ovr),
						)
					) {
						return { ok: true, warnings };
					}
				}
			}

			if (!_devices.has(deviceName)) {
				_devices.set(deviceName, { deviceName, buses: new Map() });
			}
			const device = _devices.get(deviceName);
			if (!device.buses.has(busPort)) {
				device.buses.set(busPort, { busPort, receivers: new Map() });
			}
			const bus = device.buses.get(busPort);
			if (!bus.receivers.has(receiverName)) {
				bus.receivers.set(receiverName, { receiverName, routes: [] });
			}
			bus.receivers
				.get(receiverName)
				.routes.push({ msgName, canIdOverride: ovr });

			return { ok: true, warnings };
		});
	}

	function deleteRouteEntry(deviceName, busPort, msgName) {
		return _withEditor(() => {
			const device = _devices.get(deviceName);
			if (!device) return { ok: true, warnings: [] };
			const bus = device.buses.get(busPort);
			if (!bus) return { ok: true, warnings: [] };

			for (const [recName, receiver] of [...bus.receivers.entries()]) {
				receiver.routes = receiver.routes.filter((r) => r.msgName !== msgName);
				if (receiver.routes.length === 0) {
					bus.receivers.delete(recName);
				}
			}
			if (bus.receivers.size === 0) {
				device.buses.delete(busPort);
			}
			return { ok: true, warnings: [] };
		});
	}

	function deleteRouteFromReceiver(deviceName, busPort, receiverName, msgName) {
		return _withEditor(() => {
			const device = _devices.get(deviceName);
			if (!device) return { ok: true, warnings: [] };
			const bus = device.buses.get(busPort);
			if (!bus) return { ok: true, warnings: [] };
			const receiver = bus.receivers.get(receiverName);
			if (!receiver) return { ok: true, warnings: [] };
			receiver.routes = receiver.routes.filter((r) => r.msgName !== msgName);
			if (receiver.routes.length === 0) bus.receivers.delete(receiverName);
			if (bus.receivers.size === 0) device.buses.delete(busPort);
			return { ok: true, warnings: [] };
		});
	}

	function getRouteReceivers(deviceName, busPort, msgName) {
		const editor = _getEditor();
		if (!editor) return [];
		_parse(editor.getRawText());
		const device = _devices.get(deviceName);
		if (!device) return [];
		const bus = device.buses.get(busPort);
		if (!bus) return [];
		const result = [];
		for (const receiver of bus.receivers.values()) {
			const route = receiver.routes.find((r) => r.msgName === msgName);
			if (route)
				result.push({
					receiverName: receiver.receiverName,
					canIdOverride: route.canIdOverride,
				});
		}
		return result;
	}

	function deleteBusBlock(deviceName, busPort) {
		return _withEditor(() => {
			const device = _devices.get(deviceName);
			if (!device) return { ok: true, warnings: [] };
			device.buses.delete(busPort);
			return { ok: true, warnings: [] };
		});
	}

	function addMessageDef(def) {
		return _withEditor(() => {
			if (!def || !def.name)
				return { ok: false, error: "Message name is required" };
			if (_messageIds.has(def.name))
				return {
					ok: false,
					error: `Message "${def.name}" already exists`,
				};
			if (_customCanIds.has(def.name))
				return {
					ok: false,
					error: `Name "${def.name}" already exists in Custom CAN ID section`,
				};
			if (!/^0x[0-9a-fA-F]+$/i.test(def.msgId))
				return { ok: false, error: "MSG ID must be hex (e.g. 0x003)" };
			const normId = def.msgId.toLowerCase();
			for (const msg of _messageIds.values()) {
				if (msg.msgId.toLowerCase() === normId) {
					return {
						ok: false,
						error: `MSG ID ${def.msgId} is already used by "${msg.name}"`,
					};
				}
			}
			_messageIds.set(def.name, def);
			return { ok: true, warnings: [] };
		});
	}

	function updateMessageDef(oldName, def) {
		return _withEditor(() => {
			if (!_messageIds.has(oldName))
				return { ok: false, error: `Message "${oldName}" not found` };
			if (!def || !def.name)
				return { ok: false, error: "Message name is required" };
			if (def.name !== oldName && _messageIds.has(def.name))
				return {
					ok: false,
					error: `Message "${def.name}" already exists`,
				};

			const normId = def.msgId.toLowerCase();
			for (const [k, msg] of _messageIds) {
				if (k !== oldName && msg.msgId.toLowerCase() === normId) {
					return {
						ok: false,
						error: `MSG ID ${def.msgId} is already used by "${k}"`,
					};
				}
			}

			if (def.name === oldName) {
				_messageIds.set(oldName, def);
			} else {
				// Rename: rebuild map preserving insertion order.
				const newMsgIds = new Map();
				for (const [k, v] of _messageIds) {
					newMsgIds.set(k === oldName ? def.name : k, k === oldName ? def : v);
				}
				_messageIds = newMsgIds;

				// Route ref sweep.
				for (const device of _devices.values()) {
					for (const bus of device.buses.values()) {
						for (const receiver of bus.receivers.values()) {
							for (const route of receiver.routes) {
								if (route.msgName === oldName) route.msgName = def.name;
							}
						}
					}
				}
			}

			return { ok: true, warnings: [] };
		});
	}

	// ==================== Custom CAN ID mutations ====================

	function addCustomCanIdDef(def) {
		return _withEditor(() => {
			if (!def || !def.name)
				return { ok: false, error: "Message name is required" };
			if (_customCanIds.has(def.name))
				return {
					ok: false,
					error: `Custom CAN ID "${def.name}" already exists`,
				};
			if (_messageIds.has(def.name))
				return {
					ok: false,
					error: `Name "${def.name}" already exists in Message ID section`,
				};
			if (!def.canId || !def.canId.trim())
				return { ok: false, error: "CAN ID is required" };
			if (!/^[0-9a-fA-F]+$/i.test(def.canId.trim()))
				return {
					ok: false,
					error: "CAN ID must be a valid hex value (e.g. 116 or 18FF50E5)",
				};
			if (
				!def.length ||
				isNaN(parseInt(def.length, 10)) ||
				parseInt(def.length, 10) < 0
			)
				return { ok: false, error: "Length must be a non-negative integer" };
			_customCanIds.set(def.name, {
				name: def.name,
				canId: normalizeCanId(def.canId),
				length: String(parseInt(def.length, 10)),
				signals: (def.signals || []).map((s) => ({
					name: s.name,
					bitStart: s.bitStart,
					comment: s.comment || null,
				})),
			});
			return { ok: true, warnings: [] };
		});
	}

	function updateCustomCanIdDef(oldName, def) {
		return _withEditor(() => {
			if (!_customCanIds.has(oldName))
				return {
					ok: false,
					error: `Custom CAN ID "${oldName}" not found`,
				};
			if (!def || !def.name)
				return { ok: false, error: "Message name is required" };
			if (def.name !== oldName && _customCanIds.has(def.name))
				return {
					ok: false,
					error: `Custom CAN ID "${def.name}" already exists`,
				};
			if (def.name !== oldName && _messageIds.has(def.name))
				return {
					ok: false,
					error: `Name "${def.name}" already exists in Message ID section`,
				};
			if (!def.canId || !def.canId.trim())
				return { ok: false, error: "CAN ID is required" };
			if (!/^[0-9a-fA-F]+$/i.test(def.canId.trim()))
				return {
					ok: false,
					error: "CAN ID must be a valid hex value (e.g. 116 or 18FF50E5)",
				};
			if (
				!def.length ||
				isNaN(parseInt(def.length, 10)) ||
				parseInt(def.length, 10) < 0
			)
				return { ok: false, error: "Length must be a non-negative integer" };

			const oldCanId = normalizeCanId(_customCanIds.get(oldName).canId);
			const newCanId = normalizeCanId(def.canId);

			const newEntry = {
				name: def.name,
				canId: newCanId,
				length: String(parseInt(def.length, 10)),
				signals: (def.signals || []).map((s) => ({
					name: s.name,
					bitStart: s.bitStart,
					comment: s.comment || null,
				})),
			};

			if (def.name === oldName) {
				_customCanIds.set(oldName, newEntry);
			} else {
				// Rename: rebuild map preserving insertion order.
				const newMap = new Map();
				for (const [k, v] of _customCanIds) {
					newMap.set(
						k === oldName ? def.name : k,
						k === oldName ? newEntry : v,
					);
				}
				_customCanIds = newMap;

				// Route msgName ref sweep.
				for (const device of _devices.values()) {
					for (const bus of device.buses.values()) {
						for (const receiver of bus.receivers.values()) {
							for (const route of receiver.routes) {
								if (route.msgName === oldName) route.msgName = def.name;
							}
						}
					}
				}
			}

			// canIdOverride sweep: update all routing overrides that matched the old
			// CAN ID so the two sections stay in sync when the CAN ID value changes.
			if (oldCanId !== newCanId) {
				const oldOverride = toCanIdOverride(oldCanId);
				const newOverride = toCanIdOverride(newCanId);
				for (const device of _devices.values()) {
					for (const bus of device.buses.values()) {
						for (const receiver of bus.receivers.values()) {
							for (const route of receiver.routes) {
								if (
									route.msgName === def.name &&
									route.canIdOverride === oldOverride
								) {
									route.canIdOverride = newOverride;
								}
							}
						}
					}
				}
			}

			return { ok: true, warnings: [] };
		});
	}

	function deleteCustomCanIdDef(name) {
		return _withEditor(() => {
			if (!name) return { ok: false, error: "Message name is required" };
			const warnings = [];
			_customCanIds.delete(name);

			// Sweep routing references.
			for (const device of _devices.values()) {
				for (const bus of device.buses.values()) {
					for (const [recName, receiver] of [...bus.receivers.entries()]) {
						const before = receiver.routes.length;
						receiver.routes = receiver.routes.filter((r) => r.msgName !== name);
						if (receiver.routes.length < before) {
							warnings.push(
								`Removed route to "${name}" from ${device.deviceName} > ${bus.busPort} > ${recName}`,
							);
						}
						if (receiver.routes.length === 0) {
							bus.receivers.delete(recName);
						}
					}
					if (bus.receivers.size === 0) {
						device.buses.delete(bus.busPort);
					}
				}
			}

			return { ok: true, warnings };
		});
	}

	// ==================== Read-only accessors ====================

	function deviceExists(name) {
		_ensureParsed();
		return _devices.has(name);
	}

	function grIdExists(name) {
		_ensureParsed();
		return _grIds.has(name);
	}

	function getDeviceNames() {
		_ensureParsed();
		return [..._devices.keys()];
	}

	// Returns bus names declared in the "Bus ID:" section, sorted by their
	// numeric id (lowest first). This is the single source of truth for
	// "what buses exist" — used by forms, graph view, and validation.
	function getBusNames() {
		_ensureParsed();
		return [..._busIds.entries()]
			.sort((a, b) => a[1] - b[1])
			.map(([name]) => name);
	}

	// True if busPort is a name declared in the "Bus ID:" section.
	function _isValidBusPort(busPort) {
		return _busIds.has(busPort);
	}

	function getGrIds() {
		_ensureParsed();
		return new Map(_grIds);
	}

	function getGrId(name) {
		_ensureParsed();
		return _grIds.get(name) || null;
	}

	function getMessageDef(name) {
		_ensureParsed();
		return _messageIds.get(name) || null;
	}

	function getMessageIdNames() {
		_ensureParsed();
		return [..._messageIds.keys()];
	}

	function getCustomCanIdDef(name) {
		_ensureParsed();
		return _customCanIds.get(name) || null;
	}

	function getCustomCanIdNames() {
		_ensureParsed();
		return [..._customCanIds.keys()];
	}

	function getGraphDataForBus(busPort) {
		_ensureParsed();
		const nodeSet = new Set();
		const edgeMap = new Map();
		for (const [senderName, device] of _devices) {
			const busBlock = device.buses.get(busPort);
			if (!busBlock) continue;
			nodeSet.add(senderName);
			for (const [receiverName, receiverBlock] of busBlock.receivers) {
				nodeSet.add(receiverName);
				const key = `${senderName}__${receiverName}`;
				const msgs = receiverBlock.routes.map((r) => r.msgName);
				edgeMap.set(key, {
					id: key,
					source: senderName,
					target: receiverName,
					messages: msgs,
					count: msgs.length,
				});
			}
		}
		const nodes = [...nodeSet].map((id) => ({
			id,
			label: id,
			grId: _grIds.get(id) || null,
		}));
		const edges = [...edgeMap.values()];
		return { nodes, edges };
	}

	function routeEntryExists(device, bus, receiver, msg, canIdOverride) {
		_ensureParsed();
		const dev = _devices.get(device);
		if (!dev) return false;
		const b = dev.buses.get(bus);
		if (!b) return false;
		const rec = b.receivers.get(receiver);
		if (!rec) return false;
		return rec.routes.some(
			(r) =>
				r.msgName === msg &&
				normalizeCanId(r.canIdOverride) ===
					normalizeCanId(canIdOverride || null),
		);
	}

	// ==================== Test helpers ====================
	// Exposed only for Node.js test environment.

	function _parseForTest(text) {
		_parse(text);
		return {
			devices: _devices,
			grIds: _grIds,
			messageIds: _messageIds,
			customCanIds: _customCanIds,
		};
	}

	function _serializeFromState() {
		return _serialize();
	}

	function _serializeFromStatePruned() {
		return _serialize(true);
	}

	// Returns the canonical serialized form of the current editor text.
	// Parse → serialize without side effects (does not update editor state).
	function getSerializedText() {
		const editor = _getEditor();
		if (!editor) return null;
		_parse(editor.getRawText());
		return _serialize(true);
	}

	// Parse an arbitrary raw text and serialize it with pruning — used to compute
	// the canonical download form of any snapshot (e.g. the original file).
	function getSerializedTextFrom(rawText) {
		_parse(rawText || "");
		return _serialize(true);
	}

	// ==================== Public API ====================

	return {
		// Mutations
		addDevice,
		deleteDevice,
		renameDevice,
		updateGrId,
		addBus,
		addRoute,
		deleteRouteEntry,
		deleteRouteFromReceiver,
		deleteBusBlock,
		addMessageDef,
		updateMessageDef,
		addCustomCanIdDef,
		updateCustomCanIdDef,
		deleteCustomCanIdDef,
		// Validation
		validate,
		// Read-only
		deviceExists,
		grIdExists,
		getDeviceNames,
		getBusNames,
		getGrIds,
		getGrId,
		getMessageDef,
		getMessageIdNames,
		getCustomCanIdDef,
		getCustomCanIdNames,
		routeEntryExists,
		getRouteReceivers,
		getGraphDataForBus,
		getSerializedText,
		getSerializedTextFrom,
		// Test hooks
		_parseForTest,
		_serializeFromState,
		_serializeFromStatePruned,
	};
});
