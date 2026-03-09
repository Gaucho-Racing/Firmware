window.addEventListener("DOMContentLoaded", function () {
	const HIERARCHY_MODE = "NODE_BUS"; // "NODE_BUS" or "BUS_NODE"

	const refSelect = document.getElementById("ref-select");
	const firstList = document.getElementById("bus-list");
	const secondList = document.getElementById("node-list");
	const msgList = document.getElementById("msg-list");
	const refInputRow = document.getElementById("ref-input-row");
	const firstHeader = document.querySelector("#bus-panel .panel-header");
	const secondHeader = document.querySelector("#node-panel .panel-header");
	let nodeIdMap = new Map();

	if (!window.GrcanApi || !refSelect || !firstList || !secondList || !msgList) {
		return;
	}

	function setPlaceholder(el, text) {
		el.innerHTML = `<span class="placeholder">${text}</span>`;
	}

	function makeItem(labelText, hasChevron) {
		const item = document.createElement("div");
		item.className = "panel-item";
		item.title = labelText;
		const label = document.createElement("span");
		label.className = "item-label";
		label.textContent = labelText;
		item.appendChild(label);
		if (hasChevron) {
			const chev = document.createElement("span");
			chev.className = "item-chevron";
			chev.textContent = "›";
			item.appendChild(chev);
		}
		return item;
	}

	function normalizeNodeName(name) {
		return String(name || "")
			.toLowerCase()
			.replace(/[^a-z0-9]/g, "");
	}

	function nodeIdForName(displayName) {
		return nodeIdMap.get(normalizeNodeName(displayName)) || null;
	}

	function appendNodeIdAccent(item, nodeName) {
		const nodeId = nodeIdForName(nodeName);
		if (!nodeId) return;
		const accent = document.createElement("span");
		accent.className = "item-accent";
		accent.textContent = nodeId;
		const chev = item.querySelector(".item-chevron");
		if (chev) {
			item.insertBefore(accent, chev);
		} else {
			item.appendChild(accent);
		}
	}

	async function loadNodeIds(ref) {
		const result = await window.GrcanApi.fetchNodeIds(ref);
		nodeIdMap = new Map();
		if (result.error || !result.nodeIds) return;
		result.nodeIds.forEach((entry) => {
			const pretty = entry.name.replace(/_/g, " ");
			nodeIdMap.set(normalizeNodeName(entry.name), entry.id);
			nodeIdMap.set(normalizeNodeName(pretty), entry.id);
		});
	}

	function canonicalBusName(text) {
		const value = String(text || "").toLowerCase();
		if (value.includes("primary")) return "Primary";
		if (value.includes("data")) return "Data";
		if (value.includes("charg")) return "Charger";
		if (value.includes("testing")) return "Testing";
		return null;
	}

	function setHierarchyHeaders() {
		if (!firstHeader || !secondHeader) return;
		if (HIERARCHY_MODE === "NODE_BUS") {
			firstHeader.textContent = "Nodes";
			secondHeader.textContent = "Buses";
		} else {
			firstHeader.textContent = "Bus";
			secondHeader.textContent = "Nodes";
		}
	}

	function renderMessages(messages) {
		msgList.innerHTML = "";
		if (!messages || messages.length === 0) {
			setPlaceholder(msgList, "No messages");
			return;
		}

		messages.forEach((msg) => {
			const item = document.createElement("div");
			item.className = "panel-item msg-item";

			const name = document.createElement("span");
			name.className = "msg-name";
			name.textContent = msg.msgName;
			item.appendChild(name);

			if (msg.canIdOverride || (msg.receivers && msg.receivers.length > 0)) {
				const meta = document.createElement("div");
				meta.className = "msg-meta";
				if (msg.canIdOverride) {
					const badge = document.createElement("span");
					badge.className = "msg-badge";
					badge.textContent = `ID: ${msg.canIdOverride}`;
					meta.appendChild(badge);
				}
				if (msg.receivers && msg.receivers.length > 0) {
					const recv = document.createElement("span");
					recv.className = "msg-receivers";
					recv.textContent = `-> ${msg.receivers.join(", ")}`;
					meta.appendChild(recv);
				}
				item.appendChild(meta);
			}

			if (msg.byteMappings && msg.byteMappings.length > 0) {
				const bytesWrap = document.createElement("div");
				bytesWrap.className = "msg-bytes";
				msg.byteMappings.forEach((mapping) => {
					const row = document.createElement("div");
					row.className = "msg-byte-row";

					const main = document.createElement("span");
					main.className = "msg-byte-main";
					main.textContent = `Byte ${mapping.byteLabel} -> ${mapping.fieldName}`;
					row.appendChild(main);

					if (mapping.bitLabel) {
						const bit = document.createElement("span");
						bit.className = "msg-byte-bits";
						bit.textContent = ` (bits ${mapping.bitLabel})`;
						row.appendChild(bit);
					}

					if (mapping.comment) {
						const c = document.createElement("div");
						c.className = "msg-byte-comment";
						c.textContent = mapping.comment;
						row.appendChild(c);
					}

					bytesWrap.appendChild(row);
				});
				item.appendChild(bytesWrap);
			}
			msgList.appendChild(item);
		});
	}

	function renderBusNodeSecondary(nodes) {
		secondList.innerHTML = "";
		setPlaceholder(msgList, "Select a node");
		if (!nodes || nodes.length === 0) {
			setPlaceholder(secondList, "No nodes on this bus");
			return;
		}
		nodes.forEach((node) => {
			const item = makeItem(node.name, true);
			appendNodeIdAccent(item, node.name);
			item.addEventListener("click", () => {
				secondList.querySelectorAll(".panel-item").forEach((el) => el.classList.remove("active"));
				item.classList.add("active");
				renderMessages(node.messages);
			});
			secondList.appendChild(item);
		});
	}

	async function renderBusNode(ref) {
		setPlaceholder(firstList, "Loading buses...");
		setPlaceholder(secondList, "Select a bus");
		setPlaceholder(msgList, "Select a node");

		const result = await window.GrcanApi.fetchBus(ref);
		if (result.error || !result.buses) {
			setPlaceholder(firstList, result.error === "rate_limited" ? "GitHub API rate limited" : "Unable to load buses");
			return;
		}

		firstList.innerHTML = "";
		result.buses.forEach((bus) => {
			const display = bus.label || bus.name;
			const busName = canonicalBusName(display) || canonicalBusName(bus.name);
			const item = makeItem(display, true);
			item.addEventListener("click", async () => {
				firstList.querySelectorAll(".panel-item").forEach((el) => el.classList.remove("active"));
				item.classList.add("active");
				setPlaceholder(secondList, "Loading nodes...");
				setPlaceholder(msgList, "Select a node");

				if (!busName || busName === "Testing") {
					setPlaceholder(secondList, "No routing data for this bus");
					return;
				}

				const nodesResult = await window.GrcanApi.fetchMessageByBus(ref, busName);
				if (nodesResult.error) {
					setPlaceholder(secondList, nodesResult.error === "rate_limited" ? "GitHub API rate limited" : "Unable to load nodes");
					return;
				}
				renderBusNodeSecondary(nodesResult.nodes);
			});
			firstList.appendChild(item);
		});
	}

	function renderNodeBusSecondary(busEntries) {
		secondList.innerHTML = "";
		setPlaceholder(msgList, "Select a bus");
		if (!busEntries || busEntries.length === 0) {
			setPlaceholder(secondList, "No buses for this node");
			return;
		}
		busEntries.forEach((entry) => {
			const item = makeItem(entry.busName, true);
			item.addEventListener("click", () => {
				secondList.querySelectorAll(".panel-item").forEach((el) => el.classList.remove("active"));
				item.classList.add("active");
				renderMessages(entry.messages);
			});
			secondList.appendChild(item);
		});
	}

	async function renderNodeBus(ref) {
		setPlaceholder(firstList, "Loading nodes...");
		setPlaceholder(secondList, "Select a node");
		setPlaceholder(msgList, "Select a bus");

		const busesResult = await window.GrcanApi.fetchBus(ref);
		if (busesResult.error || !busesResult.buses) {
			setPlaceholder(firstList, busesResult.error === "rate_limited" ? "GitHub API rate limited" : "Unable to load nodes");
			return;
		}

		const routingBuses = busesResult.buses
			.map((bus) => ({
				display: bus.label || bus.name,
				name: canonicalBusName(bus.label || bus.name) || canonicalBusName(bus.name),
			}))
			.filter((b) => b.name && b.name !== "Testing");

		const nodeMap = new Map();
		for (const bus of routingBuses) {
			const nodesResult = await window.GrcanApi.fetchMessageByBus(ref, bus.name);
			if (nodesResult.error || !nodesResult.nodes) continue;
			for (const node of nodesResult.nodes) {
				if (!nodeMap.has(node.name)) nodeMap.set(node.name, []);
				nodeMap.get(node.name).push({ busName: bus.display, messages: node.messages });
			}
		}

		const nodes = [...nodeMap.entries()]
			.map(([name, buses]) => ({ name, buses }))
			.sort((a, b) => a.name.localeCompare(b.name));

		firstList.innerHTML = "";
		if (nodes.length === 0) {
			setPlaceholder(firstList, "No nodes found");
			return;
		}

		nodes.forEach((nodeEntry) => {
			const item = makeItem(nodeEntry.name, true);
			appendNodeIdAccent(item, nodeEntry.name);
			item.addEventListener("click", () => {
				firstList.querySelectorAll(".panel-item").forEach((el) => el.classList.remove("active"));
				item.classList.add("active");
				renderNodeBusSecondary(nodeEntry.buses);
			});
			firstList.appendChild(item);
		});
	}

	async function renderHierarchy(ref) {
		await loadNodeIds(ref);
		if (HIERARCHY_MODE === "NODE_BUS") {
			await renderNodeBus(ref);
		} else {
			await renderBusNode(ref);
		}
	}

	async function onRefInputChange() {
		const ref = refSelect.value;
		if (!ref) {
			setPlaceholder(firstList, "Select a reference");
			setPlaceholder(secondList, HIERARCHY_MODE === "NODE_BUS" ? "Select a node" : "Select a bus");
			setPlaceholder(msgList, HIERARCHY_MODE === "NODE_BUS" ? "Select a bus" : "Select a node");
			return;
		}
		await renderHierarchy(ref);
		if (typeof window.regenerateAndDrawBg === "function") {
			window.regenerateAndDrawBg();
		}
	}

	async function init() {
		setHierarchyHeaders();
		setPlaceholder(firstList, "Loading...");

		const [branches, tags] = await Promise.all([
			window.GrcanApi.fetchBranches(),
			window.GrcanApi.fetchTags(),
		]);

		if (branches === "RATE_LIMITED" || tags === "RATE_LIMITED") {
			if (refInputRow) refInputRow.classList.remove("hidden");
			setPlaceholder(firstList, "API rate limited. References unavailable.");
			setPlaceholder(secondList, "");
			setPlaceholder(msgList, "");
			return;
		}

		refSelect.innerHTML = '<option value="">Select a branch or tag</option>';
		[...branches, ...tags].forEach((ref) => {
			const opt = document.createElement("option");
			opt.value = ref;
			opt.textContent = ref;
			refSelect.appendChild(opt);
		});

		if (branches.includes("main")) {
			refSelect.value = "main";
			await renderHierarchy("main");
		} else {
			setPlaceholder(firstList, "Select a ref");
		}
	}

	refSelect.addEventListener("change", onRefInputChange);

	init();
});
