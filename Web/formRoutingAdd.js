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

		// Build message name index for suggestions: all message names from
		// Message ID / Custom CAN ID, with a flag indicating whether they
		// are already used in any route. Unused entries are prioritized in
		// the suggestion list. Also collect a receiver/node name index for
		// simple autocomplete on the Receiver field.
		const allMessageNames = [];
		const hasRoute = new Map();
		const receiverNames = new Set();
		try {
			const rawText = editor.getRawText && editor.getRawText();
			if (rawText) {
				const lines = rawText.split("\n");
				// Collect message names from Message ID section
				const msgStart = lines.findIndex((l) => l.startsWith("Message ID:"));
				if (msgStart !== -1) {
					for (let i = msgStart + 1; i < lines.length; i++) {
						const line = lines[i];
						if (/^\S/.test(line) && line.trim() !== "") break;
						const m = line.match(/^\s{2}([^:]+):\s*$/);
						if (m) {
							const name = m[1].trim();
							allMessageNames.push(name);
						}
					}
				}
				// Collect names from Custom CAN ID section
				const customStart = lines.findIndex((l) =>
					l.startsWith("Custom CAN ID:"),
				);
				if (customStart !== -1) {
					for (let i = customStart + 1; i < lines.length; i++) {
						const line = lines[i];
						if (/^\S/.test(line) && line.trim() !== "") break;
						const m = line.match(/^\s{2}([^:]+):\s*$/);
						if (m) {
							const name = m[1].trim();
							allMessageNames.push(name);
						}
					}
				}
				// Collect node names from GR ID section
				const gridStart = lines.findIndex((l) => l.startsWith("GR ID:"));
				if (gridStart !== -1) {
					for (let i = gridStart + 1; i < lines.length; i++) {
						const line = lines[i];
						if (/^\S/.test(line) && line.trim() !== "") break;
						const m = line.match(
							/^\s+([^:]+):\s*["']?([^"'\s]+)["']?\s*(?:#.*)?$/,
						);
						if (m) {
							const name = m[1].trim();
							if (name) receiverNames.add(name);
						}
					}
				}
				// Track which messages already appear in routing
				const routingStart = lines.findIndex((l) => l.startsWith("routing:"));
				if (routingStart !== -1) {
					for (let i = routingStart + 1; i < lines.length; i++) {
						const line = lines[i];
						if (!line.trim()) continue;
						const indent = line.search(/\S/);
						const content = line.trim();
						// Sender / Device
						if (indent === 4 && content.endsWith(":")) {
							const name = content.slice(0, -1).trim();
							if (name) receiverNames.add(name);
						}
						// Receiver
						if (indent === 8 && content.endsWith(":")) {
							const name = content.slice(0, -1).trim();
							if (name) receiverNames.add(name);
						}
						// Message usage
						const m = content.match(/^- msg:\s*(.+)$/);
						if (m) hasRoute.set(m[1].trim(), true);
					}
				}
			}
		} catch (e) {
			// If anything goes wrong, fall back to simple text input without suggestions.
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
		if (busPort) busF.input.disabled = true;
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
		const receiverList = Array.from(receiverNames).sort((a, b) =>
			a.localeCompare(b),
		);
		let recSuggestIndex = -1;
		const recSuggestBox = document.createElement("div");
		recSuggestBox.className = "editor-suggest hidden";
		recF.row.appendChild(recSuggestBox);

		function renderReceiverSuggestions(term) {
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
				recSuggestIndex =
					(recSuggestIndex - 1 + items.length) % items.length;
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
					"editor-suggest-item" + (idx === 0 ? " editor-suggest-item-active" : "");
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
				fu.closeOverlay(overlay);
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

			fu.closeOverlay(overlay);
			editor.triggerReRender();
		});
	}

	window.GrcanEditor.showRoutingAddForm = showRoutingAddForm;
})();
