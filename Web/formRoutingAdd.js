// Purpose: "Add Route" modal form.
// Lets users add a new routing entry (device / bus / receiver / message) to the
// CANdo routing section, creating the device or bus block if they don't exist yet.
// Validates that the named message exists in Message ID or Custom CAN ID, rejects
// duplicate exact-match entries, and marks new/changed tracking keys accordingly.
// Depends on: formUtils.js (FormUtils), editor.js (GrcanEditor).
// Registers: window.GrcanEditor.showRoutingAddForm

(function () {
	"use strict";

	function showRoutingAddForm(deviceName, busPort) {
		const editor = window.GrcanEditor;
		const fu = window.FormUtils;
		const { overlay, body, footer } = fu.createModal("Add Route");

		// Catalog candidates are loaded from parser outputs (headers) via GrcanApi.
		// Current routing usage is still derived from in-memory text for unused-first ranking.
		let allMessageNames = [];
		const hasRoute = new Map();
		let receiverList = [];

		function routingSectionBounds(lines) {
			const routingStart = lines.findIndex((l) => l.startsWith("routing:"));
			if (routingStart === -1) return null;
			const routingEnd = lines.findIndex(
				(l, i) => i > routingStart + 1 && /^\S/.test(l),
			);
			return {
				start: routingStart,
				end: routingEnd === -1 ? lines.length : routingEnd,
			};
		}

		// busPortFilter: if provided (e.g. "CAN3"), only marks messages as used
		// when they appear under that specific bus port block. This makes the
		// "unused first" ranking bus-local rather than global.
		function buildRouteUsageMap(rawText, busPortFilter) {
			hasRoute.clear();
			const lines = String(rawText || "").split("\n");
			const bounds = routingSectionBounds(lines);
			if (!bounds) return;
			let activeBus = !busPortFilter;
			for (let i = bounds.start + 1; i < bounds.end; i++) {
				const line = lines[i];
				if (!line.trim()) continue;
				const indent = line.search(/\S/);
				const content = line.trim();
				if (indent === 6) {
					// "      CANx:" — toggle tracking based on bus match
					activeBus = !busPortFilter || content === busPortFilter + ":";
				} else if (indent === 4) {
					activeBus = !busPortFilter;
				}
				if (activeBus) {
					const m = content.match(/^- msg:\s*(.+)$/);
					if (m) hasRoute.set(m[1].trim(), true);
				}
			}
		}

		// busPortFilter: if provided, only collects receiver names (indent 8)
		// that live under the matching bus port block. Without a filter,
		// collects every receiver name across all buses.
		function buildRoutingReceiverSet(rawText, busPortFilter) {
			const names = new Set();
			const lines = String(rawText || "").split("\n");
			const bounds = routingSectionBounds(lines);
			if (!bounds) return names;
			let activeBus = !busPortFilter;
			for (let i = bounds.start + 1; i < bounds.end; i++) {
				const line = lines[i];
				if (!line.trim()) continue;
				const indent = line.search(/\S/);
				const content = line.trim();
				if (indent === 6) {
					activeBus = !busPortFilter || content === busPortFilter + ":";
				} else if (indent === 4) {
					activeBus = !busPortFilter;
				} else if (activeBus && indent === 8 && content.endsWith(":")) {
					names.add(content.slice(0, -1).trim());
				}
			}
			return names;
		}

		// Loads catalog candidates from parser-output headers (source of truth for
		// name lists). Usage ranking is bus-local: currentBusPort narrows which
		// routing entries count as "already used" so unused-first sorting is
		// meaningful in context.
		async function loadCatalogSuggestions() {
			const rawText = editor.getRawText ? editor.getRawText() : "";
			// Read the live bus value so filtering updates when the user changes it.
			const currentBusPort = busF.input.value || null;
			buildRouteUsageMap(rawText, currentBusPort);
			const routingNames = buildRoutingReceiverSet(rawText, currentBusPort);
			const fallbackMessages =
				window.GrcanApi && window.GrcanApi.parseMessageCatalogFromText
					? window.GrcanApi.parseMessageCatalogFromText(rawText)
					: [];
			const fallbackNodes =
				window.GrcanApi && window.GrcanApi.parseNodeCatalogFromText
					? window.GrcanApi.parseNodeCatalogFromText(rawText)
					: [];
			const refEl = document.getElementById("ref-select");
			const ref = refEl ? refEl.value : "";
			if (!window.GrcanApi || !ref) {
				allMessageNames = [...new Set(fallbackMessages)];
				receiverList = [...new Set([...fallbackNodes, ...routingNames])].sort(
					(a, b) => a.localeCompare(b),
				);
				return;
			}
			const [messageCatalog, nodeCatalog] = await Promise.all([
				window.GrcanApi.fetchMessageCatalog(ref),
				window.GrcanApi.fetchNodeCatalog(ref),
			]);
			const headerNames =
				!messageCatalog.error && messageCatalog.messages
					? messageCatalog.messages
					: [];
			// Merge header-derived and current-text-derived names so that new
			// message definitions created in this edit session are immediately
			// available in autocomplete. Header names remain the long-term
			// source of truth; text names add local, in-session additions.
			const messageCandidates = [...new Set([...headerNames, ...fallbackMessages])];
			// For receivers: use catalog node names ONLY when no bus is selected,
			// so we don't suggest nodes from other buses. When a bus is locked,
			// routingNames already contains the bus-local receivers.
			const nodeCandidates =
				!nodeCatalog.error && nodeCatalog.nodes ? nodeCatalog.nodes : fallbackNodes;
			const baseReceivers = currentBusPort
				? [...routingNames]
				: [...new Set([...nodeCandidates, ...routingNames])];
			allMessageNames = [...new Set(messageCandidates)];
			receiverList = [...new Set(baseReceivers)].sort((a, b) => a.localeCompare(b));
		}

		const devF = fu.makeFormRow(
			"Device",
			fu.makeInput("text", deviceName || "", "Device Name"),
			true,
		);
		if (deviceName) devF.input.disabled = true;
		body.appendChild(devF.row);

		const busF = fu.makeFormRow(
			"Bus",
			fu.makeSelect(["CAN1", "CAN2", "CAN3"], busPort || "CAN1"),
			true,
		);
		if (busPort) {
			busF.input.disabled = true;
		} else {
			// When the user changes the bus, silently refresh the underlying
			// candidate data only. Never open the dropdowns here — they must
			// only appear when the user explicitly focuses/types in the input.
			busF.input.addEventListener("change", () => {
				loadCatalogSuggestions();
			});
		}
		body.appendChild(busF.row);

		const recF = fu.makeFormRow(
			"Receiver",
			fu.makeInput("text", "", "Receiver Name"),
			true,
		);
		body.appendChild(recF.row);

		// Simple receiver name autocomplete based on known node names from
		// GR ID and routing. This is a convenience only; free-typing is
		// still allowed for new nodes.
		let recSuggestIndex = -1;
		const recSuggestBox = document.createElement("div");
		recSuggestBox.className = "editor-suggest hidden";
		recF.row.appendChild(recSuggestBox);

		function renderReceiverSuggestions(term) {
			// Hard guard: never open the dropdown unless this input is focused.
			if (document.activeElement !== recF.input) {
				recSuggestBox.classList.add("hidden");
				return;
			}
			if (!receiverList.length) {
				recSuggestBox.classList.add("hidden");
				recSuggestBox.innerHTML = "";
				return;
			}
			const q = String(term || "").toLowerCase();
			const matches = receiverList.filter(
				(name) => !q || name.toLowerCase().includes(q),
			);
			if (!matches.length) {
				recSuggestBox.classList.add("hidden");
				recSuggestBox.innerHTML = "";
				return;
			}
			recSuggestBox.innerHTML = "";
			recSuggestIndex = 0;
			matches.slice(0, 20).forEach((name, idx) => {
				const item = document.createElement("div");
				item.className =
					"editor-suggest-item" +
					(idx === 0 ? " editor-suggest-item-active" : "");
				item.textContent = name;
				item.addEventListener("mousedown", (e) => {
					e.preventDefault();
					recF.input.value = name;
					recSuggestBox.classList.add("hidden");
				});
				recSuggestBox.appendChild(item);
			});
			recSuggestBox.classList.remove("hidden");
		}

		recF.input.addEventListener("input", () => {
			renderReceiverSuggestions(recF.input.value);
		});
		recF.input.addEventListener("focus", () => {
			if (!receiverList.length) loadCatalogSuggestions();
			renderReceiverSuggestions(recF.input.value);
		});
		recF.input.addEventListener("blur", () => {
			setTimeout(() => {
				recSuggestBox.classList.add("hidden");
			}, 150);
		});
		recF.input.addEventListener("keydown", (e) => {
			const items = recSuggestBox.querySelectorAll(".editor-suggest-item");
			if (!items.length || recSuggestBox.classList.contains("hidden")) return;
			if (e.key === "ArrowDown") {
				e.preventDefault();
				recSuggestIndex = (recSuggestIndex + 1) % items.length;
			} else if (e.key === "ArrowUp") {
				e.preventDefault();
				recSuggestIndex = (recSuggestIndex - 1 + items.length) % items.length;
			} else if (e.key === "Enter") {
				e.preventDefault();
				if (recSuggestIndex >= 0 && recSuggestIndex < items.length) {
					const name = items[recSuggestIndex].textContent || "";
					recF.input.value = name;
				}
				recSuggestBox.classList.add("hidden");
				return;
			} else {
				return;
			}
			items.forEach((el, idx) => {
				el.classList.toggle(
					"editor-suggest-item-active",
					idx === recSuggestIndex,
				);
			});
		});

		const msgF = fu.makeFormRow(
			"Message",
			fu.makeInput("text", "", "Message Name"),
			true,
		);
		body.appendChild(msgF.row);

		// Suggestion dropdown for message names: prioritizes messages that do not
		// yet appear in routing. Acts like a simple typeahead; user can still
		// free-type any name.
		let suggestIndex = -1;
		const suggestBox = document.createElement("div");
		suggestBox.className = "editor-suggest hidden";
		msgF.row.appendChild(suggestBox);

		function renderSuggestions(term) {
			// Hard guard: never open the dropdown unless this input is focused.
			if (document.activeElement !== msgF.input) {
				suggestBox.classList.add("hidden");
				return;
			}
			if (!allMessageNames.length) {
				suggestBox.classList.add("hidden");
				suggestBox.innerHTML = "";
				return;
			}
			const q = String(term || "").toLowerCase();
			const matches = allMessageNames
				.filter((name) => !q || name.toLowerCase().includes(q))
				.sort((a, b) => {
					const aUsed = hasRoute.get(a) === true ? 1 : 0;
					const bUsed = hasRoute.get(b) === true ? 1 : 0;
					if (aUsed !== bUsed) return aUsed - bUsed; // unused (0) first
					return a.localeCompare(b);
				});
			if (!matches.length) {
				suggestBox.classList.add("hidden");
				suggestBox.innerHTML = "";
				return;
			}
			suggestBox.innerHTML = "";
			suggestIndex = 0;
			matches.slice(0, 20).forEach((name, idx) => {
				const item = document.createElement("div");
				item.className =
					"editor-suggest-item" +
					(idx === 0 ? " editor-suggest-item-active" : "");
				item.textContent = name;
				item.addEventListener("mousedown", (e) => {
					e.preventDefault();
					msgF.input.value = name;
					suggestBox.classList.add("hidden");
				});
				suggestBox.appendChild(item);
			});
			suggestBox.classList.remove("hidden");
		}

		msgF.input.addEventListener("input", () => {
			renderSuggestions(msgF.input.value);
		});
		msgF.input.addEventListener("focus", () => {
			if (!allMessageNames.length) loadCatalogSuggestions();
			renderSuggestions(msgF.input.value);
		});
		msgF.input.addEventListener("blur", () => {
			// Delay hiding slightly so click on suggestion can register.
			setTimeout(() => {
				suggestBox.classList.add("hidden");
			}, 150);
		});
		msgF.input.addEventListener("keydown", (e) => {
			const items = suggestBox.querySelectorAll(".editor-suggest-item");
			if (!items.length || suggestBox.classList.contains("hidden")) return;
			if (e.key === "ArrowDown") {
				e.preventDefault();
				suggestIndex = (suggestIndex + 1) % items.length;
			} else if (e.key === "ArrowUp") {
				e.preventDefault();
				suggestIndex = (suggestIndex - 1 + items.length) % items.length;
			} else if (e.key === "Enter") {
				e.preventDefault();
				if (suggestIndex >= 0 && suggestIndex < items.length) {
					const name = items[suggestIndex].textContent || "";
					msgF.input.value = name;
				}
				suggestBox.classList.add("hidden");
				return;
			} else {
				return;
			}
			items.forEach((el, idx) => {
				el.classList.toggle("editor-suggest-item-active", idx === suggestIndex);
			});
		});

		const ovrF = fu.makeFormRow(
			"CAN ID Override",
			fu.makeInput("text", "", "0x1806E5F4 (optional)"),
		);
		body.appendChild(ovrF.row);

		const cancelBtn = fu.makeBtn("Cancel");
		cancelBtn.addEventListener("click", () => fu.closeOverlay(overlay));
		const saveBtn = fu.makeBtn("Add", "editor-btn-primary");
		footer.appendChild(cancelBtn);
		footer.appendChild(saveBtn);

		// Prime suggestions asynchronously; inputs still work as plain text while loading.
		loadCatalogSuggestions();

		saveBtn.addEventListener("click", () => {
			let ok = true;

			const dev = devF.input.value.trim();
			if (!dev) {
				devF.error.textContent = "Required";
				ok = false;
			} else devF.error.textContent = "";

			const bus = busF.input.value;
			const rec = recF.input.value.trim();
			if (!rec) {
				recF.error.textContent = "Required";
				ok = false;
			} else recF.error.textContent = "";

			const msg = msgF.input.value.trim();
			if (!msg) {
				msgF.error.textContent = "Required";
				ok = false;
			} else if (!editor.messageNameExists(msg)) {
				msgF.error.textContent = "Must exist in Message ID or Custom CAN ID";
				ok = false;
			} else msgF.error.textContent = "";

			const ovr = ovrF.input.value.trim();
			if (ovr && !/^0x[0-9a-fA-F]+$/.test(ovr)) {
				ovrF.error.textContent = "Hex format";
				ok = false;
			} else ovrF.error.textContent = "";

			if (!ok) return;
			if (editor.routeEntryExists(dev, bus, rec, msg, ovr || null)) {
				fu.closeOverlay(overlay, { force: true });
				return;
			}

			const lines = editor.getLines();
			const devRange = editor.findRoutingDeviceRange(dev);
			let createdNode = false;
			let createdBus = false;

			if (!devRange) {
				createdNode = true;
				createdBus = true;
				const rStart = editor.findSectionStart(lines, "routing");
				if (rStart === -1) return;
				const rEnd = editor.findSectionEnd(lines, rStart);
				editor.insertAtLine(
					rEnd,
					"    " +
						dev +
						":\n      " +
						bus +
						":\n        " +
						rec +
						":\n" +
						editor.generateRoutingMsgYaml(msg, ovr || null),
				);
			} else {
				const busRange = editor.findRoutingBusRange(dev, bus);
				if (!busRange) {
					createdBus = true;
					editor.insertAtLine(
						devRange.endLine,
						"      " +
							bus +
							":\n        " +
							rec +
							":\n" +
							editor.generateRoutingMsgYaml(msg, ovr || null),
					);
				} else {
					let recFound = false;
					const freshLines = editor.getLines();
					for (let i = busRange.startLine + 1; i < busRange.endLine; i++) {
						if (
							freshLines[i].search(/\S/) === 8 &&
							freshLines[i].trim() === rec + ":"
						) {
							// findBlockEnd locates the end of this receiver block
							// (indent ≤ 8), bounded by the parent bus block end.
							const recEnd = editor.findBlockEnd(
								freshLines,
								i,
								busRange.endLine,
								8,
							);
							editor.insertAtLine(
								recEnd,
								editor.generateRoutingMsgYaml(msg, ovr || null),
							);
							recFound = true;
							break;
						}
					}
					if (!recFound) {
						editor.insertAtLine(
							busRange.endLine,
							"        " +
								rec +
								":\n" +
								editor.generateRoutingMsgYaml(msg, ovr || null),
						);
					}
				}
			}

			if (createdNode) editor.markNew("routeNode:" + dev);
			else editor.markEdited("routeNode:" + dev);
			if (createdBus) editor.markNew("routeBus:" + dev + "|" + bus);
			else editor.markEdited("routeBus:" + dev + "|" + bus);
			editor.markNew("routeMsg:" + dev + "|" + bus + "|" + msg);

			fu.closeOverlay(overlay, { force: true });
			editor.triggerReRender();
		});
	}

	window.GrcanEditor.showRoutingAddForm = showRoutingAddForm;
})();
