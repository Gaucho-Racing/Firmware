// Purpose: Core CANdo text mutation engine. Owns all in-memory editing state
// (rawCandoText, editMode, editedKeys, newKeys) and exposes low-level operations
// for finding, inserting, replacing, and deleting YAML line ranges. Also provides
// icon-button DOM factories (createEditBtn, createDeleteBtn, createAddBtn) used by
// viewer.js to inject editor controls into list items.
// Individual form modals are defined in their own form*.js files, which each
// augment this object by assigning their show* / confirm* methods onto it.
// Depends on: formUtils.js (for SVG icon strings on icon buttons).
// Exposed as: window.GrcanEditor

(function () {
	"use strict";

	// ==================== State ====================

	let editMode = false;
	let rawCandoText = "";
	let originalRawCandoText = "";
	let hasEdits = false;
	let reRenderCallback = null;
	let pendingNavSnapshot = null;
	const editedKeys = new Set();
	const newKeys = new Set();

	// ==================== Line-Range Helpers ====================

	function getLines() {
		return rawCandoText.split("\n");
	}

	function findSectionStart(lines, sectionName) {
		return lines.findIndex((l) => l.startsWith(sectionName + ":"));
	}

	function findSectionEnd(lines, startIdx) {
		for (let i = startIdx + 1; i < lines.length; i++) {
			if (/^\S/.test(lines[i]) && lines[i].trim() !== "") return i;
		}
		return lines.length;
	}

	function findBlockEnd(lines, startIdx, maxIdx, baseIndent) {
		let end = startIdx + 1;
		while (end < maxIdx) {
			const ni = lines[end].search(/\S/);
			if (ni !== -1 && ni <= baseIndent) break;
			end++;
		}
		return end;
	}

	function findEntryInSection(sectionName, entryName, indent) {
		const lines = getLines();
		const secStart = findSectionStart(lines, sectionName);
		if (secStart === -1) return null;
		const secEnd = findSectionEnd(lines, secStart);
		for (let i = secStart + 1; i < secEnd; i++) {
			const line = lines[i];
			if (line.search(/\S/) === indent && line.trim() === entryName + ":") {
				return {
					startLine: i,
					endLine: findBlockEnd(lines, i, secEnd, indent),
				};
			}
		}
		return null;
	}

	function findMessageDefRange(msgName) {
		return findEntryInSection("Message ID", msgName, 2);
	}

	function findCustomCanIdRange(msgName) {
		return findEntryInSection("Custom CAN ID", msgName, 2);
	}

	function findRoutingDeviceRange(deviceName) {
		const lines = getLines();
		const rStart = findSectionStart(lines, "routing");
		if (rStart === -1) return null;
		const rEnd = findSectionEnd(lines, rStart);
		for (let i = rStart + 1; i < rEnd; i++) {
			if (lines[i].search(/\S/) === 4 && lines[i].trim() === deviceName + ":") {
				return { startLine: i, endLine: findBlockEnd(lines, i, rEnd, 4) };
			}
		}
		return null;
	}

	function findRoutingBusRange(deviceName, busPort) {
		const devRange = findRoutingDeviceRange(deviceName);
		if (!devRange) return null;
		const lines = getLines();
		for (let i = devRange.startLine + 1; i < devRange.endLine; i++) {
			if (lines[i].search(/\S/) === 6 && lines[i].trim() === busPort + ":") {
				return {
					startLine: i,
					endLine: findBlockEnd(lines, i, devRange.endLine, 6),
				};
			}
		}
		return null;
	}

	function findRoutingMsgEntries(deviceName, busPort, msgName) {
		const busRange = findRoutingBusRange(deviceName, busPort);
		if (!busRange) return [];
		const lines = getLines();
		const results = [];
		for (let i = busRange.startLine + 1; i < busRange.endLine; i++) {
			if (
				lines[i].search(/\S/) === 10 &&
				lines[i].trim() === "- msg: " + msgName
			) {
				let end = i + 1;
				if (
					end < busRange.endLine &&
					lines[end].search(/\S/) === 12 &&
					lines[end].trim().startsWith("can_id_override:")
				) {
					end++;
				}
				results.push({ startLine: i, endLine: end });
			}
		}
		return results;
	}

	// ==================== Text Mutation ====================

	function spliceLines(startLine, deleteCount, newText) {
		const lines = getLines();
		const newLines =
			newText && newText.length > 0
				? (newText.endsWith("\n") ? newText.slice(0, -1) : newText).split("\n")
				: [];
		lines.splice(startLine, deleteCount, ...newLines);
		rawCandoText = lines.join("\n");
		hasEdits = true;
	}

	function markEdited(key) {
		if (key) editedKeys.add(key);
	}

	function isEdited(key) {
		return !!key && editedKeys.has(key);
	}

	function markNew(key) {
		if (key) newKeys.add(key);
	}

	function isNew(key) {
		return !!key && newKeys.has(key);
	}

	function resetEditState() {
		hasEdits = false;
		editedKeys.clear();
		newKeys.clear();
	}

	function deleteLineRange(startLine, endLine) {
		spliceLines(startLine, endLine - startLine, null);
	}

	function replaceLineRange(startLine, endLine, newText) {
		spliceLines(startLine, endLine - startLine, newText);
	}

	function insertAtLine(lineIdx, newText) {
		spliceLines(lineIdx, 0, newText);
	}

	function getLineRangeText(startLine, endLine) {
		const lines = getLines();
		const body = lines.slice(startLine, endLine).join("\n");
		return body ? body + "\n" : "";
	}

	function routeEntryExists(
		deviceName,
		busPort,
		receiver,
		msgName,
		canIdOverride,
	) {
		const busRange = findRoutingBusRange(deviceName, busPort);
		if (!busRange) return false;
		const lines = getLines();
		let curReceiver = null;
		for (let i = busRange.startLine + 1; i < busRange.endLine; i++) {
			const indent = lines[i].search(/\S/);
			const content = lines[i].trim();
			if (indent === 8) curReceiver = content.replace(/:$/, "");
			if (
				indent === 10 &&
				content === "- msg: " + msgName &&
				curReceiver === receiver
			) {
				const next = lines[i + 1] || "";
				const hasOverride =
					next.search(/\S/) === 12 &&
					next.trim().startsWith("can_id_override:");
				if (!canIdOverride && !hasOverride) return true;
				if (canIdOverride && hasOverride) {
					const ov = next.split(":")[1].trim();
					if (ov === canIdOverride) return true;
				}
			}
		}
		return false;
	}

	function getMessageIdEntries() {
		const lines = getLines();
		const start = lines.findIndex((l) => l.startsWith("Message ID:"));
		if (start === -1) return [];
		const end = findSectionEnd(lines, start);
		const entries = [];
		let cur = null;
		for (let i = start + 1; i < end; i++) {
			const raw = lines[i];
			const indent = raw.search(/\S/);
			if (indent === -1) continue;
			const content = raw.trim();
			if (indent === 2 && content.endsWith(":")) {
				cur = { name: content.slice(0, -1), msgId: null };
				entries.push(cur);
				continue;
			}
			if (cur && indent === 4 && content.startsWith("MSG ID:")) {
				cur.msgId = content.slice("MSG ID:".length).trim().toLowerCase();
			}
		}
		return entries;
	}

	function getGrIdEntries() {
		const lines = getLines();
		const start = lines.findIndex((l) => l.startsWith("GR ID:"));
		if (start === -1) return [];
		const end = findSectionEnd(lines, start);
		const entries = [];
		for (let i = start + 1; i < end; i++) {
			const raw = lines[i];
			const indent = raw.search(/\S/);
			if (indent !== 2) continue;
			const content = raw.trim();
			const colonIdx = content.indexOf(":");
			if (colonIdx <= 0) continue;
			const name = content.slice(0, colonIdx).trim();
			if (!name) continue;
			const idMatch = content.match(/:\s*["']?([^"'\s]+)["']?/);
			entries.push({ name, id: idMatch ? idMatch[1] : null, line: i });
		}
		return entries;
	}

	function findGrIdEntryRange(nodeName) {
		if (!nodeName) return null;
		const lines = getLines();
		const start = lines.findIndex((l) => l.startsWith("GR ID:"));
		if (start === -1) return null;
		const end = findSectionEnd(lines, start);
		for (let i = start + 1; i < end; i++) {
			const raw = lines[i];
			const indent = raw.search(/\S/);
			if (indent !== 2) continue;
			const content = raw.trim();
			const colonIdx = content.indexOf(":");
			if (colonIdx <= 0) continue;
			const name = content.slice(0, colonIdx).trim();
			if (name === nodeName) {
				return { startLine: i, endLine: i + 1 };
			}
		}
		return null;
	}

	function grIdNameExists(nodeName) {
		return !!findGrIdEntryRange(nodeName);
	}

	function renameGrIdNode(oldName, newName) {
		if (!oldName || !newName || oldName === newName) return false;
		const range = findGrIdEntryRange(oldName);
		if (!range) return false;
		const lines = getLines();
		const raw = lines[range.startLine];
		const colonIdx = raw.indexOf(":");
		if (colonIdx === -1) return false;
		const leadingWs = raw.match(/^\s*/);
		const prefix = leadingWs ? leadingWs[0] : "";
		const rest = raw.slice(colonIdx);
		const nextLine = prefix + newName + rest;
		if (nextLine === raw) return false;
		lines[range.startLine] = nextLine;
		rawCandoText = lines.join("\n");
		hasEdits = true;
		return true;
	}

	function messageNameExists(msgName) {
		return !!findMessageDefRange(msgName) || !!findCustomCanIdRange(msgName);
	}

	function isCustomCanIdMessage(msgName) {
		return !!findCustomCanIdRange(msgName);
	}

	function renameRoutingMessageRefs(oldName, newName) {
		if (!oldName || !newName || oldName === newName) return false;
		const lines = getLines();
		let changed = false;
		for (let i = 0; i < lines.length; i++) {
			if (
				lines[i].search(/\S/) === 10 &&
				lines[i].trim() === "- msg: " + oldName
			) {
				lines[i] = lines[i].replace("- msg: " + oldName, "- msg: " + newName);
				changed = true;
			}
		}
		if (changed) {
			rawCandoText = lines.join("\n");
			hasEdits = true;
		}
		return changed;
	}

	// ==================== YAML Generators ====================

	function generateMessageIdYaml(data) {
		let y = "  " + data.name + ":\n";
		y += "    MSG ID: " + data.msgId + "\n";
		y += "    MSG LENGTH: " + data.msgLength + "\n";
		for (const f of data.fields) {
			y += "    " + f.name + ":\n";
			y += "      bit_start: " + f.bitStart + "\n";
			if (f.comment) {
				f.comment.split("\n").forEach((line) => {
					y += "      # " + line.trim() + "\n";
				});
			}
			y += "      data type: " + f.dataType + "\n";
			if (f.units) y += "      units: " + f.units + "\n";
			if (f.scaledMin !== undefined && f.scaledMin !== "")
				y += "      scaled min: " + f.scaledMin + "\n";
			if (f.scaledMax !== undefined && f.scaledMax !== "")
				y += "      scaled max: " + f.scaledMax + "\n";
			if (f.mapEquation) y += '      map equation: "' + f.mapEquation + '"\n';
		}
		return y;
	}

	function generateRoutingMsgYaml(msgName, canIdOverride) {
		let y = "          - msg: " + msgName + "\n";
		if (canIdOverride)
			y += "            can_id_override: " + canIdOverride + "\n";
		return y;
	}

	// ==================== Re-render ====================

	function triggerReRender() {
		if (typeof reRenderCallback === "function") {
			const snapshot = pendingNavSnapshot;
			pendingNavSnapshot = null;
			reRenderCallback(snapshot);
		}
	}

	// ==================== Icon / Button Creation ====================
	// These create the small inline edit/delete/add buttons injected into viewer list
	// items. SVG strings are sourced from FormUtils to avoid duplication.

	function createEditBtn(onClick) {
		const btn = document.createElement("button");
		btn.type = "button";
		btn.className = "editor-icon-btn editor-icon-edit";
		btn.innerHTML = window.FormUtils.PENCIL_SVG;
		btn.title = "Edit";
		btn.setAttribute("aria-label", "Edit");
		btn.addEventListener("click", (e) => {
			e.stopPropagation();
			onClick();
		});
		return btn;
	}

	function createDeleteBtn(onClick) {
		const btn = document.createElement("button");
		btn.type = "button";
		btn.className = "editor-icon-btn editor-icon-delete";
		btn.innerHTML = window.FormUtils.TRASH_SVG;
		btn.title = "Delete";
		btn.setAttribute("aria-label", "Delete");
		btn.addEventListener("click", (e) => {
			e.stopPropagation();
			onClick();
		});
		return btn;
	}

	function createAddBtn(label, onClick) {
		const btn = document.createElement("button");
		btn.type = "button";
		btn.className = "editor-add-btn";
		btn.innerHTML = window.FormUtils.PLUS_SVG + " " + label;
		btn.addEventListener("click", (e) => {
			e.stopPropagation();
			onClick();
		});
		return btn;
	}

	// ==================== Download ====================

	function downloadCando() {
		const serialized = window.GrcanDocument
			? window.GrcanDocument.getSerializedText()
			: null;
		const blob = new Blob([serialized ?? rawCandoText], { type: "text/plain" });
		const url = URL.createObjectURL(blob);
		const a = document.createElement("a");
		a.href = url;
		a.download = "GRCAN.CANdo";
		document.body.appendChild(a);
		a.click();
		a.remove();
		URL.revokeObjectURL(url);
	}

	// ==================== Public API ====================
	// Form methods (showMessageEditForm, showRoutingAddForm, showRoutingNodeEditForm,
	// showRoutingBusEditForm, confirmAndDelete) are added by their respective form*.js
	// files after this object is created.

	window.GrcanEditor = {
		toggleEditMode() {
			editMode = !editMode;
			document.body.classList.toggle("edit-mode", editMode);
			return editMode;
		},
		isEditMode() {
			return editMode;
		},
		setRawText(text) {
			rawCandoText = text;
			originalRawCandoText = text;
			hasEdits = false;
			editedKeys.clear();
			newKeys.clear();
		},
		// Update working text without resetting original or edit state.
		// Used by GrcanDocument after semantic mutations.
		updateRawText(text) {
			rawCandoText = text;
			hasEdits = true;
		},
		getRawText() {
			return rawCandoText;
		},
		getOriginalRawText() {
			return originalRawCandoText;
		},
		hasUnsavedEdits() {
			return hasEdits;
		},
		setReRenderCallback(cb) {
			reRenderCallback = cb;
		},
		setNavSnapshot(snapshot) {
			pendingNavSnapshot = snapshot || null;
		},
		markEdited,
		isEdited,
		markNew,
		isNew,
		resetEditState,
		downloadCando,
		triggerReRender,
		createEditBtn,
		createDeleteBtn,
		createAddBtn,
		// Line-range accessors used by form files and viewer delete handlers:
		getLines,
		findSectionStart,
		findSectionEnd,
		findBlockEnd,
		findMessageDefRange,
		findRoutingDeviceRange,
		findRoutingBusRange,
		findRoutingMsgEntries,
		deleteLineRange,
		replaceLineRange,
		insertAtLine,
		getLineRangeText,
		routeEntryExists,
		getMessageIdEntries,
		getGrIdEntries,
		findGrIdEntryRange,
		grIdNameExists,
		renameGrIdNode,
		messageNameExists,
		isCustomCanIdMessage,
		renameRoutingMessageRefs,
		generateMessageIdYaml,
		generateRoutingMsgYaml,
	};
})();
