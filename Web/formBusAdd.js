// Purpose: "Add Bus" modal form.
// Allows creating a new bus block (CAN1/CAN2/CAN3) for an existing routing node
// without requiring receiver/message route details.
// Depends on: formUtils.js (FormUtils), editor.js (GrcanEditor).
// Registers: window.GrcanEditor.showRoutingBusAddForm

(function () {
	"use strict";

	function showRoutingBusAddForm(deviceName) {
		const editor = window.GrcanEditor;
		const fu = window.FormUtils;
		const { overlay, body, footer } = fu.createModal("Add Bus");

		const nodeF = fu.makeFormRow(
			"Node",
			fu.makeInput("text", deviceName || "", "Node Name"),
			true,
		);
		nodeF.input.disabled = true;
		body.appendChild(nodeF.row);

		const busF = fu.makeFormRow(
			"Bus",
			fu.makeSelect(["CAN1", "CAN2", "CAN3"], "CAN1"),
			true,
		);
		body.appendChild(busF.row);

		const cancelBtn = fu.makeBtn("Cancel");
		cancelBtn.addEventListener("click", () => fu.closeOverlay(overlay));
		const saveBtn = fu.makeBtn("Add", "editor-btn-primary");
		footer.appendChild(cancelBtn);
		footer.appendChild(saveBtn);

		saveBtn.addEventListener("click", () => {
			const bus = busF.input.value;
			const result = window.GrcanDocument.addBus(deviceName, bus);
			if (!result.ok) {
				busF.error.textContent = result.error;
				return;
			}
			busF.error.textContent = "";
			editor.markEdited("routeNode:" + deviceName);
			editor.markNew("routeBus:" + deviceName + "|" + bus);
			fu.closeOverlay(overlay, { force: true });
			editor.triggerReRender();
		});
	}

	window.GrcanEditor.showRoutingBusAddForm = showRoutingBusAddForm;
})();
