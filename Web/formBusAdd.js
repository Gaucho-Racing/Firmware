// Purpose: "Add Bus" modal form.
// Allows creating a new bus block for an existing routing node without
// requiring receiver/message route details. The list of available buses is
// pulled from the "Bus ID:" section via GrcanDocument.getBusNames(),
// so adding a new bus to the CANdo file requires no changes here.
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

		// Filter the bus dropdown to only buses the device is physically wired to.
		// If the topology file isn't loaded yet, every declared bus is shown.
		const _allBuses = window.GrcanDocument.getBusNames();
		const _topo = window.PhysicalTopology;
		const _busChoices =
			deviceName && _topo && _topo.isLoaded()
				? _allBuses.filter((b) => _topo.isOnBus(deviceName, b))
				: _allBuses;
		const _effectiveChoices = _busChoices.length > 0 ? _busChoices : _allBuses;
		const busF = fu.makeFormRow(
			"Bus",
			fu.makeSelect(_effectiveChoices, _effectiveChoices[0] || ""),
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

			// Hard-block: device must be physically wired to the selected bus.
			const topo = window.PhysicalTopology;
			if (topo && topo.isLoaded() && !topo.isOnBus(deviceName, bus)) {
				busF.error.textContent = `"${deviceName}" is not physically wired to ${bus}`;
				return;
			}

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
