// Purpose: Main viewer controller. Owns the three-panel hierarchy UI (Nodes /
// Buses / Messages) and all rendering logic. Handles GitHub ref selection,
// initial data load, local re-renders after edits, and change-state badge/dot
// bubbling. Edit mode is treated as a simple boolean flag on GrcanEditor; all
// if(editing) branches live here so the panels naturally show/hide edit controls
// without a separate code path. Also owns the sidebar Edit Mode toggle and
// Download button wiring (moved here from the inline script in index.html).
// Depends on: logic.js (GrcanApi), editor.js (GrcanEditor), diffViewer.js (DiffViewer).

window.addEventListener("DOMContentLoaded", function () {
	const HIERARCHY_MODE = "NODE_BUS"; // "NODE_BUS" or "BUS_NODE"

	const refSelect = document.getElementById("ref-select");
	const firstList = document.getElementById("bus-list");
	const secondList = document.getElementById("node-list");
	const msgList = document.getElementById("msg-list");
	const refInputRow = document.getElementById("ref-input-row");
	const firstHeader = document.querySelector("#bus-panel .panel-header");
	const secondHeader = document.querySelector("#node-panel .panel-header");
	const searchInput = document.getElementById("viewer-search");
	let nodeIdMap = new Map();
	let currentRef = "";
	let _allNodes = []; // persisted node→bus→messages index for search
	let _searchDropdown = null;

	let currentDeviceName = null;
	let currentBusCanonical = null;

	function navSnapshot() {
		return {
			device: currentDeviceName,
			bus: currentBusCanonical,
			mode: HIERARCHY_MODE,
		};
	}

	function isChanged(key) {
		return !!editor && !!editor.isEdited && editor.isEdited(key);
	}

	function keyStatus(key) {
		if (!!editor && !!editor.isNew && editor.isNew(key)) return "new";
		if (isChanged(key)) return "changed";
		return null;
	}

	function combineStatus(a, b) {
		if (a === "new" || b === "new") return "new";
		if (a === "changed" || b === "changed") return "changed";
		return null;
	}

	function addStatusBadge(container, status) {
		// PM request: use a simple yellow dot for all change states.
		// Keep the existing helper call sites unchanged.
		addChangedDot(container);
	}

	function addChangedDot(container) {
		const d = document.createElement("span");
		d.className = "changed-dot";
		d.title = "Changed";
		container.appendChild(d);
	}

	function messageChangeState(msgName, deviceName, busCanonical) {
		const busPort = busCanonical
			? window.GrcanApi.busToPort(busCanonical)
			: null;
		const defStatus = keyStatus("msgDef:" + msgName);
		const routeStatus =
			!!busPort && !!deviceName
				? keyStatus("routeMsg:" + deviceName + "|" + busPort + "|" + msgName)
				: null;
		const directStatus = combineStatus(defStatus, routeStatus);
		return { directStatus, any: !!directStatus };
	}

	function busChangeState(deviceName, busCanonical, messages) {
		const busPort = busCanonical
			? window.GrcanApi.busToPort(busCanonical)
			: null;
		if (!deviceName || !busPort)
			return { directStatus: null, bubbled: false, any: false };
		const directStatus = keyStatus("routeBus:" + deviceName + "|" + busPort);
		const bubbled = (messages || []).some(
			(m) => messageChangeState(m.msgName, deviceName, busCanonical).any,
		);
		return { directStatus, bubbled, any: !!directStatus || bubbled };
	}

	function nodeChangeState(deviceName, buses) {
		if (!deviceName) return { directStatus: null, bubbled: false, any: false };
		const directStatus = keyStatus("routeNode:" + deviceName);
		const bubbled = (buses || []).some(
			(b) =>
				busChangeState(
					deviceName,
					b.canonicalBus || currentBusCanonical,
					b.messages,
				).any,
		);
		return { directStatus, bubbled, any: !!directStatus || bubbled };
	}

	if (!window.GrcanApi || !refSelect || !firstList || !secondList || !msgList) {
		return;
	}

	const editor = window.GrcanEditor || null;

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

	function addIconsBeforeChevron(item, icons) {
		const chev = item.querySelector(".item-chevron");
		if (chev) item.insertBefore(icons, chev);
		else item.appendChild(icons);
	}

	function applySearchFilter() {
		const term = (searchInput ? searchInput.value : "").trim().toLowerCase();
		[firstList, secondList, msgList].forEach((list) => {
			list.querySelectorAll(".panel-item").forEach((item) => {
				const text = item.textContent.toLowerCase();
				item.style.display = term && !text.includes(term) ? "none" : "";
			});
		});
	}

	function buildMessageIndex() {
		const results = [];
		for (const node of _allNodes) {
			for (const bus of node.buses) {
				for (const msg of bus.messages) {
					results.push({
						msgName: msg.msgName,
						deviceName: node.name,
						canonicalBus: bus.canonicalBus,
						busDisplayName: bus.busName,
					});
				}
			}
		}
		return results;
	}

	function getSearchDropdown() {
		if (_searchDropdown) return _searchDropdown;
		_searchDropdown = document.createElement("div");
		_searchDropdown.id = "search-dropdown";
		const row = document.getElementById("viewer-search-row");
		if (row) row.appendChild(_searchDropdown);
		return _searchDropdown;
	}

	function highlightMessage(msgName) {
		const items = msgList.querySelectorAll(".msg-item");
		for (const item of items) {
			const nameEl = item.querySelector(".msg-name");
			if (nameEl && nameEl.textContent === msgName) {
				item.scrollIntoView({ block: "nearest" });
				item.classList.add("msg-highlight");
				setTimeout(() => item.classList.remove("msg-highlight"), 1800);
				break;
			}
		}
	}

	function navigateToMessage(result) {
		if (_searchDropdown) _searchDropdown.style.display = "none";
		if (searchInput) searchInput.value = "";
		applySearchFilter();
		const nodeEl = firstList.querySelector(
			'[data-node-name="' + CSS.escape(result.deviceName) + '"]',
		);
		if (!nodeEl) return;
		nodeEl.scrollIntoView({ block: "nearest" });
		nodeEl.click();
		requestAnimationFrame(() => {
			const busEl = secondList.querySelector(
				'[data-bus-canonical="' + CSS.escape(result.canonicalBus) + '"]',
			);
			if (!busEl) return;
			busEl.scrollIntoView({ block: "nearest" });
			busEl.click();
			requestAnimationFrame(() => highlightMessage(result.msgName));
		});
	}

	function applySearch() {
		applySearchFilter();
		const dropdown = getSearchDropdown();
		const term = searchInput ? searchInput.value.trim().toLowerCase() : "";
		if (!term || _allNodes.length === 0) {
			dropdown.style.display = "none";
			return;
		}
		const index = buildMessageIndex();
		const seen = new Set();
		const matches = index.filter((r) => {
			if (!r.msgName.toLowerCase().includes(term)) return false;
			const key = r.msgName + "|" + r.deviceName + "|" + r.canonicalBus;
			if (seen.has(key)) return false;
			seen.add(key);
			return true;
		});
		if (matches.length === 0) {
			dropdown.style.display = "none";
			return;
		}
		dropdown.innerHTML = "";
		matches.slice(0, 12).forEach((result) => {
			const row = document.createElement("div");
			row.className = "search-result-row";
			const nameEl = document.createElement("span");
			nameEl.className = "sr-name";
			nameEl.textContent = result.msgName;
			const pathEl = document.createElement("span");
			pathEl.className = "sr-path";
			pathEl.textContent =
				result.deviceName + " \u203a " + result.busDisplayName;
			row.appendChild(nameEl);
			row.appendChild(pathEl);
			row.addEventListener("mousedown", (e) => {
				e.preventDefault();
				navigateToMessage(result);
			});
			dropdown.appendChild(row);
		});
		dropdown.style.display = "block";
	}

	if (searchInput) {
		searchInput.addEventListener("input", applySearch);
		searchInput.addEventListener("keydown", (e) => {
			if (e.key === "Escape") {
				searchInput.value = "";
				applySearch();
				searchInput.blur();
			} else if (e.key === "Enter") {
				const dd = _searchDropdown;
				if (dd && dd.firstElementChild) {
					dd.firstElementChild.dispatchEvent(new MouseEvent("mousedown"));
				}
			}
		});
		document.addEventListener("click", (e) => {
			if (
				_searchDropdown &&
				!_searchDropdown.contains(e.target) &&
				e.target !== searchInput
			) {
				_searchDropdown.style.display = "none";
			}
		});
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

	function loadNodeIdsFromText(text) {
		const result = window.GrcanApi.parseNodeIdsFromText(text);
		nodeIdMap = new Map();
		if (!result.nodeIds) return;
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

	function isEditing() {
		return editor && editor.isEditMode();
	}

	// ==================== Edit Mode UI ====================
	// Centralises sidebar button state so both the toggle click and the ref-change
	// reset go through a single place.

	function setEditModeUI(active) {
		const toggleBtn = document.getElementById("edit-mode-toggle");
		if (toggleBtn) {
			toggleBtn.textContent = active ? "Exit Edit Mode" : "Edit Mode";
			toggleBtn.classList.toggle("active", active);
		}
	}

	function wireEditModeButtons() {
		const toggleBtn = document.getElementById("edit-mode-toggle");
		const dlBtn = document.getElementById("download-btn");
		if (!toggleBtn || !dlBtn || !editor) return;

		toggleBtn.addEventListener("click", function () {
			if (!editor.getRawText()) return;
			const active = editor.toggleEditMode();
			setEditModeUI(active);
			editor.triggerReRender();
		});

		dlBtn.addEventListener("click", function () {
			const oldText = editor.getOriginalRawText
				? editor.getOriginalRawText()
				: "";
			const newText = editor.getRawText ? editor.getRawText() : "";
			if (!window.DiffViewer || oldText === newText) {
				editor.downloadCando();
				return;
			}
			window.DiffViewer.show({
				oldText,
				newText,
				onConfirm: function () {
					editor.downloadCando();
				},
			});
		});
	}

	// ==================== Render Messages (Panel 3) ====================

	function renderMessages(messages) {
		msgList.innerHTML = "";
		const editing = isEditing();
		const busPort = currentBusCanonical
			? window.GrcanApi.busToPort(currentBusCanonical)
			: null;
		if (!messages || messages.length === 0) {
			setPlaceholder(msgList, "No messages");
			if (editing && busPort && currentDeviceName) {
				msgList.appendChild(
					editor.createAddBtn("Add Route", () => {
						editor.setNavSnapshot(navSnapshot());
						editor.showRoutingAddForm(currentDeviceName, busPort);
					}),
				);
				msgList.appendChild(
					editor.createAddBtn("Add Message Definition", () => {
						editor.setNavSnapshot(navSnapshot());
						editor.showMessageEditForm(null, true);
					}),
				);
				msgList.appendChild(
					editor.createAddBtn("Add Custom CAN ID", () => {
						editor.setNavSnapshot(navSnapshot());
						editor.showCustomCanIdEditForm(null, true);
					}),
				);
			}
			return;
		}

		messages.forEach((msg) => {
			const item = document.createElement("div");
			item.className = "panel-item msg-item";
			const change = messageChangeState(
				msg.msgName,
				currentDeviceName,
				currentBusCanonical,
			);
			if (change.any) item.classList.add("edited-item");

			const nameRow = document.createElement("div");
			nameRow.className = "msg-name-row";

			const name = document.createElement("span");
			name.className = "msg-name";
			name.textContent = msg.msgName;
			nameRow.appendChild(name);

			// Determine preview content: standard byte mappings or Custom CAN ID signals
			const hasByteMappings = msg.byteMappings && msg.byteMappings.length > 0;
			let customCanIdDef = null;
			if (
				!hasByteMappings &&
				editor &&
				editor.isCustomCanIdMessage(msg.msgName)
			) {
				customCanIdDef =
					window.GrcanDocument &&
					window.GrcanDocument.getCustomCanIdDef(msg.msgName);
			}
			const hasPreview =
				hasByteMappings ||
				(customCanIdDef &&
					customCanIdDef.signals &&
					customCanIdDef.signals.length > 0);

			if (hasPreview) {
				const expandBtn = document.createElement("span");
				expandBtn.className = "msg-expand-btn collapsed";
				expandBtn.textContent = "›";
				nameRow.appendChild(expandBtn);
			}

			if (editing) {
				const icons = document.createElement("span");
				icons.className = "editor-icons";
				icons.appendChild(
					editor.createEditBtn(() => {
						editor.setNavSnapshot(navSnapshot());
						if (editor.isCustomCanIdMessage(msg.msgName)) {
							editor.showCustomCanIdEditForm(msg.msgName, false);
						} else {
							editor.showMessageEditForm(msg.msgName, false);
						}
					}),
				);
				if (busPort && currentDeviceName) {
					icons.appendChild(
						editor.createDeleteBtn(() => {
							editor.setNavSnapshot(navSnapshot());
							editor.confirmAndDelete(msg.msgName, () => {
								window.GrcanDocument.deleteRouteEntry(
									currentDeviceName,
									busPort,
									msg.msgName,
								);
								editor.markEdited(
									"routeMsg:" +
										(currentDeviceName || "") +
										"|" +
										busPort +
										"|" +
										msg.msgName,
								);
							});
						}),
					);
				}
				nameRow.appendChild(icons);
			}

			item.appendChild(nameRow);
			if (change.any) {
				const badgeRow = document.createElement("div");
				badgeRow.className = "msg-edited-row";
				addStatusBadge(badgeRow, change.directStatus || "changed");
				item.appendChild(badgeRow);
			}

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

			if (hasPreview) {
				const details = document.createElement("div");
				details.className = "msg-details collapsed";

				const bytesWrap = document.createElement("div");
				bytesWrap.className = "msg-bytes";

				if (hasByteMappings) {
					msg.byteMappings.forEach((mapping) => {
						const row = document.createElement("div");
						row.className = "msg-byte-row";

						const main = document.createElement("span");
						main.className = "msg-byte-main";
						main.textContent = `Byte ${mapping.byteLabel} -> ${mapping.fieldName}`;
						row.appendChild(main);

						if (mapping.dataType) {
							const typeChip = document.createElement("span");
							typeChip.className = "msg-type-chip";
							typeChip.textContent = mapping.dataType;
							row.appendChild(typeChip);
						}

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
				} else if (customCanIdDef) {
					customCanIdDef.signals.forEach((signal) => {
						const row = document.createElement("div");
						row.className = "msg-byte-row";

						const main = document.createElement("span");
						main.className = "msg-byte-main";
						main.textContent = `Signal: ${signal.name || "(unnamed)"}`;
						row.appendChild(main);

						if (signal.bitStart !== undefined && signal.bitStart !== null) {
							const bit = document.createElement("span");
							bit.className = "msg-byte-bits";
							bit.textContent = ` (bits ${signal.bitStart})`;
							row.appendChild(bit);
						}

						if (signal.comment) {
							const c = document.createElement("div");
							c.className = "msg-byte-comment";
							c.textContent = signal.comment;
							row.appendChild(c);
						}

						bytesWrap.appendChild(row);
					});
				}

				details.appendChild(bytesWrap);
				item.appendChild(details);

				// Wire expand/collapse toggle
				const expandBtn = nameRow.querySelector(".msg-expand-btn");
				if (expandBtn) {
					expandBtn.addEventListener("click", (e) => {
						e.stopPropagation();
						const collapsed = details.classList.toggle("collapsed");
						expandBtn.classList.toggle("collapsed", collapsed);
					});
				}
			}
			msgList.appendChild(item);
		});

		if (editing && busPort && currentDeviceName) {
			msgList.appendChild(
				editor.createAddBtn("Add Route", () => {
					editor.setNavSnapshot(navSnapshot());
					editor.showRoutingAddForm(currentDeviceName, busPort);
				}),
			);
			msgList.appendChild(
				editor.createAddBtn("Add Message Definition", () => {
					editor.setNavSnapshot(navSnapshot());
					editor.showMessageEditForm(null, true);
				}),
			);
			msgList.appendChild(
				editor.createAddBtn("Add Custom CAN ID", () => {
					editor.setNavSnapshot(navSnapshot());
					editor.showCustomCanIdEditForm(null, true);
				}),
			);
		}
		applySearchFilter();
	}

	// ==================== BUS_NODE mode ====================

	function renderBusNodeSecondary(nodes) {
		secondList.innerHTML = "";
		setPlaceholder(msgList, "Select a node");
		if (!nodes || nodes.length === 0) {
			setPlaceholder(secondList, "No nodes on this bus");
			return;
		}
		const editing = isEditing();
		nodes.forEach((node) => {
			const item = makeItem(node.name, true);
			item.dataset.nodeName = node.name;
			const busState = busChangeState(
				node.name,
				currentBusCanonical,
				node.messages,
			);
			if (busState.any) {
				item.classList.add("edited-item");
				const tokenHost = document.createElement("span");
				if (busState.directStatus)
					addStatusBadge(tokenHost, busState.directStatus);
				else addChangedDot(tokenHost);
				addIconsBeforeChevron(item, tokenHost);
			}
			appendNodeIdAccent(item, node.name);

			if (editing) {
				const icons = document.createElement("span");
				icons.className = "editor-icons";
				icons.appendChild(
					editor.createEditBtn(() => {
						editor.setNavSnapshot(navSnapshot());
						editor.showRoutingNodeEditForm(node.name);
					}),
				);
				icons.appendChild(
					editor.createDeleteBtn(() => {
						const busPort = currentBusCanonical
							? window.GrcanApi.busToPort(currentBusCanonical)
							: null;
						if (busPort) {
							editor.setNavSnapshot(navSnapshot());
							editor.confirmAndDelete(
								node.name + " on " + currentBusCanonical,
								() => {
									window.GrcanDocument.deleteBusBlock(node.name, busPort);
									editor.markEdited("routeBus:" + node.name + "|" + busPort);
								},
							);
						}
					}),
				);
				addIconsBeforeChevron(item, icons);
			}

			item.addEventListener("click", () => {
				currentDeviceName = node.name;
				secondList
					.querySelectorAll(".panel-item")
					.forEach((el) => el.classList.remove("active"));
				item.classList.add("active");
				renderMessages(node.messages);
			});
			secondList.appendChild(item);
		});
	}

	async function renderBusNode(ref, localText) {
		const isLocal = !!localText;
		setPlaceholder(firstList, "Loading buses...");
		setPlaceholder(secondList, "Select a bus");
		setPlaceholder(msgList, "Select a node");

		let result;
		if (isLocal) {
			result = window.GrcanApi.parseBusIdsFromText(localText);
		} else {
			result = await window.GrcanApi.fetchBus(ref);
		}

		if (result.error || !result.buses) {
			setPlaceholder(
				firstList,
				result.error === "rate_limited"
					? "GitHub API rate limited"
					: "Unable to load buses",
			);
			return;
		}

		firstList.innerHTML = "";
		result.buses.forEach((bus) => {
			const display = bus.label || bus.name;
			const busName = canonicalBusName(display) || canonicalBusName(bus.name);
			const item = makeItem(display, true);
			item.dataset.busCanonical = busName || "";
			if (isLocal && busName && busName !== "Testing") {
				const nr = window.GrcanApi.parseMessageByBusFromText(
					localText,
					busName,
				);
				const busChanged = (nr.nodes || []).some(
					(n) => busChangeState(n.name, busName, n.messages).any,
				);
				if (busChanged) {
					item.classList.add("edited-item");
					const tokenHost = document.createElement("span");
					addChangedDot(tokenHost);
					addIconsBeforeChevron(item, tokenHost);
				}
			}
			item.addEventListener("click", async () => {
				currentBusCanonical = busName;
				currentDeviceName = null;
				firstList
					.querySelectorAll(".panel-item")
					.forEach((el) => el.classList.remove("active"));
				item.classList.add("active");
				setPlaceholder(secondList, "Loading nodes...");
				setPlaceholder(msgList, "Select a node");

				if (!busName || busName === "Testing") {
					setPlaceholder(secondList, "No routing data for this bus");
					return;
				}

				let nodesResult;
				if (isLocal) {
					nodesResult = window.GrcanApi.parseMessageByBusFromText(
						localText,
						busName,
					);
				} else {
					nodesResult = await window.GrcanApi.fetchMessageByBus(ref, busName);
				}

				if (nodesResult.error) {
					setPlaceholder(
						secondList,
						nodesResult.error === "rate_limited"
							? "GitHub API rate limited"
							: "Unable to load nodes",
					);
					return;
				}
				const nodesOnBus = (nodesResult.nodes || []).filter(
					(node) => node.hasBus || (node.messages || []).length > 0,
				);
				renderBusNodeSecondary(nodesOnBus);
			});
			firstList.appendChild(item);
		});
	}

	// ==================== NODE_BUS mode ====================

	function renderNodeBusSecondary(busEntries, deviceName) {
		secondList.innerHTML = "";
		setPlaceholder(msgList, "Select a bus");
		const editing = isEditing();
		if (!busEntries || busEntries.length === 0) {
			setPlaceholder(secondList, "No buses for this node");
			if (editing) {
				const hint = document.createElement("div");
				hint.className = "placeholder";
				hint.textContent = "Use Add Bus to create this node's first bus.";
				secondList.appendChild(hint);
			}
			if (editing && deviceName) {
				secondList.appendChild(
					editor.createAddBtn("Add Bus", () => {
						editor.setNavSnapshot(navSnapshot());
						editor.showRoutingBusAddForm(deviceName);
					}),
				);
			}
			return;
		}
		busEntries.forEach((entry) => {
			const item = makeItem(entry.busName, true);
			item.dataset.busCanonical = entry.canonicalBus || "";
			const busState = busChangeState(
				deviceName,
				entry.canonicalBus,
				entry.messages,
			);
			if (busState.any) {
				item.classList.add("edited-item");
				const tokenHost = document.createElement("span");
				if (busState.directStatus)
					addStatusBadge(tokenHost, busState.directStatus);
				else addChangedDot(tokenHost);
				addIconsBeforeChevron(item, tokenHost);
			}

			if (editing && entry.canonicalBus) {
				const busPort = window.GrcanApi.busToPort(entry.canonicalBus);
				if (busPort) {
					const icons = document.createElement("span");
					icons.className = "editor-icons";
					icons.appendChild(
						editor.createEditBtn(() => {
							editor.setNavSnapshot(navSnapshot());
							editor.showRoutingBusEditForm(deviceName, busPort);
						}),
					);
					icons.appendChild(
						editor.createDeleteBtn(() => {
							editor.setNavSnapshot(navSnapshot());
							editor.confirmAndDelete(
								deviceName + " > " + entry.busName,
								() => {
									window.GrcanDocument.deleteBusBlock(deviceName, busPort);
									editor.markEdited("routeBus:" + deviceName + "|" + busPort);
								},
							);
						}),
					);
					addIconsBeforeChevron(item, icons);
				}
			}

			item.addEventListener("click", () => {
				currentBusCanonical = entry.canonicalBus || null;
				secondList
					.querySelectorAll(".panel-item")
					.forEach((el) => el.classList.remove("active"));
				item.classList.add("active");
				renderMessages(entry.messages);
			});
			secondList.appendChild(item);
		});

		if (editing && deviceName) {
			secondList.appendChild(
				editor.createAddBtn("Add Bus", () => {
					editor.setNavSnapshot(navSnapshot());
					editor.showRoutingBusAddForm(deviceName);
				}),
			);
		}
		applySearchFilter();
	}

	async function renderNodeBus(ref, localText) {
		_allNodes = [];
		const isLocal = !!localText;
		setPlaceholder(firstList, "Loading nodes...");
		setPlaceholder(secondList, "Select a node");
		setPlaceholder(msgList, "Select a bus");

		let busesResult;
		if (isLocal) {
			busesResult = window.GrcanApi.parseBusIdsFromText(localText);
		} else {
			busesResult = await window.GrcanApi.fetchBus(ref);
		}

		if (busesResult.error || !busesResult.buses) {
			setPlaceholder(
				firstList,
				busesResult.error === "rate_limited"
					? "GitHub API rate limited"
					: "Unable to load nodes",
			);
			return;
		}

		const routingBuses = busesResult.buses
			.map((bus) => ({
				display: bus.label || bus.name,
				name:
					canonicalBusName(bus.label || bus.name) || canonicalBusName(bus.name),
			}))
			.filter((b) => b.name && b.name !== "Testing");

		const nodeMap = new Map();
		let nodeCatalogResult;
		if (isLocal) {
			nodeCatalogResult = window.GrcanApi.parseNodeCatalogFromText(localText);
		} else {
			nodeCatalogResult = await window.GrcanApi.fetchNodeCatalog(ref);
		}
		const catalogNodes = Array.isArray(nodeCatalogResult)
			? nodeCatalogResult
			: nodeCatalogResult && Array.isArray(nodeCatalogResult.nodes)
				? nodeCatalogResult.nodes
				: [];
		if (catalogNodes.length > 0) {
			catalogNodes.forEach((nodeName) => {
				if (!nodeMap.has(nodeName)) nodeMap.set(nodeName, []);
			});
		}
		for (const bus of routingBuses) {
			let nodesResult;
			if (isLocal) {
				nodesResult = window.GrcanApi.parseMessageByBusFromText(
					localText,
					bus.name,
				);
			} else {
				nodesResult = await window.GrcanApi.fetchMessageByBus(ref, bus.name);
			}
			if (nodesResult.error || !nodesResult.nodes) continue;
			for (const node of nodesResult.nodes) {
				if (!node.hasBus && (!node.messages || node.messages.length === 0))
					continue;
				if (!nodeMap.has(node.name)) nodeMap.set(node.name, []);
				nodeMap.get(node.name).push({
					busName: bus.display,
					canonicalBus: bus.name,
					messages: node.messages,
				});
			}
		}

		const nodes = [...nodeMap.entries()]
			.map(([name, buses]) => ({ name, buses }))
			.sort((a, b) => {
				const idA = nodeIdForName(a.name);
				const idB = nodeIdForName(b.name);
				const numA = idA ? parseInt(idA, 16) : Infinity;
				const numB = idB ? parseInt(idB, 16) : Infinity;
				if (numA !== numB) return numA - numB;
				return a.name.localeCompare(b.name);
			});
		_allNodes = nodes;

		firstList.innerHTML = "";
		if (nodes.length === 0) {
			setPlaceholder(firstList, "No nodes found");
			return;
		}

		const editing = isEditing();

		nodes.forEach((nodeEntry) => {
			const item = makeItem(nodeEntry.name, true);
			item.dataset.nodeName = nodeEntry.name;
			const nodeState = nodeChangeState(nodeEntry.name, nodeEntry.buses);
			if (nodeState.any) {
				item.classList.add("edited-item");
				const tokenHost = document.createElement("span");
				if (nodeState.directStatus)
					addStatusBadge(tokenHost, nodeState.directStatus);
				else addChangedDot(tokenHost);
				addIconsBeforeChevron(item, tokenHost);
			}
			appendNodeIdAccent(item, nodeEntry.name);

			if (editing) {
				const icons = document.createElement("span");
				icons.className = "editor-icons";
				icons.appendChild(
					editor.createEditBtn(() => {
						editor.setNavSnapshot(navSnapshot());
						editor.showRoutingNodeEditForm(nodeEntry.name);
					}),
				);
				icons.appendChild(
					editor.createDeleteBtn(() => {
						editor.setNavSnapshot(navSnapshot());
						editor.confirmAndDelete(nodeEntry.name + " (all routes)", () => {
							const result = window.GrcanDocument.deleteDevice(nodeEntry.name);
							if (!result.ok) {
								console.error("deleteDevice failed:", result.error);
								return;
							}
							editor.markEdited("routeNode:" + nodeEntry.name);
						});
					}),
				);
				addIconsBeforeChevron(item, icons);
			}

			item.addEventListener("click", () => {
				currentDeviceName = nodeEntry.name;
				firstList
					.querySelectorAll(".panel-item")
					.forEach((el) => el.classList.remove("active"));
				item.classList.add("active");
				renderNodeBusSecondary(nodeEntry.buses, nodeEntry.name);
			});
			firstList.appendChild(item);
		});

		if (editing) {
			firstList.appendChild(
				editor.createAddBtn("Add Node", () => {
					editor.setNavSnapshot(navSnapshot());
					editor.showRoutingAddForm(null, null);
				}),
			);
			firstList.appendChild(
				editor.createAddBtn("Super Add", () => {
					editor.setNavSnapshot(navSnapshot());
					editor.showSuperAddForm();
				}),
			);
		}
		applySearchFilter();
	}

	// ==================== Hierarchy entry points ====================

	async function renderHierarchy(ref) {
		const candoResult = await window.GrcanApi.fetchCando(ref);
		const localText = candoResult.notFound ? null : candoResult.content;

		if (localText) {
			loadNodeIdsFromText(localText);
		} else {
			await loadNodeIds(ref);
		}

		if (!candoResult.notFound && editor) {
			editor.setRawText(candoResult.content);
			if (window.GrcanDocument) {
				const violations = window.GrcanDocument.validate();
				if (violations.length > 0) {
					console.warn(
						"[GrcanDocument] CANdo validation issues on load:",
						violations,
					);
				}
			}
		}

		if (HIERARCHY_MODE === "NODE_BUS") {
			await renderNodeBus(ref, localText);
		} else {
			await renderBusNode(ref, localText);
		}
	}

	function restoreSelection(snapshot) {
		if (!snapshot) return;
		if (snapshot.mode === "NODE_BUS") {
			if (snapshot.device) {
				const n = firstList.querySelector(
					'.panel-item[data-node-name="' + CSS.escape(snapshot.device) + '"]',
				);
				if (n) n.click();
			}
			if (snapshot.bus) {
				const b = secondList.querySelector(
					'.panel-item[data-bus-canonical="' + CSS.escape(snapshot.bus) + '"]',
				);
				if (b) b.click();
			}
		} else {
			if (snapshot.bus) {
				const b = firstList.querySelector(
					'.panel-item[data-bus-canonical="' + CSS.escape(snapshot.bus) + '"]',
				);
				if (b) b.click();
			}
			if (snapshot.device) {
				const n = secondList.querySelector(
					'.panel-item[data-node-name="' + CSS.escape(snapshot.device) + '"]',
				);
				if (n) n.click();
			}
		}
	}

	async function reRenderLocal(snapshot) {
		if (!editor) return;
		const text = editor.getRawText();
		if (!text) return;
		loadNodeIdsFromText(text);
		if (HIERARCHY_MODE === "NODE_BUS") {
			await renderNodeBus(null, text);
		} else {
			await renderBusNode(null, text);
		}
		restoreSelection(snapshot || navSnapshot());
	}

	if (editor) {
		editor.setReRenderCallback(reRenderLocal);
	}

	// Warn users before accidental tab close/reload when in-memory edits exist.
	window.addEventListener("beforeunload", function (e) {
		if (!editor || !editor.hasUnsavedEdits || !editor.hasUnsavedEdits()) return;
		e.preventDefault();
		e.returnValue = "";
		return "";
	});

	// ==================== Event handlers ====================

	async function onRefInputChange() {
		const ref = refSelect.value;
		if (
			editor &&
			currentRef &&
			ref &&
			ref !== currentRef &&
			editor.hasUnsavedEdits()
		) {
			const wantsDownload = window.confirm(
				"You have unsaved changes. Download your changes before switching reference?",
			);
			if (wantsDownload) {
				const oldText = editor.getOriginalRawText
					? editor.getOriginalRawText()
					: "";
				const newText = editor.getRawText ? editor.getRawText() : "";
				if (window.DiffViewer && oldText !== newText) {
					window.DiffViewer.show({
						oldText,
						newText,
						onConfirm: function () {
							editor.downloadCando();
						},
					});
				} else {
					editor.downloadCando();
				}
			}
			refSelect.value = currentRef;
			return;
		}
		if (!ref) {
			setPlaceholder(firstList, "Select a reference");
			setPlaceholder(
				secondList,
				HIERARCHY_MODE === "NODE_BUS" ? "Select a node" : "Select a bus",
			);
			setPlaceholder(
				msgList,
				HIERARCHY_MODE === "NODE_BUS" ? "Select a bus" : "Select a node",
			);
			return;
		}
		if (editor && editor.isEditMode()) {
			editor.toggleEditMode();
			setEditModeUI(false);
		}
		await renderHierarchy(ref);
		currentRef = ref;
		if (typeof window.regenerateAndDrawBg === "function") {
			window.regenerateAndDrawBg();
		}
	}

	async function init() {
		setHierarchyHeaders();
		wireEditModeButtons();
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
			currentRef = "main";
		} else {
			setPlaceholder(firstList, "Select a ref");
		}
	}

	refSelect.addEventListener("change", onRefInputChange);

	// ==================== Local-file toggle ====================
	const localToggle = document.getElementById("local-toggle");
	const localFileInput = document.getElementById("local-file-input");

	if (localToggle && localFileInput) {
		localToggle.addEventListener("change", function () {
			if (localToggle.checked) {
				localFileInput.style.display = "block";
				localFileInput.click();
			} else {
				localFileInput.style.display = "none";
				localFileInput.value = "";
				window.GrcanApi.setLocalCandoText(null);
				refSelect.disabled = false;
				if (currentRef) renderHierarchy(currentRef);
			}
		});

		localFileInput.addEventListener("change", function () {
			const file = localFileInput.files[0];
			if (!file) {
				localToggle.checked = false;
				localFileInput.style.display = "none";
				window.GrcanApi.setLocalCandoText(null);
				refSelect.disabled = false;
				return;
			}
			const reader = new FileReader();
			reader.onload = function (e) {
				window.GrcanApi.setLocalCandoText(e.target.result);
				refSelect.disabled = true;
				renderHierarchy(currentRef || "local");
			};
			reader.readAsText(file);
		});

		localFileInput.addEventListener("cancel", function () {
			localToggle.checked = false;
			localFileInput.style.display = "none";
		});
	}

	init();
});
