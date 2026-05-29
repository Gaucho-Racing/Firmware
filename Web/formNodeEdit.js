// Purpose: "Edit Node" modal form.
// Allows editing a routing device/node entry in-place: its name and its GR ID
// (hex node ID). Validates that the new name is non-empty and does not collide
// with an existing node, and that the GR ID is hex and not already used by
// another node. A no-op save (name and ID unchanged) closes without marking any
// change.
// Depends on: formUtils.js (FormUtils), editor.js (GrcanEditor), candoDocument.js.
// Registers: window.GrcanEditor.showRoutingNodeEditForm

(function () {
	"use strict";

	function showRoutingNodeEditForm(oldDeviceName) {
		const editor = window.GrcanEditor;
		const fu = window.FormUtils;
		const { overlay, body, footer } = fu.createModal("Edit Node");

		const oldGrId =
			(window.GrcanDocument && window.GrcanDocument.getGrId(oldDeviceName)) ||
			"";

		const nameF = fu.makeFormRow(
			"Node Name",
			fu.makeInput("text", oldDeviceName || "", "Node Name"),
			true,
		);
		const idF = fu.makeFormRow(
			"Node ID (GR ID)",
			fu.makeInput("text", oldGrId, "0x2B"),
			true,
		);
		body.appendChild(nameF.row);
		body.appendChild(idF.row);

		const cancelBtn = fu.makeBtn("Cancel");
		cancelBtn.addEventListener("click", () => fu.closeOverlay(overlay));
		const saveBtn = fu.makeBtn("Save", "editor-btn-primary");
		footer.appendChild(cancelBtn);
		footer.appendChild(saveBtn);

		saveBtn.addEventListener("click", () => {
			const newName = nameF.input.value.trim();
			const newId = idF.input.value.trim();
			let ok = true;

			if (!newName) {
				nameF.error.textContent = "Required";
				ok = false;
			} else if (
				newName !== oldDeviceName &&
				window.GrcanDocument.deviceExists(newName)
			) {
				nameF.error.textContent = "Node already exists";
				ok = false;
			} else {
				nameF.error.textContent = "";
			}

			if (!newId) {
				idF.error.textContent = "Required";
				ok = false;
			} else if (!/^0x[0-9a-fA-F]+$/.test(newId)) {
				idF.error.textContent = "Hex format (e.g. 0x2B)";
				ok = false;
			} else {
				idF.error.textContent = "";
			}
			if (!ok) return;

			const nameChanged = newName !== oldDeviceName;
			const idChanged = newId.toLowerCase() !== String(oldGrId).toLowerCase();

			if (!nameChanged && !idChanged) {
				fu.closeOverlay(overlay, { force: true });
				return;
			}

			if (nameChanged) {
				const result = window.GrcanDocument.renameDevice(
					oldDeviceName,
					newName,
				);
				if (!result.ok) {
					nameF.error.textContent = result.error;
					return;
				}
			}

			if (idChanged) {
				const result = window.GrcanDocument.updateGrId(newName, newId);
				if (!result.ok) {
					idF.error.textContent = result.error;
					return;
				}
			}

			editor.markEdited("routeNode:" + newName);
			fu.closeOverlay(overlay, { force: true });
			editor.triggerReRender();
		});
	}

	window.GrcanEditor.showRoutingNodeEditForm = showRoutingNodeEditForm;
})();
