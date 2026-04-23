// Purpose: "Edit Node" modal form.
// Allows renaming a routing device/node entry in-place. Validates that the new
// name is non-empty and does not collide with an existing node. A no-op save
// (same name) closes without marking any change.
// Depends on: formUtils.js (FormUtils), editor.js (GrcanEditor).
// Registers: window.GrcanEditor.showRoutingNodeEditForm

(function () {
	"use strict";

	function showRoutingNodeEditForm(oldDeviceName) {
		const editor = window.GrcanEditor;
		const fu = window.FormUtils;
		const { overlay, body, footer } = fu.createModal("Edit Node");

		const nameF = fu.makeFormRow(
			"Node Name",
			fu.makeInput("text", oldDeviceName || "", "Node Name"),
			true,
		);
		body.appendChild(nameF.row);

		const cancelBtn = fu.makeBtn("Cancel");
		cancelBtn.addEventListener("click", () => fu.closeOverlay(overlay));
		const saveBtn = fu.makeBtn("Save", "editor-btn-primary");
		footer.appendChild(cancelBtn);
		footer.appendChild(saveBtn);

		saveBtn.addEventListener("click", () => {
			const newName = nameF.input.value.trim();
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
			if (!ok) return;

			if (newName === oldDeviceName) {
				fu.closeOverlay(overlay, { force: true });
				return;
			}

			const result = window.GrcanDocument.renameDevice(oldDeviceName, newName);
			if (!result.ok) {
				nameF.error.textContent = result.error;
				return;
			}
			editor.markEdited("routeNode:" + newName);
			fu.closeOverlay(overlay, { force: true });
			editor.triggerReRender();
		});
	}

	window.GrcanEditor.showRoutingNodeEditForm = showRoutingNodeEditForm;
})();
