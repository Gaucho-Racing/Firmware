window.GrcanGraphView = (() => {
	// ==================== Constants ====================

	const COLOR_PALETTE = [
		"#7c3aed",
		"#0ea5e9",
		"#10b981",
		"#f59e0b",
		"#ef4444",
		"#ec4899",
		"#14b8a6",
		"#f97316",
	];

	const BUS_LABELS = { CAN1: "Primary", CAN2: "Data", CAN3: "Charger" };

	// ==================== State ====================

	let cy = null;
	let currentBus = "CAN1";
	let overlayEl = null;
	let nodePanelEl = null;
	let focusPillEl = null;
	let _escHandler = null;
	let _focusedNodeId = null; // null = full graph view
	let _savedPositions = null; // node positions before entering focus
	let _currentGraphData = null;

	// ==================== Color Assignment ====================

	function _assignColors(nodes) {
		const sorted = nodes.map((n) => n.id).sort();
		const map = new Map();
		sorted.forEach((id, i) =>
			map.set(id, COLOR_PALETTE[i % COLOR_PALETTE.length]),
		);
		return map;
	}

	// ==================== Layout ====================

	function _overviewLayout(nodeCount) {
		if (nodeCount <= 6) {
			return {
				name: "cose",
				animate: true,
				animationDuration: 300,
				nodeRepulsion: 8000,
				edgeElasticity: 80,
				gravity: 0.4,
				numIter: 800,
				fit: true,
				padding: 48,
			};
		}
		return {
			name: "concentric",
			concentric: (node) => node.degree(),
			levelWidth: () => 2,
			minNodeSpacing: 80,
			fit: true,
			padding: 64,
			animate: true,
			animationDuration: 400,
		};
	}

	function _spokeLayout(centerId) {
		return {
			name: "concentric",
			concentric: (node) => (node.id() === centerId ? 100 : 1),
			levelWidth: () => 1,
			minNodeSpacing: 48,
			fit: true,
			padding: 80,
			animate: true,
			animationDuration: 280,
		};
	}

	// ==================== Ghosting (sends/receives distinction) ====================

	function _applyGhosting(nodeId) {
		cy.elements().removeClass("highlighted faded sends-edge receives-edge");
		const node = cy.getElementById(nodeId);
		node.addClass("highlighted");
		node.outgoers("edge").addClass("sends-edge");
		node.outgoers("edge").targets().addClass("highlighted");
		node.incomers("edge").addClass("receives-edge");
		node.incomers("edge").sources().addClass("highlighted");
		cy.elements(":visible")
			.not(".highlighted, .sends-edge, .receives-edge")
			.addClass("faded");
	}

	function _clearGhosting() {
		cy.elements().removeClass("highlighted faded sends-edge receives-edge");
	}

	// ==================== Focus Mode ====================

	function _enterFocus(nodeId) {
		_focusedNodeId = nodeId;

		// Save every node's current position for restore
		_savedPositions = {};
		cy.nodes().forEach((n) => {
			_savedPositions[n.id()] = { x: n.position("x"), y: n.position("y") };
		});

		const focusNode = cy.getElementById(nodeId);
		const neighborhood = focusNode.closedNeighborhood(); // node + neighbours + edges

		// Hide everything outside the neighbourhood
		cy.elements().not(neighborhood).style("display", "none");

		// Layout just the visible neighbourhood as a spoke
		cy.layout(_spokeLayout(nodeId)).run();

		// Apply sends/receives ghosting within the spoke
		_applyGhosting(nodeId);

		// Show focus pill
		_showFocusPill(nodeId);
	}

	function _exitFocus() {
		_focusedNodeId = null;

		// Restore all elements
		cy.elements().style("display", "element");
		_clearGhosting();

		// Restore saved positions then fit
		if (_savedPositions) {
			cy.batch(() => {
				cy.nodes().forEach((n) => {
					const pos = _savedPositions[n.id()];
					if (pos) n.position(pos);
				});
			});
			_savedPositions = null;
		}
		cy.fit(64);

		_hideFocusPill();
		_hideNodePanel();
	}

	// ==================== Focus Pill ====================

	function _showFocusPill(nodeId) {
		focusPillEl.querySelector(".graph-pill-label").textContent =
			`Focus: ${nodeId}`;
		focusPillEl.style.display = "flex";
	}

	function _hideFocusPill() {
		focusPillEl.style.display = "none";
	}

	// ==================== DOM Construction ====================

	function _buildOverlay() {
		const overlay = document.createElement("div");
		overlay.className = "graph-overlay";

		// Toolbar
		const toolbar = document.createElement("div");
		toolbar.className = "graph-toolbar";

		const backBtn = document.createElement("button");
		backBtn.className = "graph-back-btn";
		backBtn.textContent = "← Back";

		const title = document.createElement("span");
		title.className = "graph-title";
		title.textContent = "CAN Graph";

		const tabs = document.createElement("div");
		tabs.className = "graph-bus-tabs";
		["CAN1", "CAN2", "CAN3"].forEach((bus) => {
			const btn = document.createElement("button");
			btn.className = "graph-bus-tab";
			btn.dataset.bus = bus;
			btn.textContent = BUS_LABELS[bus];
			tabs.appendChild(btn);
		});

		// Focus pill (hidden until a node is selected)
		const pill = document.createElement("div");
		pill.className = "graph-focus-pill";
		pill.style.display = "none";

		const pillLabel = document.createElement("span");
		pillLabel.className = "graph-pill-label";

		const pillClose = document.createElement("button");
		pillClose.className = "graph-pill-close";
		pillClose.textContent = "×";
		pillClose.setAttribute("aria-label", "Exit focus");

		pill.appendChild(pillLabel);
		pill.appendChild(pillClose);

		const fitBtn = document.createElement("button");
		fitBtn.className = "graph-fit-btn";
		fitBtn.textContent = "Fit";

		toolbar.appendChild(backBtn);
		toolbar.appendChild(title);
		toolbar.appendChild(tabs);
		toolbar.appendChild(pill);
		toolbar.appendChild(fitBtn);

		// Canvas area
		const canvasArea = document.createElement("div");
		canvasArea.className = "graph-canvas-area";

		const cyContainer = document.createElement("div");
		cyContainer.id = "graph-cy-container";

		// Node detail panel
		const nodePanel = document.createElement("div");
		nodePanel.className = "graph-node-panel";

		canvasArea.appendChild(cyContainer);
		canvasArea.appendChild(nodePanel);

		overlay.appendChild(toolbar);
		overlay.appendChild(canvasArea);

		return overlay;
	}

	// ==================== Cytoscape Init ====================

	function _initCytoscape(containerEl) {
		cy = cytoscape({
			container: containerEl,
			elements: [],
			style: [
				// ── Default node ──────────────────────────────────────────────
				{
					selector: "node",
					style: {
						"background-color": "#13172a",
						"border-width": 1.5,
						"border-color": "#2c3350",
						label: "data(labelFull)",
						color: "#e2e8f0",
						"font-size": "12px",
						"font-family": "monospace",
						"text-valign": "center",
						"text-halign": "center",
						"text-wrap": "wrap",
						"text-max-width": "120px",
						width: "label",
						height: "label",
						padding: "14px",
						shape: "round-rectangle",
						"min-zoomed-font-size": 7,
					},
				},
				// Hub nodes — slightly larger
				{
					selector: "node[degree > 4]",
					style: {
						padding: "20px",
						"border-width": 2,
						"background-color": "#181d30",
					},
				},
				// Highlighted node (selected or neighbour)
				{
					selector: "node.highlighted",
					style: {
						"border-color": "#0ea5e9",
						"border-width": 3,
						opacity: 1,
					},
				},
				// Faded node
				{ selector: "node.faded", style: { opacity: 0.2 } },

				// ── Default edge — very transparent overview hint ─────────────
				// NOTE: Cytoscape ignores alpha in line-color strings.
				// Transparency must be set via the `opacity` property.
				{
					selector: "edge",
					style: {
						"curve-style": "bezier",
						"target-arrow-shape": "triangle",
						"arrow-scale": 1.0,
						"line-color": "#7090b0",
						"target-arrow-color": "#7090b0",
						opacity: 0.06, // ← actual transparency control
						width: "data(width)",
						"min-zoomed-font-size": 7,
					},
				},
				// SENDS edge: selected → receiver (sender color, solid, labeled)
				{
					selector: "edge.sends-edge",
					style: {
						"line-color": "data(color)",
						"target-arrow-color": "data(color)",
						"line-style": "solid",
						width: "data(highlightWidth)",
						label: "data(countLabel)",
						"font-size": "10px",
						color: "#e2e8f0",
						"text-background-color": "#07090f",
						"text-background-opacity": 0.92,
						"text-background-padding": "3px",
						"text-background-shape": "round-rectangle",
						"text-rotation": "autorotate",
						opacity: 1,
					},
				},
				// RECEIVES edge: sender → selected (gray dashed, labeled)
				{
					selector: "edge.receives-edge",
					style: {
						"line-color": "#475569",
						"target-arrow-color": "#475569",
						"line-style": "dashed",
						"line-dash-pattern": [6, 3],
						width: "data(highlightWidth)",
						label: "data(countLabel)",
						"font-size": "10px",
						color: "#94a3b8",
						"text-background-color": "#07090f",
						"text-background-opacity": 0.92,
						"text-background-padding": "3px",
						"text-background-shape": "round-rectangle",
						"text-rotation": "autorotate",
						opacity: 0.85,
					},
				},
				// Faded edge — nearly invisible
				{
					selector: "edge.faded",
					style: { opacity: 0.02 },
				},
			],
			userZoomingEnabled: true,
			userPanningEnabled: true,
			boxSelectionEnabled: false,
			minZoom: 0.1,
			maxZoom: 4,
		});
	}

	// ==================== Bus Loading ====================

	function _loadBus(busPort) {
		currentBus = busPort;
		_focusedNodeId = null;
		_savedPositions = null;

		overlayEl.querySelectorAll(".graph-bus-tab").forEach((btn) => {
			btn.classList.toggle("active", btn.dataset.bus === busPort);
		});
		overlayEl.querySelector(".graph-title").textContent =
			`CAN Graph — ${BUS_LABELS[busPort]}`;

		_hideFocusPill();
		_hideNodePanel();

		const doc = window.GrcanDocument;
		if (!doc) return;

		const { nodes, edges } = doc.getGraphDataForBus(busPort);
		_currentGraphData = { nodes, edges };

		const colorMap = _assignColors(nodes);

		// Degree map for node sizing
		const degreeMap = new Map();
		edges.forEach((e) => {
			degreeMap.set(e.source, (degreeMap.get(e.source) || 0) + 1);
			degreeMap.set(e.target, (degreeMap.get(e.target) || 0) + 1);
		});

		const cyNodes = nodes.map((n) => ({
			data: {
				id: n.id,
				label: n.id,
				labelFull: n.grId ? `${n.id}\n${n.grId}` : n.id,
				grId: n.grId,
				degree: degreeMap.get(n.id) || 0,
			},
		}));

		const cyEdges = edges.map((e) => {
			const w = Math.min(1.2 + e.count * 0.4, 4);
			return {
				data: {
					id: e.id,
					source: e.source,
					target: e.target,
					messages: e.messages,
					count: e.count,
					countLabel: e.count === 1 ? e.messages[0] : `${e.count} msgs`,
					color: colorMap.get(e.source) || "#94a3b8",
					width: w,
					highlightWidth: Math.min(w + 1.2, 5),
				},
			};
		});

		cy.elements().remove();
		if (cyNodes.length === 0) return;

		cy.add([...cyNodes, ...cyEdges]);
		cy.layout(_overviewLayout(cyNodes.length)).run();

		_wireEvents();
	}

	function _wireEvents() {
		cy.removeAllListeners();

		cy.on("tap", "node", (evt) => {
			const nodeId = evt.target.data("id");
			if (_focusedNodeId === nodeId) {
				_exitFocus();
			} else {
				_exitFocus(); // clean up any prior focus first
				_enterFocus(nodeId);
				_showNodePanel(nodeId);
			}
		});

		cy.on("tap", "edge", (evt) => {
			// Tapping an edge focuses the source node
			const sourceId = evt.target.data("source");
			if (_focusedNodeId === sourceId) {
				_exitFocus();
			} else {
				_exitFocus();
				_enterFocus(sourceId);
				_showNodePanel(sourceId);
			}
		});

		cy.on("tap", (evt) => {
			if (evt.target === cy) {
				_exitFocus();
			}
		});
	}

	// ==================== Node Panel ====================

	function _showNodePanel(nodeId) {
		if (!_currentGraphData) return;
		const { nodes, edges } = _currentGraphData;
		const nodeData = nodes.find((n) => n.id === nodeId);
		if (!nodeData) return;

		nodePanelEl.innerHTML = "";

		const title = document.createElement("div");
		title.className = "graph-panel-title";
		title.textContent = nodeData.id;
		nodePanelEl.appendChild(title);

		if (nodeData.grId) {
			const grIdEl = document.createElement("div");
			grIdEl.className = "graph-panel-grid";
			grIdEl.textContent = `GR ID: ${nodeData.grId}`;
			nodePanelEl.appendChild(grIdEl);
		}

		_appendPeerSection(
			nodePanelEl,
			"Sends",
			edges.filter((e) => e.source === nodeId),
			(e) => e.target,
			"→",
		);
		_appendPeerSection(
			nodePanelEl,
			"Receives",
			edges.filter((e) => e.target === nodeId),
			(e) => e.source,
			"←",
		);

		nodePanelEl.classList.add("open");
	}

	function _appendPeerSection(
		container,
		label,
		peerEdges,
		getPeerId,
		arrowChar,
	) {
		const divider = document.createElement("div");
		divider.className = "graph-panel-divider";
		container.appendChild(divider);

		const sectionLabel = document.createElement("div");
		sectionLabel.className = "graph-panel-section-label";
		sectionLabel.textContent = label;
		container.appendChild(sectionLabel);

		if (peerEdges.length === 0) {
			const empty = document.createElement("div");
			empty.className = "graph-panel-empty";
			empty.textContent =
				label === "Sends" ? "No outgoing messages" : "No incoming messages";
			container.appendChild(empty);
			return;
		}

		peerEdges.forEach((edge) => {
			const peerId = getPeerId(edge);

			const peer = document.createElement("button");
			peer.className = "graph-panel-peer";

			const arrow = document.createElement("span");
			arrow.className = "graph-panel-peer-arrow";
			arrow.textContent = arrowChar;

			const peerLabel = document.createElement("span");
			peerLabel.textContent = peerId;

			peer.appendChild(arrow);
			peer.appendChild(peerLabel);
			peer.addEventListener("click", () => {
				_exitFocus();
				_enterFocus(peerId);
				_showNodePanel(peerId);
			});
			container.appendChild(peer);

			const msgList = document.createElement("div");
			msgList.className = "graph-panel-msgs";
			edge.messages.forEach((m) => {
				const item = document.createElement("div");
				item.className = "graph-panel-msg";
				item.textContent = `• ${m}`;
				msgList.appendChild(item);
			});
			container.appendChild(msgList);
		});
	}

	function _hideNodePanel() {
		nodePanelEl.classList.remove("open");
	}

	// ==================== Open / Close ====================

	function open() {
		if (overlayEl) return;

		overlayEl = _buildOverlay();
		document.body.appendChild(overlayEl);

		nodePanelEl = overlayEl.querySelector(".graph-node-panel");
		focusPillEl = overlayEl.querySelector(".graph-focus-pill");
		const cyContainer = overlayEl.querySelector("#graph-cy-container");

		_initCytoscape(cyContainer);

		overlayEl.querySelectorAll(".graph-bus-tab").forEach((btn) => {
			btn.addEventListener("click", () => _loadBus(btn.dataset.bus));
		});

		overlayEl
			.querySelector(".graph-fit-btn")
			.addEventListener("click", () => cy.fit(64));
		overlayEl
			.querySelector(".graph-back-btn")
			.addEventListener("click", _close);
		overlayEl
			.querySelector(".graph-pill-close")
			.addEventListener("click", () => {
				_exitFocus();
			});

		_escHandler = (e) => {
			if (e.key === "Escape") {
				if (_focusedNodeId) _exitFocus();
				else _close();
			}
		};
		document.addEventListener("keydown", _escHandler);

		_loadBus("CAN1");
	}

	function _close() {
		if (!overlayEl) return;
		if (cy) {
			cy.destroy();
			cy = null;
		}
		overlayEl.remove();
		overlayEl = null;
		nodePanelEl = null;
		focusPillEl = null;
		_focusedNodeId = null;
		_savedPositions = null;
		_currentGraphData = null;
		if (_escHandler) {
			document.removeEventListener("keydown", _escHandler);
			_escHandler = null;
		}
	}

	// ==================== Init ====================

	const graphBtn = document.getElementById("graph-view-btn");
	if (graphBtn) graphBtn.addEventListener("click", open);

	return { open };
})();
