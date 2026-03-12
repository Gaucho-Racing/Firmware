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

		const msgF = fu.makeFormRow(
			"Message",
			fu.makeInput("text", "", "Message Name"),
			true,
		);
		body.appendChild(msgF.row);

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
