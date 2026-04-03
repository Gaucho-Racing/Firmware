// Purpose: "Add / Edit Custom CAN ID" modal form.
// Handles Custom CAN ID section entries: CAN ID (hex), Length, and signals
// (name, bit_start, optional comment). Uses GrcanDocument for mutations.
// Depends on: formUtils.js (FormUtils), editor.js (GrcanEditor), candoDocument.js.
// Registers: window.GrcanEditor.showCustomCanIdEditForm

(function () {
	"use strict";

	function showCustomCanIdEditForm(msgName, isNew) {
		const editor = window.GrcanEditor;
		const doc = window.GrcanDocument;
		const fu = window.FormUtils;

		const existing = isNew ? null : doc.getCustomCanIdDef(msgName);
		const { overlay, body, footer } = fu.createModal(
			isNew ? "Add Custom CAN ID" : "Edit: " + msgName,
		);

		const nameF = fu.makeFormRow(
			"Message Name",
			fu.makeInput("text", isNew ? "" : msgName),
			true,
		);
		const idF = fu.makeFormRow(
			"CAN ID",
			fu.makeInput("text", existing?.canId || "", "e.g. 116 or 18FF50E5"),
			true,
		);
		const lenF = fu.makeFormRow(
			"Length (bytes)",
			fu.makeInput("number", existing?.length || "", "8"),
			true,
		);
		body.appendChild(nameF.row);
		body.appendChild(idF.row);
		body.appendChild(lenF.row);

		// Signals section
		const sigHdr = document.createElement("div");
		sigHdr.className = "editor-section-header";
		const sigTitle = document.createElement("span");
		sigTitle.textContent = "Signals";
		sigHdr.appendChild(sigTitle);
		const addSigBtn = document.createElement("button");
		addSigBtn.className = "editor-btn editor-btn-sm";
		addSigBtn.innerHTML = fu.PLUS_SVG + " Add Signal";
		sigHdr.appendChild(addSigBtn);
		body.appendChild(sigHdr);

		const sigBox = document.createElement("div");
		sigBox.className = "editor-fields-box";
		body.appendChild(sigBox);

		function addSignalRow(sig) {
			const card = document.createElement("div");
			card.className = "editor-field-card";

			const topRow = document.createElement("div");
			topRow.className = "editor-field-grid";

			const fName = fu.makeFormRow(
				"Name",
				fu.makeInput("text", sig?.name || "", "Signal Name"),
				true,
			);
			const fBit = fu.makeFormRow(
				"bit_start",
				fu.makeInput("text", sig?.bitStart || "", "0 or 0-7"),
				true,
			);
			topRow.appendChild(fName.row);
			topRow.appendChild(fBit.row);
			card.appendChild(topRow);

			const commentRow = document.createElement("div");
			commentRow.className = "editor-field-grid";
			const fComment = fu.makeFormRow(
				"Comment",
				fu.makeInput("textarea", sig?.comment || "", "Description"),
			);
			commentRow.appendChild(fComment.row);
			card.appendChild(commentRow);

			const controlsRow = document.createElement("div");
			controlsRow.className = "editor-field-controls";
			const upBtn = fu.makeBtn("Move Up", "editor-btn-sm");
			const downBtn = fu.makeBtn("Move Down", "editor-btn-sm");
			const removeBtn = document.createElement("button");
			removeBtn.className = "editor-btn editor-btn-danger editor-btn-sm";
			removeBtn.innerHTML = fu.TRASH_SVG + " Remove";
			removeBtn.addEventListener("click", () => card.remove());
			upBtn.addEventListener("click", (e) => {
				e.preventDefault();
				const prev = card.previousElementSibling;
				if (prev) card.parentNode.insertBefore(card, prev);
			});
			downBtn.addEventListener("click", (e) => {
				e.preventDefault();
				const next = card.nextElementSibling;
				if (next) card.parentNode.insertBefore(next, card);
			});
			controlsRow.appendChild(upBtn);
			controlsRow.appendChild(downBtn);
			controlsRow.appendChild(removeBtn);
			card.appendChild(controlsRow);

			card._getValues = () => ({
				name: fName.input.value.trim(),
				bitStart: fBit.input.value.trim(),
				comment: fComment.input.value.trim() || null,
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

				return ok;
			};

			sigBox.appendChild(card);
		}

		if (existing && existing.signals.length > 0) {
			existing.signals.forEach((s) => addSignalRow(s));
		}
		addSigBtn.addEventListener("click", () => addSignalRow(null));

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

			const canId = idF.input.value.trim();
			if (!canId) {
				idF.error.textContent = "Required";
				ok = false;
			} else if (!/^[0-9a-fA-F]+$/.test(canId)) {
				idF.error.textContent = "Must be hex (e.g. 116 or 18FF50E5)";
				ok = false;
			} else idF.error.textContent = "";

			const length = lenF.input.value.trim();
			if (!length || parseInt(length, 10) < 0 || isNaN(parseInt(length, 10))) {
				lenF.error.textContent = "Non-negative integer";
				ok = false;
			} else lenF.error.textContent = "";

			const cards = sigBox.querySelectorAll(".editor-field-card");
			cards.forEach((c) => {
				if (!c._validate()) ok = false;
			});
			if (!ok) return;

			const signals = [];
			cards.forEach((c) => signals.push(c._getValues()));

			const def = { name, canId, length, signals };

			if (!isNew && msgName) {
				const result = doc.updateCustomCanIdDef(msgName, def);
				if (!result.ok) {
					nameF.error.textContent = result.error;
					return;
				}
				editor.markEdited("customCan:" + msgName);
				if (name !== msgName) editor.markEdited("customCan:" + name);
			} else {
				const result = doc.addCustomCanIdDef(def);
				if (!result.ok) {
					nameF.error.textContent = result.error;
					return;
				}
				editor.markNew("customCan:" + name);
			}
			fu.closeOverlay(overlay, { force: true });
			editor.triggerReRender();
		});
	}

	window.GrcanEditor.showCustomCanIdEditForm = showCustomCanIdEditForm;
})();
