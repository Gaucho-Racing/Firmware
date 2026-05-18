// Purpose: "Add / Edit Message Definition" modal form.
// Handles the full lifecycle of the Message ID section entry: parsing the current
// raw YAML into a pre-populated form, validating all fields (required, hex MSG ID,
// positive length, bit layout, overlap checks), writing the result back via
// GrcanEditor mutations, and renaming routing references on rename.
// Depends on: formUtils.js (FormUtils), editor.js (GrcanEditor).
// Registers: window.GrcanEditor.showMessageEditForm

(function () {
	"use strict";

	const VALID_DATA_TYPES = [
		"b",
		"u4",
		"s4",
		"u8",
		"s8",
		"u16",
		"s16",
		"u32",
		"s32",
		"s",
	];

	// ==================== Raw Text Parser ====================
	// Reads the current working text from GrcanEditor to pre-populate the form
	// when editing an existing message definition.

	function parseMessageFromRaw(msgName) {
		const editor = window.GrcanEditor;
		const range = editor.findMessageDefRange(msgName);
		if (!range) return null;
		const lines = editor.getLines().slice(range.startLine, range.endLine);
		const result = { name: msgName, msgId: "", msgLength: "", fields: [] };
		let cur = null;
		let _inComment = false;

		for (const line of lines) {
			const indent = line.search(/\S/);
			if (indent === -1) continue;
			const c = line.trim();

			if (indent === 4) {
				_inComment = false;
				if (c.startsWith("MSG ID:")) {
					result.msgId = c.slice(7).trim();
				} else if (c.startsWith("MSG LENGTH:")) {
					result.msgLength = c.slice(11).trim().replace(/,/g, "");
				} else if (c.endsWith(":")) {
					if (cur) result.fields.push(cur);
					cur = {
						fieldName: c.slice(0, -1),
						bitStart: null,
						bitEnd: null,
						rawDataType: "",
						comment: "",
						units: "",
						scaledMin: "",
						scaledMax: "",
						mapEquation: "",
					};
				}
			} else if (indent >= 6 && cur) {
				// Any indent-6 line starts a new field property; only deeper
				// lines are valid comment continuation lines.
				if (indent === 6) _inComment = false;
				if (c.startsWith("bit_start:")) {
					const v = c.slice(10).trim();
					const rm = v.match(/^(\d+)\s*-\s*(\d+)$/);
					if (rm) {
						cur.bitStart = parseInt(rm[1], 10);
						cur.bitEnd = parseInt(rm[2], 10);
					} else {
						const n = parseInt(v.replace(/,/g, ""), 10);
						if (!isNaN(n)) {
							cur.bitStart = n;
							cur.bitEnd = n;
						}
					}
				} else if (c.startsWith("comment:")) {
					const raw = c.slice("comment:".length).trim();
					const inline = raw === "|" || raw === ">" ? "" : raw;
					cur.comment = inline || "";
					_inComment = true;
				} else if (c.startsWith("#")) {
					// backward compat: old # format
					_inComment = false;
					const t = c.replace(/^#\s*/, "").trim();
					if (t) cur.comment = cur.comment ? cur.comment + "\n" + t : t;
				} else if (!_inComment && c.startsWith("data type:")) {
					const rawType = c.slice(10).trim();
					// Backward compatibility for older aliases while keeping
					// canonical signed type labels in the editor UI.
					if (rawType === "i16") cur.rawDataType = "s16";
					else if (rawType === "i32") cur.rawDataType = "s32";
					else cur.rawDataType = rawType;
				} else if (!_inComment && c.startsWith("units:")) {
					cur.units = c.slice(6).trim();
				} else if (!_inComment && c.startsWith("scaled min:")) {
					cur.scaledMin = c.slice(11).trim();
				} else if (!_inComment && c.startsWith("scaled max:")) {
					cur.scaledMax = c.slice(11).trim();
				} else if (!_inComment && c.startsWith("map equation:")) {
					cur.mapEquation = c
						.slice(13)
						.trim()
						.replace(/^["']|["']$/g, "");
				} else if (_inComment) {
					// continuation lines of the comment: block
					cur.comment = cur.comment ? cur.comment + "\n" + c : c;
				}
			}
		}
		if (cur) result.fields.push(cur);
		return result;
	}

	// ==================== Form ====================

	function showMessageEditForm(msgName, isNewMsg) {
		const editor = window.GrcanEditor;
		const fu = window.FormUtils;

		const existing = isNewMsg ? null : parseMessageFromRaw(msgName);
		const { overlay, body, footer } = fu.createModal(
			isNewMsg ? "Add Message Definition" : "Edit: " + msgName,
		);

		const nameF = fu.makeFormRow(
			"Message Name",
			fu.makeInput("text", isNewMsg ? "" : msgName),
			true,
		);
		const idF = fu.makeFormRow(
			"MSG ID",
			fu.makeInput("text", existing?.msgId || "", "0x000"),
			true,
		);
		const lenF = fu.makeFormRow(
			"MSG LENGTH (bytes)",
			fu.makeInput("number", existing?.msgLength || "", "8"),
			true,
		);
		body.appendChild(nameF.row);
		body.appendChild(idF.row);
		body.appendChild(lenF.row);

		const fieldsHdr = document.createElement("div");
		fieldsHdr.className = "editor-section-header";
		const fieldsTitle = document.createElement("span");
		fieldsTitle.textContent = "Fields";
		fieldsHdr.appendChild(fieldsTitle);
		const addFieldBtn = document.createElement("button");
		addFieldBtn.className = "editor-btn editor-btn-sm";
		addFieldBtn.innerHTML = fu.PLUS_SVG + " Add Field";
		fieldsHdr.appendChild(addFieldBtn);
		body.appendChild(fieldsHdr);

		const fieldsBox = document.createElement("div");
		fieldsBox.className = "editor-fields-box";
		body.appendChild(fieldsBox);

		function addFieldRow(field) {
			const card = document.createElement("div");
			card.className = "editor-field-card";

			const topRow = document.createElement("div");
			topRow.className = "editor-field-grid";

			const fName = fu.makeFormRow(
				"Name",
				fu.makeInput("text", field?.fieldName || "", "Field Name"),
				true,
			);
			const bitVal = field
				? field.bitEnd !== null && field.bitEnd !== field.bitStart
					? field.bitStart + "-" + field.bitEnd
					: field.bitStart !== null
						? String(field.bitStart)
						: ""
				: "";
			const fBit = fu.makeFormRow(
				"bit_start",
				fu.makeInput("text", bitVal, "0 or 0-7"),
				true,
			);
			const fType = fu.makeFormRow(
				"Data Type",
				fu.makeSelect(VALID_DATA_TYPES, field?.rawDataType || "u8"),
				true,
			);
			topRow.appendChild(fName.row);
			topRow.appendChild(fBit.row);
			topRow.appendChild(fType.row);
			card.appendChild(topRow);

			const commentRow = document.createElement("div");
			commentRow.className = "editor-field-comment-row";
			const fComment = fu.makeFormRow(
				"Comment",
				fu.makeInput("textarea", field?.comment || "", "Description"),
			);
			commentRow.appendChild(fComment.row);
			card.appendChild(commentRow);

			const botRow = document.createElement("div");
			botRow.className = "editor-field-grid editor-field-grid-4";
			const fUnits = fu.makeFormRow(
				"Units",
				fu.makeInput("text", field?.units || "", "e.g. Volts"),
			);
			const fMin = fu.makeFormRow(
				"Scaled Min",
				fu.makeInput("text", field?.scaledMin || ""),
			);
			const fMax = fu.makeFormRow(
				"Scaled Max",
				fu.makeInput("text", field?.scaledMax || ""),
			);
			const fEq = fu.makeFormRow(
				"Map Equation",
				fu.makeInput("text", field?.mapEquation || "", "e.g. 0.01x"),
			);
			botRow.appendChild(fUnits.row);
			botRow.appendChild(fMin.row);
			botRow.appendChild(fMax.row);
			botRow.appendChild(fEq.row);
			card.appendChild(botRow);

			const removeBtn = document.createElement("button");
			removeBtn.className = "editor-btn editor-btn-danger editor-btn-sm";
			removeBtn.innerHTML = fu.TRASH_SVG + " Remove";
			removeBtn.addEventListener("click", () => card.remove());

			const controlsRow = document.createElement("div");
			controlsRow.className = "editor-field-controls";
			const upBtn = fu.makeBtn("Move Up", "editor-btn-sm");
			const downBtn = fu.makeBtn("Move Down", "editor-btn-sm");
			upBtn.addEventListener("click", (e) => {
				e.preventDefault();
				const parent = card.parentNode;
				if (!parent) return;
				const prev = card.previousElementSibling;
				if (prev) parent.insertBefore(card, prev);
			});
			downBtn.addEventListener("click", (e) => {
				e.preventDefault();
				const parent = card.parentNode;
				if (!parent) return;
				const next = card.nextElementSibling;
				if (next) parent.insertBefore(next, card);
			});
			controlsRow.appendChild(upBtn);
			controlsRow.appendChild(downBtn);
			controlsRow.appendChild(removeBtn);
			card.appendChild(controlsRow);

			card._getValues = () => ({
				name: fName.input.value.trim(),
				bitStart: fBit.input.value.trim(),
				dataType: fType.input.value,
				comment: fComment.input.value.trim(),
				units: fUnits.input.value.trim(),
				scaledMin: fMin.input.value.trim(),
				scaledMax: fMax.input.value.trim(),
				mapEquation: fEq.input.value.trim(),
			});

			card._validate = () => {
				let ok = true;
				if (!fName.input.value.trim()) {
					fName.error.textContent = "Required";
					ok = false;
				} else fName.error.textContent = "";

				const bs = fBit.input.value.trim();
				if (!bs) {
					fBit.error.textContent = "Required";
					ok = false;
				} else if (!/^\d+(\s*-\s*\d+)?$/.test(bs)) {
					fBit.error.textContent = "Integer or range (e.g. 0-7)";
					ok = false;
				} else fBit.error.textContent = "";

				const units = fUnits.input.value.trim();
				if (units && (units.length > 32 || /[:\n\r]/.test(units))) {
					fUnits.error.textContent = "Invalid units format";
					ok = false;
				} else fUnits.error.textContent = "";

				const minRaw = fMin.input.value.trim();
				const maxRaw = fMax.input.value.trim();
				const minVal = minRaw ? fu.parseNumericText(minRaw) : null;
				const maxVal = maxRaw ? fu.parseNumericText(maxRaw) : null;
				if (minRaw && minVal === null) {
					fMin.error.textContent = "Must be numeric";
					ok = false;
				} else fMin.error.textContent = "";
				if (maxRaw && maxVal === null) {
					fMax.error.textContent = "Must be numeric";
					ok = false;
				} else fMax.error.textContent = "";
				if (minVal !== null && maxVal !== null && minVal > maxVal) {
					fMax.error.textContent = "Must be >= scaled min";
					ok = false;
				}

				const eq = fEq.input.value.trim();
				if (eq && !fu.isValidMapEquation(eq)) {
					fEq.error.textContent = "Invalid equation format";
					ok = false;
				} else fEq.error.textContent = "";
				return ok;
			};

			fieldsBox.appendChild(card);
		}

		if (existing && existing.fields.length > 0) {
			existing.fields.forEach((f) => addFieldRow(f));
		}
		addFieldBtn.addEventListener("click", () => addFieldRow(null));

		const cancelBtn = fu.makeBtn("Cancel");
		cancelBtn.addEventListener("click", () => fu.closeOverlay(overlay));
		const saveBtn = fu.makeBtn("Save", "editor-btn-primary");
		footer.appendChild(cancelBtn);
		footer.appendChild(saveBtn);

		saveBtn.addEventListener("click", () => {
			let ok = true;

			const name = nameF.input.value.trim();
			if (!name) {
				nameF.error.textContent = "Required";
				ok = false;
			} else nameF.error.textContent = "";

			const msgId = idF.input.value.trim();
			if (!msgId) {
				idF.error.textContent = "Required";
				ok = false;
			} else if (!/^0x[0-9a-fA-F]+$/.test(msgId)) {
				idF.error.textContent = "Hex format (e.g. 0x003)";
				ok = false;
			} else idF.error.textContent = "";

			const existingDefs = editor.getMessageIdEntries();
			const existingName = existingDefs.find(
				(e) => e.name === name && (isNewMsg || e.name !== msgName),
			);
			if (existingName) {
				nameF.error.textContent = "Message name already exists";
				ok = false;
			}
			const msgIdTakenBy = existingDefs.find(
				(e) =>
					e.msgId === msgId.toLowerCase() && (isNewMsg || e.name !== msgName),
			);
			if (msgIdTakenBy) {
				idF.error.textContent = "MSG ID already used by " + msgIdTakenBy.name;
				ok = false;
			}

			const msgLen = lenF.input.value.trim();
			if (!msgLen || parseInt(msgLen, 10) <= 0) {
				lenF.error.textContent = "Positive integer";
				ok = false;
			} else lenF.error.textContent = "";

			const cards = fieldsBox.querySelectorAll(".editor-field-card");
			cards.forEach((c) => {
				if (!c._validate()) ok = false;
			});
			if (!ok) return;

			const fields = [];
			cards.forEach((c) => fields.push(c._getValues()));
			lenF.error.textContent = "";
			const msgLenBits = parseInt(msgLen, 10) * 8;
			const spans = [];
			for (let i = 0; i < fields.length; i++) {
				const f = fields[i];
				const rawBit = String(f.bitStart || "").trim();
				const range = rawBit.match(/^(\d+)\s*-\s*(\d+)$/);
				const start = range ? parseInt(range[1], 10) : parseInt(rawBit, 10);
				const explicitEnd = range ? parseInt(range[2], 10) : null;
				const width = fu.typeBitWidth(f.dataType);
				if (!Number.isInteger(start) || start < 0) {
					lenF.error.textContent = "Invalid bit layout: negative/invalid start";
					ok = false;
					break;
				}
				let end;
				if (f.dataType === "s") {
					if (explicitEnd !== null) {
						lenF.error.textContent = "String field must use single bit_start";
						ok = false;
						break;
					}
					if (start % 8 !== 0) {
						lenF.error.textContent =
							"String field bit_start should be byte-aligned";
						ok = false;
						break;
					}
					end = msgLenBits - 1;
				} else {
					if (!width) {
						lenF.error.textContent = "Unsupported data type in field layout";
						ok = false;
						break;
					}
					if (explicitEnd !== null) {
						if (explicitEnd < start) {
							lenF.error.textContent = "Invalid bit range in field " + f.name;
							ok = false;
							break;
						}
						const spanWidth = explicitEnd - start + 1;
						if (spanWidth !== width) {
							lenF.error.textContent =
								"Field " +
								f.name +
								" bit range width must match data type width";
							ok = false;
							break;
						}
						end = explicitEnd;
					} else {
						end = start + width - 1;
					}
				}
				if (end >= msgLenBits) {
					lenF.error.textContent =
						"Field " + f.name + " exceeds MSG LENGTH (" + msgLen + " bytes)";
					ok = false;
					break;
				}
				spans.push({ start, end, name: f.name, type: f.dataType });
			}

			if (ok) {
				for (let i = 0; i < spans.length; i++) {
					for (let j = i + 1; j < spans.length; j++) {
						if (
							spans[i].type !== "s" &&
							spans[j].type !== "s" &&
							spans[i].start <= spans[j].end &&
							spans[j].start <= spans[i].end
						) {
							lenF.error.textContent =
								"Field overlap: " + spans[i].name + " and " + spans[j].name;
							ok = false;
							break;
						}
					}
					if (!ok) break;
				}
			}
			if (!ok) return;

			const def = { name, msgId, msgLength: msgLen, fields };

			if (!isNewMsg && msgName) {
				const result = window.GrcanDocument.updateMessageDef(msgName, def);
				if (!result.ok) {
					nameF.error.textContent = result.error;
					return;
				}
				editor.markEdited("msgDef:" + msgName);
				if (name !== msgName) editor.markEdited("msgDef:" + name);
				fu.closeOverlay(overlay, { force: true });
				editor.triggerReRender();
				return;
			} else {
				const result = window.GrcanDocument.addMessageDef(def);
				if (!result.ok) {
					nameF.error.textContent = result.error;
					return;
				}
				editor.markNew("msgDef:" + name);
			}
			fu.closeOverlay(overlay, { force: true });
			editor.triggerReRender();
		});
	}

	window.GrcanEditor.showMessageEditForm = showMessageEditForm;
})();
