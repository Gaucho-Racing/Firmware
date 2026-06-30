// Purpose: Data acquisition and parse helpers for GRCAN.CANdo.
// Fetches branches, tags, and file contents from the GitHub API, and provides
// pure text parsers (parseMessageByBusFromText, parseBusIdsFromText,
// parseNodeIdsFromText, parseMessageDefinitions) used by viewer.js for local
// re-renders after edits. No DOM manipulation; no edit state.
// Exposed as: window.GrcanApi

const GR_PINK = "#EF0DA1";
const GR_PURPLE = "#7920FF";
const GR_NAVY = "#195297";
const GR_GRAY = "#9AA3B0";

const GITHUB_API = "https://api.github.com/repos/Gaucho-Racing/Firmware";

// Local-file mode: when set, fetchCando returns this content instead of hitting the API.
let _localCandoText = null;
function setLocalCandoText(text) {
	_localCandoText = text;
}
function getLocalCandoText() {
	return _localCandoText;
}
function isLocalMode() {
	return _localCandoText !== null;
}
const CANDO_PATH = "Autogen/CAN/Doc/GRCAN.CANdo";
const BUS_ID_PATH = "Autogen/CAN/Inc/GRCAN_BUS_ID.h";
const NODE_ID_PATH = "Autogen/CAN/Inc/GRCAN_NODE_ID.h";
const MSG_ID_PATH = "Autogen/CAN/Inc/GRCAN_MSG_ID.h";
const CUSTOM_ID_PATH = "Autogen/CAN/Inc/GRCAN_CUSTOM_ID.h";

function parseBitRange(rawBitStart) {
	const cleaned = String(rawBitStart || "")
		.replace(/,/g, "")
		.trim();
	if (/^\d+$/.test(cleaned)) {
		const n = parseInt(cleaned, 10);
		return { start: n, end: n };
	}
	const rangeMatch = cleaned.match(/^(\d+)\s*-\s*(\d+)$/);
	if (!rangeMatch) return null;
	const start = parseInt(rangeMatch[1], 10);
	const end = parseInt(rangeMatch[2], 10);
	return { start, end };
}

function parseMessageDefinitions(candoText) {
	const lines = candoText.split("\n");
	const start = lines.findIndex((l) => l.startsWith("Message ID:"));
	if (start === -1) return new Map();

	const end = lines.findIndex((l, i) => i > start + 1 && /^\S/.test(l));
	const section = lines.slice(start + 1, end === -1 ? undefined : end);
	const defs = new Map();

	let currentMsg = null;
	let currentField = null;

	function pushField() {
		if (!currentMsg || !currentField) return;
		currentMsg.fields.push(currentField);
		currentField = null;
	}

	for (const raw of section) {
		const indent = raw.search(/\S/);
		if (indent === -1) continue;
		const content = raw.trim();

		if (indent === 2 && content.endsWith(":")) {
			pushField();
			const msgName = content.replace(/:$/, "");
			currentMsg = { msgId: null, msgLength: null, fields: [] };
			defs.set(msgName, currentMsg);
			continue;
		}

		if (!currentMsg) continue;

		if (indent === 4) {
			if (content.startsWith("MSG ID:")) {
				currentMsg.msgId = content.split(":")[1].trim();
				continue;
			}
			if (content.startsWith("MSG LENGTH:")) {
				const rawLen = content.split(":")[1].trim().replace(/,/g, "");
				currentMsg.msgLength = /^\d+$/.test(rawLen)
					? parseInt(rawLen, 10)
					: null;
				continue;
			}
			if (content.endsWith(":")) {
				pushField();
				currentField = {
					fieldName: content.replace(/:$/, ""),
					bitStart: null,
					bitEnd: null,
					dataType: null,
					comment: null,
					scaledMin: null,
					scaledMax: null,
					mapEquation: null,
				};
				continue;
			}
		}

		if (indent >= 6 && currentField) {
			if (content.startsWith("bit_start:")) {
				const rawBits = content.slice("bit_start:".length).trim();
				const parsed = parseBitRange(rawBits);
				if (parsed) {
					currentField.bitStart = parsed.start;
					currentField.bitEnd = parsed.end;
				}
				continue;
			}
			if (content.startsWith("#")) {
				const lineComment = content.replace(/^#\s*/, "").trim();
				if (!lineComment) continue;
				currentField.comment = currentField.comment
					? `${currentField.comment} ${lineComment}`
					: lineComment;
				continue;
			}
			if (content.startsWith("data type:")) {
				const rawType = content.slice("data type:".length).trim();
				if (rawType === "s") {
					currentField.dataType = "string";
				} else if (rawType === "i16") {
					currentField.dataType = "s16";
				} else if (rawType === "i32") {
					currentField.dataType = "s32";
				} else {
					currentField.dataType = rawType || null;
				}
			}
			if (content.startsWith("scaled min:")) {
				const v = content.slice("scaled min:".length).trim().replace(/,/g, "");
				currentField.scaledMin = v || null;
			}
			if (content.startsWith("scaled max:")) {
				const v = content.slice("scaled max:".length).trim().replace(/,/g, "");
				currentField.scaledMax = v || null;
			}
			if (content.startsWith("map equation:")) {
				const v = content
					.slice("map equation:".length)
					.trim()
					.replace(/^["']|["']$/g, "");
				currentField.mapEquation = v || null;
			}
		}
	}

	pushField();

	for (const def of defs.values()) {
		def.byteMappings = def.fields
			.filter(
				(f) => typeof f.bitStart === "number" && typeof f.bitEnd === "number",
			)
			.map((f) => {
				const byteStart = Math.floor(f.bitStart / 8);
				const byteEnd = Math.floor(f.bitEnd / 8);
				return {
					fieldName: f.fieldName,
					byteStart,
					byteEnd,
					byteLabel:
						byteStart === byteEnd ? `${byteStart}` : `${byteStart}-${byteEnd}`,
					bitLabel:
						f.bitStart === f.bitEnd
							? `${f.bitStart}`
							: `${f.bitStart}-${f.bitEnd}`,
					dataType: f.dataType,
					comment: f.comment,
					scaledMin: f.scaledMin ?? null,
					scaledMax: f.scaledMax ?? null,
					mapEquation: f.mapEquation ?? null,
				};
			});
	}

	return defs;
}

function isValidSha(str) {
	if (typeof str !== "string") return false;
	if (str.length < 7 || str.length > 40) return false;
	for (let i = 0; i < str.length; ++i) {
		const c = str[i];
		if (!(
			(c >= "0" && c <= "9") ||
			(c >= "a" && c <= "f") ||
			(c >= "A" && c <= "F")
		)) {
			return false;
		}
	}
	return true;
}

function normalizeCatalogName(name) {
	return String(name || "")
		.toLowerCase()
		.replace(/[^a-z0-9]/g, "");
}

function sectionLinesByHeader(candoText, headerName) {
	const lines = String(candoText || "").split("\n");
	const start = lines.findIndex((l) => l.startsWith(headerName + ":"));
	if (start === -1) return [];
	const end = lines.findIndex((l, i) => i > start + 1 && /^\S/.test(l));
	return lines.slice(start + 1, end === -1 ? undefined : end);
}

function parseMessageCatalogFromText(candoText) {
	const names = [];
	const seen = new Set();
	function collectFromSection(sectionHeader) {
		const section = sectionLinesByHeader(candoText, sectionHeader);
		section.forEach((raw) => {
			const indent = raw.search(/\S/);
			const content = raw.trim();
			if (indent === 2 && content.endsWith(":")) {
				const name = content.replace(/:$/, "").trim();
				if (!name || seen.has(name)) return;
				seen.add(name);
				names.push(name);
			}
		});
	}
	collectFromSection("Message ID");
	collectFromSection("Custom CAN ID");
	return names;
}

function parseNodeCatalogFromText(candoText) {
	const section = sectionLinesByHeader(candoText, "GR ID");
	const names = [];
	const seen = new Set();
	section.forEach((line) => {
		const m = line.match(/^\s+([^:]+):\s*["']?([^"'\s]+)["']?\s*(?:#.*)?$/);
		if (!m) return;
		const name = m[1].trim();
		if (!name || seen.has(name)) return;
		seen.add(name);
		names.push(name);
	});
	return names;
}

function parseMsgIdHeaderNames(headerText) {
	const names = [];
	const seen = new Set();
	String(headerText || "")
		.split("\n")
		.forEach((line) => {
			const m = line.match(/^\s*MSG_([A-Z0-9_]+)\s*=/);
			if (!m) return;
			const human = m[1].replace(/_/g, " ").trim();
			if (!human || seen.has(human)) return;
			seen.add(human);
			names.push(human);
		});
	return names;
}

function parseCustomIdHeaderNames(headerText) {
	const names = [];
	const seen = new Set();
	String(headerText || "")
		.split("\n")
		.forEach((line) => {
			const m = line.match(/^\s*([A-Z0-9_]+)_CAN_ID\s*=/);
			if (!m) return;
			const human = m[1].replace(/_/g, " ").trim();
			if (!human || seen.has(human)) return;
			seen.add(human);
			names.push(human);
		});
	return names;
}

function reconcileCatalogNames(preferredNames, canonicalNames) {
	const byCanonical = new Map();
	preferredNames.forEach((name) => {
		const key = normalizeCatalogName(name);
		if (!key || byCanonical.has(key)) return;
		byCanonical.set(key, name);
	});
	const resolved = [];
	const seen = new Set();
	canonicalNames.forEach((name) => {
		const key = normalizeCatalogName(name);
		const preferred = byCanonical.get(key) || name;
		if (!preferred || seen.has(preferred)) return;
		seen.add(preferred);
		resolved.push(preferred);
	});
	return resolved;
}

function decodeBase64Utf8(b64) {
	const bytes = Uint8Array.from(atob(b64), (c) => c.charCodeAt(0));
	return new TextDecoder().decode(bytes);
}

async function fetchRepoText(path, ref) {
	const res = await fetch(
		`${GITHUB_API}/contents/${path}?ref=${encodeURIComponent(ref)}`,
	);
	if (res.status === 403) return { text: null, error: "rate_limited" };
	if (res.status === 404) return { text: null, error: "not_found" };
	if (!res.ok) return { text: null, error: "fetch_failed" };
	const data = await res.json();
	if (data.encoding !== "base64") return { text: null, error: "encoding" };
	return {
		text: decodeBase64Utf8(data.content.replace(/\n/g, "")),
		error: null,
	};
}

async function fetchBranches() {
	try {
		const res = await fetch(`${GITHUB_API}/branches`);
		if (!res.ok) {
			return "RATE_LIMITED";
		}
		const branches = await res.json();
		return branches.map((b) => b.name);
	} catch (e) {
		return "RATE_LIMITED";
	}
}

async function fetchTags() {
	try {
		const res = await fetch(`${GITHUB_API}/tags`);
		if (!res.ok) {
			return "RATE_LIMITED";
		}
		const tags = await res.json();
		return tags.map((t) => t.name);
	} catch (e) {
		return "RATE_LIMITED";
	}
}

async function fetchCando(ref) {
	if (_localCandoText !== null) {
		return { content: _localCandoText, notFound: false };
	}
	try {
		const res = await fetch(
			`${GITHUB_API}/contents/${CANDO_PATH}?ref=${encodeURIComponent(ref)}`,
		);
		if (res.status === 403) {
			return { content: "[GitHub API rate limited]", notFound: false };
		}
		if (res.status === 404) {
			return {
				content: "[Unable to load GRCAN.CANdo for this reference]",
				notFound: true,
			};
		}
		if (!res.ok) throw new Error("File not found");
		const data = await res.json();
		if (data.encoding === "base64") {
			const decoded = decodeBase64Utf8(data.content.replace(/\n/g, ""));
			return { content: decoded, notFound: false };
		} else {
			return { content: "[Unsupported file encoding]", notFound: true };
		}
	} catch (e) {
		return {
			content: "[Unable to load GRCAN.CANdo for this reference]",
			notFound: true,
		};
	}
}

async function fetchBus(ref) {
	try {
		const res = await fetch(
			`${GITHUB_API}/contents/${BUS_ID_PATH}?ref=${encodeURIComponent(ref)}`,
		);
		if (res.status === 403) return { buses: null, error: "rate_limited" };
		if (res.status === 404) return { buses: null, error: "not_found" };
		if (!res.ok) throw new Error("Unexpected response");
		const data = await res.json();
		if (data.encoding !== "base64") return { buses: null, error: "encoding" };
		const text = decodeBase64Utf8(data.content.replace(/\n/g, ""));
		const buses = [];
		const enumBody = text.match(/typedef\s+enum\s*\{([^}]*)\}/s);
		if (enumBody) {
			const commentRe = /\/\*\*\s*(.*?)\s*\*\//g;
			const entryRe = /(\w+)\s*=\s*(\d+)/g;
			const body = enumBody[1];
			const comments = [];
			let cm;
			while ((cm = commentRe.exec(body)) !== null) {
				comments.push({ index: cm.index, label: cm[1] });
			}
			let em;
			let commentIdx = 0;
			while ((em = entryRe.exec(body)) !== null) {
				while (
					commentIdx + 1 < comments.length &&
					comments[commentIdx + 1].index < em.index
				) {
					commentIdx++;
				}
				const label =
					comments[commentIdx] && comments[commentIdx].index < em.index
						? comments[commentIdx].label
						: null;
				buses.push({ name: em[1], id: parseInt(em[2], 10), label });
				commentIdx++;
			}
		}
		return { buses, error: null };
	} catch (e) {
		return { buses: null, error: "fetch_failed" };
	}
}

async function fetchNodeIds(ref) {
	try {
		const res = await fetch(
			`${GITHUB_API}/contents/${NODE_ID_PATH}?ref=${encodeURIComponent(ref)}`,
		);
		if (res.status === 403) return { nodeIds: null, error: "rate_limited" };
		if (res.status === 404) return { nodeIds: null, error: "not_found" };
		if (!res.ok) throw new Error("Unexpected response");
		const data = await res.json();
		if (data.encoding !== "base64") return { nodeIds: null, error: "encoding" };
		const text = decodeBase64Utf8(data.content.replace(/\n/g, ""));

		const nodeIds = [];
		const enumBody = text.match(/typedef\s+enum\s*\{([^}]*)\}/s);
		if (enumBody) {
			const entryRe = /(\w+)\s*=\s*(0x[0-9a-fA-F]+|\d+)/g;
			let em;
			while ((em = entryRe.exec(enumBody[1])) !== null) {
				nodeIds.push({ name: em[1], id: em[2] });
			}
		}
		return { nodeIds, error: null };
	} catch (e) {
		return { nodeIds: null, error: "fetch_failed" };
	}
}

async function fetchMessageCatalog(ref) {
	try {
		const [msgHeader, customHeader, candoFile] = await Promise.all([
			fetchRepoText(MSG_ID_PATH, ref),
			fetchRepoText(CUSTOM_ID_PATH, ref),
			fetchCando(ref),
		]);
		if (msgHeader.error) return { messages: null, error: msgHeader.error };
		if (customHeader.error && customHeader.error !== "not_found")
			return { messages: null, error: customHeader.error };
		const fromHeaders = [
			...parseMsgIdHeaderNames(msgHeader.text || ""),
			...parseCustomIdHeaderNames(customHeader.text || ""),
		];
		const headerUnique = [...new Set(fromHeaders)];
		if (candoFile && !candoFile.notFound && candoFile.content) {
			const exactNames = parseMessageCatalogFromText(candoFile.content);
			return {
				messages: reconcileCatalogNames(exactNames, headerUnique),
				error: null,
			};
		}
		return { messages: headerUnique, error: null };
	} catch (e) {
		return { messages: null, error: "fetch_failed" };
	}
}

async function fetchNodeCatalog(ref) {
	try {
		const [nodeHeader, candoFile] = await Promise.all([
			fetchRepoText(NODE_ID_PATH, ref),
			fetchCando(ref),
		]);
		if (nodeHeader.error) return { nodes: null, error: nodeHeader.error };
		const fromHeader = [];
		String(nodeHeader.text || "")
			.split("\n")
			.forEach((line) => {
				const m = line.match(
					/^\s*([A-Za-z_][A-Za-z0-9_]*)\s*=\s*(0x[0-9A-Fa-f]+|\d+)/,
				);
				if (!m) return;
				fromHeader.push(m[1].replace(/_/g, " ").trim());
			});
		const headerUnique = [...new Set(fromHeader)];
		if (candoFile && !candoFile.notFound && candoFile.content) {
			const exactNames = parseNodeCatalogFromText(candoFile.content);
			return {
				nodes: reconcileCatalogNames(exactNames, headerUnique),
				error: null,
			};
		}
		return { nodes: headerUnique, error: null };
	} catch (e) {
		return { nodes: null, error: "fetch_failed" };
	}
}

async function fetchBusCatalog(ref) {
	const result = await fetchBus(ref);
	if (result.error) return { buses: null, error: result.error };
	return { buses: result.buses || [], error: null };
}

function parseMessageByBusFromText(text, busName) {
	const messageDefs = parseMessageDefinitions(text);

	if (!busName) return { nodes: null, error: "unknown_bus" };
	const declared = parseBusIdsFromText(text).buses || [];
	const targetBus = String(busName).toLowerCase();
	const known = declared.some(
		(b) => String(b.name).toLowerCase() === targetBus,
	);
	if (!known) return { nodes: null, error: "unknown_bus" };

	const lines = text.split("\n");
	const routingStart = lines.findIndex((l) => l.startsWith("routing:"));
	if (routingStart === -1) return { nodes: [], error: null };

	const msgSectionStart = lines.findIndex(
		(l, i) => i > routingStart && l.trim() === "messages:",
	);
	const nextTopLevel = lines.findIndex(
		(l, i) => i > routingStart + 1 && /^\S/.test(l),
	);
	const routingLines = lines.slice(
		msgSectionStart + 1,
		nextTopLevel === -1 ? undefined : nextTopLevel,
	);

	const nodeMap = new Map();
	let currentNode = null;
	let onTargetPort = false;
	let receiver = null;
	let pendingMsg = null;

	for (const raw of routingLines) {
		const indent = raw.search(/\S/);
		if (indent === -1) continue;
		const content = raw.trim();

		if (indent === 4) {
			const senderName = content.replace(/:$/, "");
			if (!nodeMap.has(senderName))
				nodeMap.set(senderName, {
					name: senderName,
					messages: [],
					hasBus: false,
				});
			currentNode = nodeMap.get(senderName);
			onTargetPort = false;
			receiver = null;
			pendingMsg = null;
		} else if (indent === 6) {
			onTargetPort = content.replace(/:$/, "").toLowerCase() === targetBus;
			if (onTargetPort && currentNode) currentNode.hasBus = true;
			receiver = null;
			pendingMsg = null;
		} else if (onTargetPort && indent === 8) {
			receiver = content.replace(/:$/, "");
			pendingMsg = null;
		} else if (onTargetPort && indent === 10 && content.startsWith("- msg:")) {
			const msgName = content.replace("- msg:", "").trim();
			const msgDef = messageDefs.get(msgName);
			const existing = currentNode.messages.find((m) => m.msgName === msgName);
			if (existing) {
				if (!existing.receivers.includes(receiver))
					existing.receivers.push(receiver);
				pendingMsg = null;
			} else {
				pendingMsg = {
					msgName,
					canIdOverride: null,
					receivers: [receiver],
					msgId: msgDef ? msgDef.msgId : null,
					msgLength: msgDef ? msgDef.msgLength : null,
					byteMappings: msgDef ? msgDef.byteMappings : [],
				};
				currentNode.messages.push(pendingMsg);
			}
		} else if (
			onTargetPort &&
			indent === 12 &&
			content.startsWith("can_id_override:") &&
			pendingMsg
		) {
			pendingMsg.canIdOverride = content.split(":")[1].trim();
		}
	}

	const nodes = [...nodeMap.values()];
	return { nodes, error: null };
}

async function fetchMessageByBus(ref, busName) {
	if (_localCandoText !== null) {
		return parseMessageByBusFromText(_localCandoText, busName);
	}
	try {
		const res = await fetch(
			`${GITHUB_API}/contents/${CANDO_PATH}?ref=${encodeURIComponent(ref)}`,
		);
		if (res.status === 403) return { nodes: null, error: "rate_limited" };
		if (res.status === 404) return { nodes: null, error: "not_found" };
		if (!res.ok) throw new Error("Unexpected response");
		const data = await res.json();
		if (data.encoding !== "base64") return { nodes: null, error: "encoding" };
		const text = decodeBase64Utf8(data.content.replace(/\n/g, ""));
		return parseMessageByBusFromText(text, busName);
	} catch (e) {
		return { nodes: null, error: "fetch_failed" };
	}
}

function parseBusIdsFromText(candoText) {
	const lines = candoText.split("\n");
	const startIdx = lines.findIndex((l) => l.startsWith("Bus ID:"));
	if (startIdx === -1) return { buses: [], error: null };
	const buses = [];
	let pendingBusName = null; // for nested format: "  BusName:" then "    id: N"
	for (let i = startIdx + 1; i < lines.length; i++) {
		const line = lines[i];
		if (/^\S/.test(line) && line.trim() !== "") break;
		// Old flat format: "  BusName: 0" or "  BusName: 0  # label"
		// \S after the two leading spaces ensures we don't match deeper-indented lines.
		const flatMatch = line.match(/^  (\S[^:]*):\s*(\d+)\s*(?:#\s*(.*))?$/);
		if (flatMatch) {
			buses.push({
				name: flatMatch[1].trim(),
				id: parseInt(flatMatch[2], 10),
				label: (flatMatch[3] || flatMatch[1]).trim(),
			});
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
				buses.push({
					name: pendingBusName,
					id: parseInt(idMatch[1], 10),
					label: pendingBusName,
				});
				pendingBusName = null;
			}
		}
	}
	return { buses, error: null };
}

function parseNodeIdsFromText(candoText) {
	const lines = candoText.split("\n");
	const startIdx = lines.findIndex((l) => l.startsWith("GR ID:"));
	if (startIdx === -1) return { nodeIds: [], error: null };
	const nodeIds = [];
	for (let i = startIdx + 1; i < lines.length; i++) {
		const line = lines[i];
		if (/^\S/.test(line) && line.trim() !== "") break;
		const match = line.match(/^\s+([^:]+):\s*["']?([^"'\s]+)["']?/);
		if (match) {
			nodeIds.push({
				name: match[1].trim().replace(/[^a-zA-Z0-9]/g, "_"),
				id: match[2].trim(),
			});
		}
	}
	return { nodeIds, error: null };
}

window.GrcanApi = {
	isValidSha,
	decodeBase64Utf8,
	fetchBranches,
	fetchTags,
	fetchCando,
	setLocalCandoText,
	getLocalCandoText,
	isLocalMode,
	fetchBus,
	fetchNodeIds,
	fetchMessageCatalog,
	fetchNodeCatalog,
	fetchBusCatalog,
	fetchMessageByBus,
	parseMessageByBusFromText,
	parseMessageCatalogFromText,
	parseBusIdsFromText,
	parseNodeCatalogFromText,
	parseNodeIdsFromText,
};
