// Purpose: "Edit Bus" modal form.
// Allows changing the bus assigned to a routing bus block for a given device.
// The list of choices is sourced dynamically from the "Bus ID:" section via
// GrcanDocument.getBusNames(), so adding/renaming a bus there propagates
// automatically. Prevents renaming to a port that the device already has.
// A no-op save (same bus port) closes without marking any change.
// Depends on: formUtils.js (FormUtils), editor.js (GrcanEditor).
// Registers: window.GrcanEditor.showRoutingBusEditForm

(function () {
	"use strict";

	function showRoutingBusEditForm(deviceName, oldBusPort) {
		const editor = window.GrcanEditor;
		const fu = window.FormUtils;
		const { overlay, body, footer } = fu.createModal("Edit Bus");

		const _allBuses = window.GrcanDocument.getBusNames();
		const busF = fu.makeFormRow(
			"Bus",
			fu.makeSelect(_allBuses, oldBusPort || _allBuses[0] || ""),
			true,
		);
		body.appendChild(busF.row);

		const cancelBtn = fu.makeBtn("Cancel");
		cancelBtn.addEventListener("click", () => fu.closeOverlay(overlay));
		const saveBtn = fu.makeBtn("Save", "editor-btn-primary");
		footer.appendChild(cancelBtn);
		footer.appendChild(saveBtn);

		saveBtn.addEventListener("click", () => {
			const newBus = busF.input.value;
			const range = editor.findRoutingBusRange(deviceName, oldBusPort);
			if (!range) return;

			if (newBus === oldBusPort) {
				fu.closeOverlay(overlay, { force: true });
				return;
			}

			if (editor.findRoutingBusRange(deviceName, newBus)) {
				busF.error.textContent = "Bus already exists for this node";
				return;
			}
			busF.error.textContent = "";

			editor.replaceLineRange(
				range.startLine,
				range.startLine + 1,
				"      " + newBus + ":\n",
			);
			editor.markEdited("routeBus:" + deviceName + "|" + newBus);
			fu.closeOverlay(overlay, { force: true });
			editor.triggerReRender();
		});
	}

	window.GrcanEditor.showRoutingBusEditForm = showRoutingBusEditForm;
})();
