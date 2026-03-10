const GR_PINK = "#EF0DA1";
const GR_PURPLE = "#7920FF";
const GR_NAVY = "#195297";
const GR_GRAY = "#9AA3B0";

const GITHUB_API = "https://api.github.com/repos/Gaucho-Racing/Firmware";
const CANDO_PATH = "Autogen/CAN/Doc/GRCAN.CANdo";
const BUS_ID_PATH =
	"Lib/FancyLayers-RENAME/GRCAN/TemporaryHoldover/Inc/GR_OLD_BUS_ID.h";
const NODE_ID_PATH = "Autogen/CAN/Inc/GRCAN_NODE_ID.h";

// Maps CANdo CAN port names to logical bus names as used in routing section
const CAN_PORT_TO_BUS = {
	CAN1: "Primary",
	CAN2: "Data",
	CAN3: "Charger",
};

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
				currentField.dataType = rawType === "s" ? "string" : rawType || null;
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
		if (
			!(
				(c >= "0" && c <= "9") ||
				(c >= "a" && c <= "f") ||
				(c >= "A" && c <= "F")
			)
		) {
			return false;
		}
	}
	return true;
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
			const decoded = atob(data.content.replace(/\n/g, ""));
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
		const text = atob(data.content.replace(/\n/g, ""));
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
		const text = atob(data.content.replace(/\n/g, ""));

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

// Returns { nodes, error } where nodes is an array of
// { name, messages[] } grouped by sender node, for all messages
// routed on the given busName (e.g. "Primary", "Data", "Charger").
// Each message entry: { msgName, canIdOverride, receivers[] }
async function fetchMessageByBus(ref, busName) {
	try {
		const res = await fetch(
			`${GITHUB_API}/contents/${CANDO_PATH}?ref=${encodeURIComponent(ref)}`,
		);
		if (res.status === 403) return { nodes: null, error: "rate_limited" };
		if (res.status === 404) return { nodes: null, error: "not_found" };
		if (!res.ok) throw new Error("Unexpected response");
		const data = await res.json();
		if (data.encoding !== "base64") return { nodes: null, error: "encoding" };
		const text = atob(data.content.replace(/\n/g, ""));
		const messageDefs = parseMessageDefinitions(text);

		const targetPort = Object.entries(CAN_PORT_TO_BUS).find(
			([, bus]) => bus.toLowerCase() === busName.toLowerCase(),
		)?.[0];
		if (!targetPort) return { nodes: null, error: "unknown_bus" };

		const lines = text.split("\n");
		const routingStart = lines.findIndex((l) => l.startsWith("routing:"));
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

		// nodeMap: senderName -> { name, messages[] }
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
					nodeMap.set(senderName, { name: senderName, messages: [] });
				currentNode = nodeMap.get(senderName);
				onTargetPort = false;
				receiver = null;
				pendingMsg = null;
			} else if (indent === 6) {
				onTargetPort = content.replace(/:$/, "") === targetPort;
				receiver = null;
				pendingMsg = null;
			} else if (onTargetPort && indent === 8) {
				receiver = content.replace(/:$/, "");
				pendingMsg = null;
			} else if (
				onTargetPort &&
				indent === 10 &&
				content.startsWith("- msg:")
			) {
				const msgName = content.replace("- msg:", "").trim();
				const msgDef = messageDefs.get(msgName);
				const existing = currentNode.messages.find(
					(m) => m.msgName === msgName,
				);
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

		// Only return nodes that actually sent something on this bus
		const nodes = [...nodeMap.values()].filter((n) => n.messages.length > 0);
		return { nodes, error: null };
	} catch (e) {
		return { nodes: null, error: "fetch_failed" };
	}
}

window.GrcanApi = {
	CAN_PORT_TO_BUS,
	isValidSha,
	fetchBranches,
	fetchTags,
	fetchCando,
	fetchBus,
	fetchNodeIds,
	fetchMessageByBus,
};
