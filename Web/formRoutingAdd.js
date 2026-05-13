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
		let receiverList = []; // topology-filtered; drives the autocomplete dropdown
		let _fullReceiverList = []; // all known nodes pre-filter; used for inline topology warning

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

		// busPortFilter: if provided (e.g. "Charger"), only marks messages as used
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
					// "      <BusName>:" — toggle tracking based on bus match
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
		function loadCatalogSuggestions() {
			const rawText = editor.getRawText ? editor.getRawText() : "";
			// Read the live bus value so filtering updates when the user changes it.
			const currentBusPort = busF.input.value || null;
			buildRouteUsageMap(rawText, currentBusPort);
			const routingNames = buildRoutingReceiverSet(rawText, currentBusPort);
			const messages =
				window.GrcanApi && window.GrcanApi.parseMessageCatalogFromText
					? window.GrcanApi.parseMessageCatalogFromText(rawText)
					: [];
			const nodes =
				window.GrcanApi && window.GrcanApi.parseNodeCatalogFromText
					? window.GrcanApi.parseNodeCatalogFromText(rawText)
					: [];
			allMessageNames = [...new Set(messages)];
			// For receivers: when a bus is locked, routingNames already contains
			// the bus-local receivers; don't mix in nodes from other buses.
			const baseReceivers = currentBusPort
				? [...routingNames]
				: [...new Set([...nodes, ...routingNames])];
			// Store the full pre-filter list for inline topology warning checks.
			_fullReceiverList = [...new Set(baseReceivers)];
			// Apply physical topology filter: only show nodes physically on the selected bus.
			const _topo = window.PhysicalTopology;
			const filteredReceivers =
				currentBusPort && _topo && _topo.isLoaded()
					? baseReceivers.filter((name) => _topo.isOnBus(name, currentBusPort))
					: baseReceivers;
			receiverList = [...new Set(filteredReceivers)].sort((a, b) =>
				a.localeCompare(b),
			);
			if (!receiverList.includes("ALL")) receiverList.unshift("ALL");
		}

		const devF = fu.makeFormRow(
			"Device",
			fu.makeInput("text", deviceName || "", "Device Name"),
			true,
		);
		if (deviceName) devF.input.disabled = true;
		body.appendChild(devF.row);

		// GR ID field: only shown (and required) when the typed device name is new.
		const grIdF = fu.makeFormRow(
			"GR ID (new device)",
			fu.makeInput("text", "", "e.g. 0x2B"),
			false,
		);
		grIdF.row.style.display = "none";
		body.appendChild(grIdF.row);

		function updateGrIdVisibility() {
			const isNew =
				!devF.input.disabled &&
				!window.GrcanDocument.deviceExists(devF.input.value.trim());
			grIdF.row.style.display = isNew && devF.input.value.trim() ? "" : "none";
		}
		devF.input.addEventListener("input", updateGrIdVisibility);
		// Run once on open in case a device name was pre-filled.
		if (!devF.input.disabled) updateGrIdVisibility();

		// Filter available buses to only those the device is physically wired to.
		// Only applies when bus is NOT already locked (busPort provided) — if the
		// bus is pre-selected, the select is disabled anyway, so filtering its
		// option list would cause the locked value to have no matching option and
		// the browser would silently default to the first entry (wrong bus).
		const _allBuses = window.GrcanDocument.getBusNames();
		const _topoForBus = window.PhysicalTopology;
		const _availableBuses =
			!busPort && deviceName && _topoForBus && _topoForBus.isLoaded()
				? _allBuses.filter((b) => _topoForBus.isOnBus(deviceName, b))
				: _allBuses;
		const _busChoices =
			_availableBuses.length > 0 ? _availableBuses : _allBuses;
		const busF = fu.makeFormRow(
			"Bus",
			fu.makeSelect(_busChoices, busPort || _busChoices[0] || ""),
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

		// Eagerly populate suggestions so "ALL" and other candidates are ready
		// before the user focuses the receiver field.
		loadCatalogSuggestions();

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

		// Warn (but do not block) when the user manually types a node that is
		// recognized but not physically wired to the selected bus.
		function checkReceiverTopology() {
			const val = recF.input.value.trim();
			const currentBus = busF.input.value || null;
			const topo = window.PhysicalTopology;
			if (!val || !currentBus || !topo || !topo.isLoaded()) {
				recF.error.textContent = "";
				return;
			}
			if (_fullReceiverList.includes(val) && !topo.isOnBus(val, currentBus)) {
				recF.error.textContent = `"${val}" is not physically on ${currentBus}.`;
			} else {
				recF.error.textContent = "";
			}
		}

		recF.input.addEventListener("input", () => {
			renderReceiverSuggestions(recF.input.value);
			checkReceiverTopology();
		});
		recF.input.addEventListener("focus", () => {
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
					syncOverrideForMessage(name);
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
					syncOverrideForMessage(name);
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

		// Auto-fill the override field when the selected message is a Custom CAN ID
		// message (whose canId is stored as bare hex, e.g. "2416"). The routing
		// section requires 0x-prefixed format, so we prefix it here.
		// _autoFilled tracks whether the current value was set programmatically so
		// we can clear it when the user switches to a non-custom message, without
		// ever clearing a value the user typed themselves.
		ovrF.input._autoFilled = false;
		ovrF.input.addEventListener("input", () => {
			ovrF.input._autoFilled = false;
		});

		function syncOverrideForMessage(name) {
			if (!name) return;
			if (window.GrcanEditor.isCustomCanIdMessage(name)) {
				const def =
					window.GrcanDocument && window.GrcanDocument.getCustomCanIdDef(name);
				if (def && def.canId && !ovrF.input.value.trim()) {
					ovrF.input.value = "0x" + def.canId.toUpperCase();
					ovrF.input._autoFilled = true;
				}
			} else if (ovrF.input._autoFilled) {
				ovrF.input.value = "";
				ovrF.input._autoFilled = false;
			}
		}

		msgF.input.addEventListener("blur", () =>
			syncOverrideForMessage(msgF.input.value.trim()),
		);

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

			// Check that the sender device is physically wired to the selected bus.
			// Only enforced for existing devices — new devices aren't in the topology file yet.
			const _isExistingDev =
				dev && window.GrcanDocument && window.GrcanDocument.deviceExists(dev);
			const _topo = window.PhysicalTopology;
			if (
				_isExistingDev &&
				_topo &&
				_topo.isLoaded() &&
				!_topo.isOnBus(dev, bus)
			) {
				busF.error.textContent = `"${dev}" is not physically wired to ${bus}`;
				ok = false;
			} else {
				busF.error.textContent = "";
			}

			const rec = recF.input.value.trim();
			if (!rec) {
				recF.error.textContent = "Required";
				ok = false;
			} else if (
				window.GrcanDocument &&
				!window.GrcanDocument.grIdExists(rec)
			) {
				// Receiver must be registered in GR ID before it can be used as a route target.
				recF.error.textContent = "Node does not exist";
				ok = false;
			} else if (_topo && _topo.isLoaded() && !_topo.isOnBus(rec, bus)) {
				recF.error.textContent = `"${rec}" is not physically on ${bus}`;
				ok = false;
			} else {
				recF.error.textContent = "";
			}

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

			// If device is new, validate and create it with a GR ID first.
			const isNewDevice = !window.GrcanDocument.deviceExists(dev);
			if (isNewDevice) {
				const grId = grIdF.input.value.trim();
				if (!grId || !/^0x[0-9a-fA-F]+$/i.test(grId)) {
					grIdF.error.textContent = "Required for new device (hex, e.g. 0x2B)";
					return;
				}
				const addResult = window.GrcanDocument.addDevice(dev, grId);
				if (!addResult.ok) {
					devF.error.textContent = addResult.error;
					return;
				}
			}

			const routeResult = window.GrcanDocument.addRoute(
				dev,
				bus,
				rec,
				msg,
				ovr || null,
			);
			if (!routeResult.ok) {
				msgF.error.textContent = routeResult.error;
				return;
			}

			if (isNewDevice) editor.markNew("routeNode:" + dev);
			else editor.markEdited("routeNode:" + dev);
			editor.markNew("routeBus:" + dev + "|" + bus);
			editor.markNew("routeMsg:" + dev + "|" + bus + "|" + msg);

			fu.closeOverlay(overlay, { force: true });
			editor.triggerReRender();
		});
	}

	window.GrcanEditor.showRoutingAddForm = showRoutingAddForm;
})();
