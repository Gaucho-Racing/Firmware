// Purpose: "Super Add" wizard for composing new nodes, message definition, and route
// in one flow. Each piece is optional so users can create only what they need.
// Depends on: formUtils.js (FormUtils), editor.js (GrcanEditor).
// Registers: window.GrcanEditor.showSuperAddForm

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

	function parseGrIdEntries(editor) {
		const lines = editor.getLines();
		const secStart = editor.findSectionStart(lines, "GR ID");
		if (secStart === -1) return [];
		const secEnd = editor.findSectionEnd(lines, secStart);
		const entries = [];
		for (let i = secStart + 1; i < secEnd; i++) {
			const line = lines[i];
			const m = line.match(/^\s+([^:]+):\s*["']?([^"'\s]+)["']?/);
			if (!m) continue;
			entries.push({ name: m[1].trim(), id: m[2].trim().toLowerCase() });
		}
		return entries;
	}

	function insertGrIdEntry(editor, name, nodeId) {
		const lines = editor.getLines();
		const secStart = editor.findSectionStart(lines, "GR ID");
		if (secStart === -1) return false;
		const secEnd = editor.findSectionEnd(lines, secStart);
		editor.insertAtLine(secEnd, "  " + name + ': "' + nodeId + '"\n');
		return true;
	}

	function appendRoute(editor, device, bus, receiver, msg, overrideId) {
		if (
			editor.routeEntryExists(device, bus, receiver, msg, overrideId || null)
		) {
			return { changed: false, createdNode: false, createdBus: false };
		}

		const lines = editor.getLines();
		const devRange = editor.findRoutingDeviceRange(device);
		let createdNode = false;
		let createdBus = false;

		if (!devRange) {
			createdNode = true;
			createdBus = true;
			const rStart = editor.findSectionStart(lines, "routing");
			if (rStart === -1)
				return { changed: false, createdNode: false, createdBus: false };
			const rEnd = editor.findSectionEnd(lines, rStart);
			editor.insertAtLine(
				rEnd,
				"    " +
					device +
					":\n      " +
					bus +
					":\n        " +
					receiver +
					":\n" +
					editor.generateRoutingMsgYaml(msg, overrideId || null),
			);
			return { changed: true, createdNode, createdBus };
		}

		const busRange = editor.findRoutingBusRange(device, bus);
		if (!busRange) {
			createdBus = true;
			editor.insertAtLine(
				devRange.endLine,
				"      " +
					bus +
					":\n        " +
					receiver +
					":\n" +
					editor.generateRoutingMsgYaml(msg, overrideId || null),
			);
			return { changed: true, createdNode, createdBus };
		}

		let recFound = false;
		const freshLines = editor.getLines();
		for (let i = busRange.startLine + 1; i < busRange.endLine; i++) {
			if (
				freshLines[i].search(/\S/) === 8 &&
				freshLines[i].trim() === receiver + ":"
			) {
				const recEnd = editor.findBlockEnd(freshLines, i, busRange.endLine, 8);
				editor.insertAtLine(
					recEnd,
					editor.generateRoutingMsgYaml(msg, overrideId || null),
				);
				recFound = true;
				break;
			}
		}
		if (!recFound) {
			editor.insertAtLine(
				busRange.endLine,
				"        " +
					receiver +
					":\n" +
					editor.generateRoutingMsgYaml(msg, overrideId || null),
			);
		}
		return { changed: true, createdNode, createdBus };
	}

	function showSuperAddForm() {
		const editor = window.GrcanEditor;
		const fu = window.FormUtils;
		const { overlay, body, footer } = fu.createModal("Super Add");

		const note = document.createElement("div");
		note.className = "editor-hint";
		note.textContent =
			"Optional blocks let you create only the pieces you need.";
		body.appendChild(note);

		const mkToggle = (label, checked) => {
			const row = document.createElement("div");
			row.className = "editor-form-row";
			const lbl = document.createElement("label");
			lbl.className = "editor-label";
			const cb = document.createElement("input");
			cb.type = "checkbox";
			cb.style.marginRight = "8px";
			cb.checked = !!checked;
			lbl.appendChild(cb);
			lbl.appendChild(document.createTextNode(label));
			row.appendChild(lbl);
			body.appendChild(row);
			return cb;
		};

		const doRoute = mkToggle("Add route (sender -> receiver on bus)", true);
		const addSenderNode = mkToggle("Create sender node in GR ID", false);
		const addReceiverNode = mkToggle("Create receiver node in GR ID", false);
		const addMessageDef = mkToggle("Create message definition", false);

		const senderF = fu.makeFormRow(
			"Sender Node",
			fu.makeInput("text", "", "e.g. Alice"),
			true,
		);
		const senderIdF = fu.makeFormRow(
			"Sender Node ID",
			fu.makeInput("text", "", "e.g. 0x31"),
		);
		const receiverF = fu.makeFormRow(
			"Receiver Node",
			fu.makeInput("text", "", "e.g. Bob"),
			true,
		);
		const receiverIdF = fu.makeFormRow(
			"Receiver Node ID",
			fu.makeInput("text", "", "e.g. 0x32"),
		);
		const _allBuses = window.GrcanDocument.getBusNames();
		const busF = fu.makeFormRow(
			"Bus",
			fu.makeSelect(_allBuses, _allBuses[0] || ""),
			true,
		);
		const msgNameF = fu.makeFormRow(
			"Message Name",
			fu.makeInput("text", "", "e.g. Alice Status"),
			true,
		);
		const ovrF = fu.makeFormRow(
			"CAN ID Override",
			fu.makeInput("text", "", "0x1806E5F4 (optional)"),
		);

		const msgDefHdr = document.createElement("div");
		msgDefHdr.className = "editor-section-header";
		const msgDefTitle = document.createElement("span");
		msgDefTitle.textContent = "New Message Definition";
		msgDefHdr.appendChild(msgDefTitle);

		const msgIdF = fu.makeFormRow("MSG ID", fu.makeInput("text", "", "0x123"));
		const msgLenF = fu.makeFormRow(
			"MSG LENGTH (bytes)",
			fu.makeInput("number", "8", "8"),
		);
		const fieldNameF = fu.makeFormRow(
			"Field Name",
			fu.makeInput("text", "value", "value"),
		);
		const fieldBitF = fu.makeFormRow(
			"Field bit_start",
			fu.makeInput("text", "0", "0 or 0-7"),
		);
		const fieldTypeF = fu.makeFormRow(
			"Field Data Type",
			fu.makeSelect(VALID_DATA_TYPES, "u8"),
		);

		body.appendChild(senderF.row);
		body.appendChild(senderIdF.row);
		body.appendChild(receiverF.row);
		body.appendChild(receiverIdF.row);
		body.appendChild(busF.row);
		body.appendChild(msgNameF.row);
		body.appendChild(ovrF.row);
		body.appendChild(msgDefHdr);
		body.appendChild(msgIdF.row);
		body.appendChild(msgLenF.row);
		body.appendChild(fieldNameF.row);
		body.appendChild(fieldBitF.row);
		body.appendChild(fieldTypeF.row);

		function syncVisibility() {
			const routeOn = !!doRoute.checked;
			senderF.row.style.display =
				routeOn || addSenderNode.checked ? "" : "none";
			receiverF.row.style.display =
				routeOn || addReceiverNode.checked ? "" : "none";
			msgNameF.row.style.display =
				routeOn || addMessageDef.checked ? "" : "none";
			busF.row.style.display = routeOn ? "" : "none";
			ovrF.row.style.display = routeOn ? "" : "none";
			senderIdF.row.style.display = addSenderNode.checked ? "" : "none";
			receiverIdF.row.style.display = addReceiverNode.checked ? "" : "none";

			const msgOn = addMessageDef.checked;
			msgDefHdr.style.display = msgOn ? "" : "none";
			msgIdF.row.style.display = msgOn ? "" : "none";
			msgLenF.row.style.display = msgOn ? "" : "none";
			fieldNameF.row.style.display = msgOn ? "" : "none";
			fieldBitF.row.style.display = msgOn ? "" : "none";
			fieldTypeF.row.style.display = msgOn ? "" : "none";
		}

		doRoute.addEventListener("change", syncVisibility);
		addSenderNode.addEventListener("change", syncVisibility);
		addReceiverNode.addEventListener("change", syncVisibility);
		addMessageDef.addEventListener("change", syncVisibility);
		syncVisibility();

		const cancelBtn = fu.makeBtn("Cancel");
		cancelBtn.addEventListener("click", () => fu.closeOverlay(overlay));
		const saveBtn = fu.makeBtn("Apply", "editor-btn-primary");
		footer.appendChild(cancelBtn);
		footer.appendChild(saveBtn);

		saveBtn.addEventListener("click", () => {
			let ok = true;
			const routeOn = !!doRoute.checked;
			const createSender = !!addSenderNode.checked;
			const createReceiver = !!addReceiverNode.checked;
			const createMsg = !!addMessageDef.checked;

			[
				senderF.error,
				senderIdF.error,
				receiverF.error,
				receiverIdF.error,
				msgNameF.error,
				ovrF.error,
				msgIdF.error,
				msgLenF.error,
				fieldNameF.error,
				fieldBitF.error,
			].forEach((el) => {
				el.textContent = "";
			});

			const sender = senderF.input.value.trim();
			const senderId = senderIdF.input.value.trim();
			const receiver = receiverF.input.value.trim();
			const receiverId = receiverIdF.input.value.trim();
			const bus = busF.input.value;
			const msgName = msgNameF.input.value.trim();
			const overrideId = ovrF.input.value.trim();
			const creatingMsg = createMsg;

			if (!routeOn && !createSender && !createReceiver && !createMsg) {
				senderF.error.textContent = "Select at least one action";
				return;
			}

			if ((routeOn || createSender) && !sender) {
				senderF.error.textContent = "Required";
				ok = false;
			}
			if ((routeOn || createReceiver) && !receiver) {
				receiverF.error.textContent = "Required";
				ok = false;
			}
			if ((routeOn || creatingMsg) && !msgName) {
				msgNameF.error.textContent = "Required";
				ok = false;
			}
			if (routeOn && overrideId && !/^0x[0-9a-fA-F]+$/.test(overrideId)) {
				ovrF.error.textContent = "Hex format";
				ok = false;
			}

			if (createSender && !/^0x[0-9a-fA-F]+$/.test(senderId)) {
				senderIdF.error.textContent = "Required hex (e.g. 0x31)";
				ok = false;
			}
			if (createReceiver && !/^0x[0-9a-fA-F]+$/.test(receiverId)) {
				receiverIdF.error.textContent = "Required hex (e.g. 0x32)";
				ok = false;
			}

			const grEntries = parseGrIdEntries(editor);
			const byName = new Map(grEntries.map((e) => [e.name, e.id]));
			const byId = new Map(grEntries.map((e) => [e.id, e.name]));

			if (createSender) {
				if (byName.has(sender)) {
					senderF.error.textContent = "Node already exists in GR ID";
					ok = false;
				}
				const existingForId = byId.get(senderId.toLowerCase());
				if (existingForId) {
					senderIdF.error.textContent = "ID already used by " + existingForId;
					ok = false;
				}
			}
			if (createReceiver) {
				if (byName.has(receiver)) {
					receiverF.error.textContent = "Node already exists in GR ID";
					ok = false;
				}
				const existingForId = byId.get(receiverId.toLowerCase());
				if (existingForId) {
					receiverIdF.error.textContent = "ID already used by " + existingForId;
					ok = false;
				}
				if (
					createSender &&
					sender === receiver &&
					senderId.toLowerCase() !== receiverId.toLowerCase()
				) {
					receiverF.error.textContent = "Same node name must use same ID";
					ok = false;
				}
			}

			const existingDefs = editor.getMessageIdEntries();
			if (creatingMsg) {
				const msgId = msgIdF.input.value.trim();
				const msgLen = msgLenF.input.value.trim();
				const fieldName = fieldNameF.input.value.trim();
				const fieldBit = fieldBitF.input.value.trim();
				const fieldType = fieldTypeF.input.value;

				if (!/^0x[0-9a-fA-F]+$/.test(msgId)) {
					msgIdF.error.textContent = "Required hex";
					ok = false;
				}
				if (!msgLen || parseInt(msgLen, 10) <= 0) {
					msgLenF.error.textContent = "Positive integer";
					ok = false;
				}
				if (!fieldName) {
					fieldNameF.error.textContent = "Required";
					ok = false;
				}
				if (!/^\d+(\s*-\s*\d+)?$/.test(fieldBit)) {
					fieldBitF.error.textContent = "Integer or range";
					ok = false;
				}

				const existingName = existingDefs.find((e) => e.name === msgName);
				if (existingName) {
					msgNameF.error.textContent = "Message name already exists";
					ok = false;
				}
				const existingId = existingDefs.find(
					(e) => e.msgId === msgId.toLowerCase(),
				);
				if (existingId) {
					msgIdF.error.textContent =
						"MSG ID already used by " + existingId.name;
					ok = false;
				}

				const msgLenBits = parseInt(msgLen, 10) * 8;
				if (ok) {
					const range = fieldBit.match(/^(\d+)\s*-\s*(\d+)$/);
					const start = range ? parseInt(range[1], 10) : parseInt(fieldBit, 10);
					const explicitEnd = range ? parseInt(range[2], 10) : null;
					const width = fu.typeBitWidth(fieldType);
					let end =
						explicitEnd !== null ? explicitEnd : start + (width || 1) - 1;
					if (fieldType === "s") {
						if (explicitEnd !== null || start % 8 !== 0) {
							fieldBitF.error.textContent =
								"String must be single, byte-aligned";
							ok = false;
						}
						end = msgLenBits - 1;
					} else if (!width) {
						fieldTypeF.parentNode.querySelector(".editor-error").textContent =
							"Unsupported type";
						ok = false;
					} else if (explicitEnd !== null && end - start + 1 !== width) {
						fieldBitF.error.textContent = "Range width must match type bits";
						ok = false;
					}
					if (end >= msgLenBits || start < 0) {
						fieldBitF.error.textContent = "Field exceeds MSG LENGTH";
						ok = false;
					}
				}
			} else if (routeOn && !editor.messageNameExists(msgName)) {
				msgNameF.error.textContent =
					"Must exist in Message ID or Custom CAN ID";
				ok = false;
			}

			if (!ok) return;

			let changed = false;
			const doc = window.GrcanDocument;

			if (createSender && !doc.deviceExists(sender)) {
				const r = doc.addDevice(sender, senderId);
				if (!r.ok) {
					senderF.error.textContent = r.error;
					return;
				}
				editor.markNew("routeNode:" + sender);
				changed = true;
			}
			if (
				createReceiver &&
				receiver !== sender &&
				!doc.deviceExists(receiver)
			) {
				const r = doc.addDevice(receiver, receiverId);
				if (!r.ok) {
					receiverF.error.textContent = r.error;
					return;
				}
				editor.markNew("routeNode:" + receiver);
				changed = true;
			}

			if (creatingMsg) {
				const msgDef = {
					name: msgName,
					msgId: msgIdF.input.value.trim(),
					msgLength: msgLenF.input.value.trim(),
					fields: [
						{
							name: fieldNameF.input.value.trim(),
							bitStart: fieldBitF.input.value.trim(),
							dataType: fieldTypeF.input.value,
							comment: "",
							units: "",
							scaledMin: "",
							scaledMax: "",
							mapEquation: "",
						},
					],
				};
				const r = doc.addMessageDef(msgDef);
				if (!r.ok) {
					msgNameF.error.textContent = r.error;
					return;
				}
				editor.markNew("msgDef:" + msgName);
				changed = true;
			}

			if (routeOn) {
				const r = doc.addRoute(
					sender,
					bus,
					receiver,
					msgName,
					overrideId || null,
				);
				if (!r.ok) {
					msgNameF.error.textContent = r.error;
					return;
				}
				if (!doc.deviceExists(sender)) editor.markNew("routeNode:" + sender);
				else editor.markEdited("routeNode:" + sender);
				editor.markNew("routeBus:" + sender + "|" + bus);
				editor.markNew("routeMsg:" + sender + "|" + bus + "|" + msgName);
				changed = true;
			}

			fu.closeOverlay(overlay, { force: true });
			if (changed) editor.triggerReRender();
		});
	}

	window.GrcanEditor.showSuperAddForm = showSuperAddForm;
})();
