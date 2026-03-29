// Purpose: "Edit Route" modal form.
// Lets users modify receiver assignments and CAN ID overrides for an existing
// routing entry without having to delete and re-add it manually.
// Device, Bus, and Message are locked (read-only). Only receivers and their
// optional CAN ID overrides are editable.
// Depends on: formUtils.js (FormUtils), editor.js (GrcanEditor), candoDocument.js.
// Registers: window.GrcanEditor.showRoutingEditForm

(function () {
	"use strict";

	function showRoutingEditForm(deviceName, busPort, msgName, currentReceivers) {
		const editor = window.GrcanEditor;
		const fu = window.FormUtils;
		const { overlay, body, footer } = fu.createModal(
			"Edit Route \u2014 " + msgName,
		);
		overlay.querySelector(".editor-modal").classList.add("editor-modal-wide");

		// Compact context subtitle (device · bus) instead of locked row fields.
		const subtitle = document.createElement("p");
		subtitle.className = "route-edit-subtitle";
		subtitle.textContent = deviceName + " \u00b7 " + busPort;
		body.appendChild(subtitle);

		// Working copy of receiver rows. Mutated by add/remove actions before Save.
		let rows = currentReceivers.map((r) => ({
			receiverName: r.receiverName,
			canIdOverride: r.canIdOverride || "",
		}));

		// Receiver name candidates for autocomplete (shared across all rows).
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

		function buildRoutingReceiverSet(rawText) {
			const names = new Set();
			const lines = String(rawText || "").split("\n");
			const bounds = routingSectionBounds(lines);
			if (!bounds) return names;
			let activeBus = false;
			for (let i = bounds.start + 1; i < bounds.end; i++) {
				const line = lines[i];
				if (!line.trim()) continue;
				const indent = line.search(/\S/);
				const content = line.trim();
				if (indent === 6) {
					activeBus = content === busPort + ":";
				} else if (indent === 4) {
					activeBus = false;
				} else if (activeBus && indent === 8 && content.endsWith(":")) {
					names.add(content.slice(0, -1).trim());
				}
			}
			return names;
		}

		function loadReceiverList() {
			const rawText = editor.getRawText ? editor.getRawText() : "";
			const routingNames = buildRoutingReceiverSet(rawText);
			const nodes =
				window.GrcanApi && window.GrcanApi.parseNodeCatalogFromText
					? window.GrcanApi.parseNodeCatalogFromText(rawText)
					: [];
			const base = [...new Set([...nodes, ...routingNames])];
			receiverList = [...new Set(base)].sort((a, b) => a.localeCompare(b));
			if (!receiverList.includes("ALL")) receiverList.unshift("ALL");
		}

		// ── Locked header fields ──────────────────────────────────────────────────

		// ── Receiver assignments section ──────────────────────────────────────────

		// Column headers aligned with the receiver rows below.
		const colHeaders = document.createElement("div");
		colHeaders.className = "route-edit-col-headers";
		colHeaders.innerHTML = "<span>Receiver</span><span>CAN ID Override</span>";
		body.appendChild(colHeaders);

		const rowsContainer = document.createElement("div");
		rowsContainer.className = "route-edit-rows";
		body.appendChild(rowsContainer);

		// Inline error shown below the rows container.
		const rowsError = document.createElement("div");
		rowsError.className = "editor-error";
		body.appendChild(rowsError);

		function makeReceiverSuggestBox(inputEl) {
			let suggestIndex = -1;
			const suggestBox = document.createElement("div");
			suggestBox.className = "editor-suggest hidden";

			function renderSuggestions(term) {
				if (document.activeElement !== inputEl) {
					suggestBox.classList.add("hidden");
					return;
				}
				const q = String(term || "").toLowerCase();
				const matches = receiverList.filter(
					(name) => !q || name.toLowerCase().includes(q),
				);
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
						inputEl.value = name;
						suggestBox.classList.add("hidden");
					});
					suggestBox.appendChild(item);
				});
				suggestBox.classList.remove("hidden");
			}

			inputEl.addEventListener("input", () => renderSuggestions(inputEl.value));
			inputEl.addEventListener("focus", () => renderSuggestions(inputEl.value));
			inputEl.addEventListener("blur", () => {
				setTimeout(() => suggestBox.classList.add("hidden"), 150);
			});
			inputEl.addEventListener("keydown", (e) => {
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
						inputEl.value = items[suggestIndex].textContent || "";
					}
					suggestBox.classList.add("hidden");
					return;
				} else {
					return;
				}
				items.forEach((el, idx) =>
					el.classList.toggle(
						"editor-suggest-item-active",
						idx === suggestIndex,
					),
				);
			});

			return suggestBox;
		}

		function renderRows() {
			rowsContainer.innerHTML = "";
			rows.forEach((row, idx) => {
				const rowEl = document.createElement("div");
				rowEl.className = "route-edit-row";

				// Receiver input
				const recInput = document.createElement("input");
				recInput.type = "text";
				recInput.className = "editor-input route-edit-rec-input";
				recInput.placeholder = "Receiver Name";
				recInput.value = row.receiverName;
				recInput.addEventListener("input", () => {
					rows[idx].receiverName = recInput.value;
				});

				const recWrap = document.createElement("div");
				recWrap.className = "route-edit-rec-wrap";
				recWrap.appendChild(recInput);
				recWrap.appendChild(makeReceiverSuggestBox(recInput));

				// CAN ID Override input
				const ovrInput = document.createElement("input");
				ovrInput.type = "text";
				ovrInput.className = "editor-input route-edit-ovr-input";
				ovrInput.placeholder = "0x... (optional)";
				ovrInput.value = row.canIdOverride;
				ovrInput.addEventListener("input", () => {
					rows[idx].canIdOverride = ovrInput.value;
				});

				// Per-row error span
				const rowErr = document.createElement("span");
				rowErr.className = "route-edit-row-error editor-error";

				// Remove button
				const removeBtn = document.createElement("button");
				removeBtn.type = "button";
				removeBtn.className = "editor-icon-btn route-edit-remove-btn";
				removeBtn.title = "Remove";
				removeBtn.textContent = "\u00d7";
				removeBtn.addEventListener("click", () => {
					rows.splice(idx, 1);
					renderRows();
				});

				rowEl.appendChild(recWrap);
				rowEl.appendChild(ovrInput);
				rowEl.appendChild(rowErr);
				rowEl.appendChild(removeBtn);
				rowsContainer.appendChild(rowEl);
			});
		}

		// Populate suggestions and render initial rows.
		loadReceiverList();
		renderRows();

		// [+ Add Receiver] button
		const addRecBtn = fu.makeBtn("+ Add Receiver");
		addRecBtn.className = "editor-btn route-edit-add-btn";
		addRecBtn.addEventListener("click", () => {
			rows.push({ receiverName: "", canIdOverride: "" });
			renderRows();
			// Focus the new receiver input
			const inputs = rowsContainer.querySelectorAll(".route-edit-rec-input");
			if (inputs.length) inputs[inputs.length - 1].focus();
		});
		body.appendChild(addRecBtn);

		// ── Footer ────────────────────────────────────────────────────────────────

		const cancelBtn = fu.makeBtn("Cancel");
		cancelBtn.addEventListener("click", () => fu.closeOverlay(overlay));
		const saveBtn = fu.makeBtn("Save", "editor-btn-primary");
		footer.appendChild(cancelBtn);
		footer.appendChild(saveBtn);

		saveBtn.addEventListener("click", () => {
			rowsError.textContent = "";
			// Clear per-row errors
			rowsContainer
				.querySelectorAll(".route-edit-row-error")
				.forEach((el) => (el.textContent = ""));

			// Validate: at least one non-empty receiver
			const filledRows = rows.filter((r) => r.receiverName.trim());
			if (!filledRows.length) {
				rowsError.textContent = "At least one receiver is required";
				return;
			}

			// Validate: no duplicate receivers
			const names = filledRows.map((r) => r.receiverName.trim());
			const dupeSet = new Set();
			let hasDupe = false;
			names.forEach((n) => {
				if (dupeSet.has(n)) hasDupe = true;
				dupeSet.add(n);
			});
			if (hasDupe) {
				rowsError.textContent = "Duplicate receiver names";
				return;
			}

			// Validate: CAN ID overrides are hex if filled
			let ovrValid = true;
			filledRows.forEach((r, idx) => {
				const ovr = r.canIdOverride.trim();
				if (ovr && !/^0x[0-9a-fA-F]+$/.test(ovr)) {
					const rowEls = rowsContainer.querySelectorAll(".route-edit-row");
					const errEl =
						rowEls[idx] && rowEls[idx].querySelector(".route-edit-row-error");
					if (errEl)
						errEl.textContent = "Hex format required (e.g. 0x1806E5F4)";
					ovrValid = false;
				}
			});
			if (!ovrValid) return;

			// Build diff maps
			const originalMap = new Map(
				currentReceivers.map((r) => [r.receiverName, r.canIdOverride || null]),
			);
			const newMap = new Map(
				filledRows.map((r) => [
					r.receiverName.trim(),
					r.canIdOverride.trim() || null,
				]),
			);

			let anyChange = false;

			// Removed: in original but not in new
			for (const [recName] of originalMap) {
				if (!newMap.has(recName)) {
					window.GrcanDocument.deleteRouteFromReceiver(
						deviceName,
						busPort,
						recName,
						msgName,
					);
					anyChange = true;
				}
			}

			// Added or changed
			for (const [recName, ovr] of newMap) {
				const origOvr = originalMap.get(recName);
				if (origOvr === undefined) {
					// New receiver
					window.GrcanDocument.addRoute(
						deviceName,
						busPort,
						recName,
						msgName,
						ovr,
					);
					anyChange = true;
				} else if (origOvr !== ovr) {
					// Same receiver, updated CAN ID override
					window.GrcanDocument.deleteRouteFromReceiver(
						deviceName,
						busPort,
						recName,
						msgName,
					);
					window.GrcanDocument.addRoute(
						deviceName,
						busPort,
						recName,
						msgName,
						ovr,
					);
					anyChange = true;
				}
			}

			if (anyChange) {
				editor.markEdited(
					"routeMsg:" + deviceName + "|" + busPort + "|" + msgName,
				);
			}

			fu.closeOverlay(overlay, { force: true });
			editor.triggerReRender();
		});
	}

	window.GrcanEditor.showRoutingEditForm = showRoutingEditForm;
})();
