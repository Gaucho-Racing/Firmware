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

	const SVG_NS = "http://www.w3.org/2000/svg";
	const ZOOM_MIN = 0.4;
	const ZOOM_MAX = 3.0;

	// ==================== State ====================

	let currentBus = null;
	// Bus list active for the current overlay session: [{ name, label }, ...].
	// Populated in open() from GrcanDocument.getBusNames(); read by the tab
	// builder and _loadBus to render labels / titles.
	let _busList = [];

	function _normalizeBusList(busList) {
		return (busList || [])
			.map((bus) => {
				if (typeof bus === "string") return { name: bus, label: bus };
				if (!bus || bus.name == null) return null;
				const name = String(bus.name);
				return {
					name,
					label: bus.label == null ? name : String(bus.label),
				};
			})
			.filter((bus) => bus && bus.name);
	}

	function _busLabel(busName) {
		const entry = _busList.find((b) => b.name === busName);
		return (entry && entry.label) || busName;
	}
	let overlayEl = null;
	let svgEl = null;
	let viewportG = null;
	let baseLayerG = null;
	let overlayLayerG = null;
	let nodePanelEl = null;
	let focusPillEl = null;
	let resetBtnEl = null;
	let _escHandler = null;
	let _focusedNodeId = null;
	let _currentGraphData = null;
	let _currentLayout = null;
	let _colorMap = null;
	const _nodeEls = new Map();
	let _pan = { x: 0, y: 0 };
	let _zoom = 1;
	let _panState = null;
	let _pinchState = null;
	let _resizeObserver = null;

	// ==================== SVG helpers ====================

	function _el(tag, attrs, children) {
		const el = document.createElementNS(SVG_NS, tag);
		if (attrs) {
			for (const k in attrs) {
				const v = attrs[k];
				if (v == null) continue;
				el.setAttribute(k, v);
			}
		}
		if (children) for (const c of children) el.appendChild(c);
		return el;
	}

	function _text(value, attrs) {
		const t = _el("text", attrs);
		t.textContent = value;
		return t;
	}

	// ==================== Color assignment ====================

	function _assignColors(nodes) {
		const sorted = nodes.map((n) => n.id).sort();
		const map = new Map();
		sorted.forEach((id, i) =>
			map.set(id, COLOR_PALETTE[i % COLOR_PALETTE.length]),
		);
		return map;
	}

	// ==================== Geometry helpers ====================

	function _cardAnchor(card, targetX, targetY) {
		const cx = card.x + card.w / 2;
		const cy = card.y + card.h / 2;
		const dx = targetX - cx;
		const dy = targetY - cy;
		if (dx === 0 && dy === 0) return { x: cx, y: cy };
		const hw = card.w / 2;
		const hh = card.h / 2;
		const tHoriz = Math.abs(dx) > 0 ? hw / Math.abs(dx) : Infinity;
		const tVert = Math.abs(dy) > 0 ? hh / Math.abs(dy) : Infinity;
		const t = Math.min(tHoriz, tVert);
		return { x: cx + dx * t, y: cy + dy * t };
	}

	function _bezierPath(x1, y1, x2, y2, bend) {
		const mx = (x1 + x2) / 2;
		const my = (y1 + y2) / 2;
		const dx = x2 - x1;
		const dy = y2 - y1;
		const len = Math.hypot(dx, dy) || 1;
		const nx = -dy / len;
		const ny = dx / len;
		const cx = mx + nx * bend;
		const cy = my + ny * bend;
		return `M ${x1.toFixed(1)} ${y1.toFixed(1)} Q ${cx.toFixed(1)} ${cy.toFixed(1)} ${x2.toFixed(1)} ${y2.toFixed(1)}`;
	}

	// ==================== Pan / zoom ====================

	function _applyViewport() {
		if (viewportG) {
			viewportG.setAttribute(
				"transform",
				`translate(${_pan.x} ${_pan.y}) scale(${_zoom})`,
			);
		}
	}

	function _resetViewport() {
		_pan = { x: 0, y: 0 };
		_zoom = 1;
		_applyViewport();
	}

	function _onWheel(evt) {
		evt.preventDefault();
		const rect = svgEl.getBoundingClientRect();
		const mx = evt.clientX - rect.left;
		const my = evt.clientY - rect.top;
		const factor = Math.exp(-evt.deltaY * 0.0015);
		const newZoom = Math.max(ZOOM_MIN, Math.min(ZOOM_MAX, _zoom * factor));
		const applied = newZoom / _zoom;
		_pan.x = mx - (mx - _pan.x) * applied;
		_pan.y = my - (my - _pan.y) * applied;
		_zoom = newZoom;
		_applyViewport();
	}

	function _onMouseDown(evt) {
		if (evt.button !== 0) return;
		if (evt.target.closest(".gv-node")) return;
		_panState = {
			sx: evt.clientX,
			sy: evt.clientY,
			px: _pan.x,
			py: _pan.y,
			moved: false,
		};
		svgEl.classList.add("grabbing");
	}

	function _onMouseMove(evt) {
		if (!_panState) return;
		const dx = evt.clientX - _panState.sx;
		const dy = evt.clientY - _panState.sy;
		if (Math.hypot(dx, dy) > 3) _panState.moved = true;
		_pan.x = _panState.px + dx;
		_pan.y = _panState.py + dy;
		_applyViewport();
	}

	function _onMouseUp() {
		if (!_panState) return;
		const moved = _panState.moved;
		_panState = null;
		if (svgEl) svgEl.classList.remove("grabbing");
		if (!moved && _focusedNodeId) _exitFocus();
	}

	// ==================== Touch pan / pinch-zoom ====================

	function _onTouchStart(evt) {
		if (evt.touches.length === 1) {
			_pinchState = null;
			_panState = {
				sx: evt.touches[0].clientX,
				sy: evt.touches[0].clientY,
				px: _pan.x,
				py: _pan.y,
				moved: false,
			};
		} else if (evt.touches.length === 2) {
			_panState = null;
			const dx = evt.touches[1].clientX - evt.touches[0].clientX;
			const dy = evt.touches[1].clientY - evt.touches[0].clientY;
			_pinchState = {
				dist: Math.hypot(dx, dy),
				zoom: _zoom,
				cx: (evt.touches[0].clientX + evt.touches[1].clientX) / 2,
				cy: (evt.touches[0].clientY + evt.touches[1].clientY) / 2,
				px: _pan.x,
				py: _pan.y,
			};
		}
		evt.preventDefault();
	}

	function _onTouchMove(evt) {
		evt.preventDefault();
		if (evt.touches.length === 1 && _panState) {
			const dx = evt.touches[0].clientX - _panState.sx;
			const dy = evt.touches[0].clientY - _panState.sy;
			if (Math.hypot(dx, dy) > 3) _panState.moved = true;
			_pan.x = _panState.px + dx;
			_pan.y = _panState.py + dy;
			_applyViewport();
		} else if (evt.touches.length === 2 && _pinchState) {
			const dx = evt.touches[1].clientX - evt.touches[0].clientX;
			const dy = evt.touches[1].clientY - evt.touches[0].clientY;
			const dist = Math.hypot(dx, dy);
			const factor = dist / _pinchState.dist;
			const newZoom = Math.max(
				ZOOM_MIN,
				Math.min(ZOOM_MAX, _pinchState.zoom * factor),
			);
			const rect = svgEl.getBoundingClientRect();
			const mx = _pinchState.cx - rect.left;
			const my = _pinchState.cy - rect.top;
			const applied = newZoom / _pinchState.zoom;
			_pan.x = mx - (mx - _pinchState.px) * applied;
			_pan.y = my - (my - _pinchState.py) * applied;
			_zoom = newZoom;
			_applyViewport();
		}
	}

	function _onTouchEnd(evt) {
		if (evt.touches.length === 0) {
			if (_panState && !_panState.moved && _focusedNodeId) _exitFocus();
			_panState = null;
			_pinchState = null;
		} else if (evt.touches.length < 2) {
			_pinchState = null;
		}
	}

	// ==================== Focus pill ====================

	function _showFocusPill(nodeId) {
		focusPillEl.querySelector(".graph-pill-label").textContent =
			`Focus: ${nodeId}`;
		focusPillEl.style.display = "flex";
	}

	function _hideFocusPill() {
		if (focusPillEl) focusPillEl.style.display = "none";
	}

	// ==================== DOM construction (toolbar + shell) ====================

	function _buildOverlay() {
		const overlay = document.createElement("div");
		overlay.className = "graph-overlay";

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
		_busList.forEach((bus) => {
			const btn = document.createElement("button");
			btn.className = "graph-bus-tab";
			btn.dataset.bus = bus.name;
			btn.textContent = bus.label || bus.name;
			tabs.appendChild(btn);
		});

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

		const resetBtn = document.createElement("button");
		resetBtn.className = "graph-fit-btn";
		resetBtn.textContent = "Reset";

		toolbar.appendChild(backBtn);
		toolbar.appendChild(title);
		toolbar.appendChild(tabs);
		toolbar.appendChild(pill);
		toolbar.appendChild(resetBtn);

		const canvasArea = document.createElement("div");
		canvasArea.className = "graph-canvas-area";

		const svgHost = document.createElement("div");
		svgHost.id = "graph-svg-host";

		const nodePanel = document.createElement("div");
		nodePanel.className = "graph-node-panel";

		canvasArea.appendChild(svgHost);
		canvasArea.appendChild(nodePanel);

		overlay.appendChild(toolbar);
		overlay.appendChild(canvasArea);

		return overlay;
	}

	// ==================== SVG init ====================

	function _initSvg(hostEl) {
		svgEl = _el("svg", {
			class: "gv-svg",
			preserveAspectRatio: "xMidYMid meet",
		});
		svgEl.style.width = "100%";
		svgEl.style.height = "100%";

		const defs = _el("defs");
		defs.appendChild(
			_el(
				"marker",
				{
					id: "gv-arrowhead",
					viewBox: "0 0 10 10",
					refX: 9,
					refY: 5,
					markerWidth: 6,
					markerHeight: 6,
					orient: "auto-start-reverse",
				},
				[
					_el("path", {
						d: "M 0 0 L 10 5 L 0 10 z",
						fill: "currentColor",
					}),
				],
			),
		);
		svgEl.appendChild(defs);

		viewportG = _el("g", { class: "graph-viewport" });
		baseLayerG = _el("g", { class: "graph-base-layer" });
		overlayLayerG = _el("g", { class: "graph-overlay-layer" });
		viewportG.appendChild(baseLayerG);
		viewportG.appendChild(overlayLayerG);
		svgEl.appendChild(viewportG);
		hostEl.appendChild(svgEl);

		svgEl.addEventListener("wheel", _onWheel, { passive: false });
		svgEl.addEventListener("mousedown", _onMouseDown);
		window.addEventListener("mousemove", _onMouseMove);
		window.addEventListener("mouseup", _onMouseUp);
		svgEl.addEventListener("touchstart", _onTouchStart, { passive: false });
		svgEl.addEventListener("touchmove", _onTouchMove, { passive: false });
		svgEl.addEventListener("touchend", _onTouchEnd, { passive: false });

		_resizeObserver = new ResizeObserver(() => {
			if (_currentGraphData) _rerender();
		});
		_resizeObserver.observe(hostEl);
	}

	function _clearSvg() {
		if (baseLayerG) baseLayerG.innerHTML = "";
		if (overlayLayerG) overlayLayerG.innerHTML = "";
		_nodeEls.clear();
	}

	// ==================== Render ====================

	function _renderLayout(layout, nodesData) {
		_clearSvg();
		svgEl.setAttribute(
			"viewBox",
			`0 0 ${layout.logicalWidth} ${layout.logicalHeight}`,
		);

		const bus = layout.busGeometry;

		// Single bus rail + terminators + label.
		const busY = bus.yCenter != null ? bus.yCenter : (bus.yHigh + bus.yLow) / 2;
		baseLayerG.appendChild(
			_el("line", {
				class: "gv-bus-rail",
				x1: bus.x1,
				y1: busY,
				x2: bus.x2,
				y2: busY,
			}),
		);
		baseLayerG.appendChild(
			_el("line", {
				class: "gv-bus-terminator",
				x1: bus.x1,
				y1: busY - 10,
				x2: bus.x1,
				y2: busY + 10,
			}),
		);
		baseLayerG.appendChild(
			_el("line", {
				class: "gv-bus-terminator",
				x1: bus.x2,
				y1: busY - 10,
				x2: bus.x2,
				y2: busY + 10,
			}),
		);
		baseLayerG.appendChild(
			_text(`${_busLabel(currentBus).toUpperCase()} CAN BUS`, {
				class: "gv-bus-speed",
				x: bus.x2 - 8,
				y: busY - 14,
				"text-anchor": "end",
			}),
		);

		// Stubs + drop circles.
		for (const s of layout.stubs) {
			baseLayerG.appendChild(
				_el("line", {
					class: "gv-stub",
					x1: s.x,
					y1: s.y1,
					x2: s.x,
					y2: s.y2,
				}),
			);
			baseLayerG.appendChild(
				_el("circle", {
					class: "gv-stub-drop",
					cx: s.x,
					cy: busY,
					r: 3,
				}),
			);
		}

		// Nodes.
		const nodesById = new Map(nodesData.map((n) => [n.id, n]));
		const onBusIds = new Set(layout.busNodes.map((n) => n.id));
		for (const [id, pos] of layout.nodePositions) {
			const nd = nodesById.get(id) || { id, grId: null };
			const isBus = onBusIds.has(id);
			const g = _el("g", {
				class: `gv-node${isBus ? " gv-node-bus" : ""}`,
				"data-id": id,
				transform: `translate(${pos.x} ${pos.y})`,
			});
			g.appendChild(
				_el("rect", {
					class: "gv-node-rect",
					width: pos.w,
					height: pos.h,
					rx: 8,
					ry: 8,
				}),
			);
			if (nd.grId) {
				g.appendChild(
					_text(id, {
						class: "gv-node-label",
						x: pos.w / 2,
						y: 22,
						"text-anchor": "middle",
					}),
				);
				g.appendChild(
					_text(nd.grId, {
						class: "gv-node-grid",
						x: pos.w / 2,
						y: 40,
						"text-anchor": "middle",
					}),
				);
			} else {
				g.appendChild(
					_text(id, {
						class: "gv-node-label",
						x: pos.w / 2,
						y: pos.h / 2 + 4,
						"text-anchor": "middle",
					}),
				);
			}
			g.addEventListener("click", (evt) => {
				evt.stopPropagation();
				_onNodeClick(id);
			});
			baseLayerG.appendChild(g);
			_nodeEls.set(id, g);
		}
	}

	function _rerender() {
		if (!_currentGraphData) return;
		const { nodes } = _currentGraphData;
		const presentIds = nodes.map((n) => n.id);
		const groupsApi = window.PhysicalGroups;
		let partition;
		if (groupsApi) {
			partition = groupsApi.getGroupsForBus(currentBus, presentIds);
		} else {
			// Fallback: everything into one "Other" group; Debugger/ALL on spine.
			const spine = ["Debugger", "ALL"].filter((s) => presentIds.includes(s));
			const rest = presentIds.filter((n) => !spine.includes(n));
			partition = {
				top: [],
				bottom: rest.length ? [{ name: "Other", nodes: rest }] : [],
				bus: spine,
			};
		}
		const hostEl = svgEl.parentElement;
		const rect = hostEl.getBoundingClientRect();
		const vw = Math.max(320, rect.width);
		const vh = Math.max(320, rect.height);
		const layout = window.LayoutPhysicalBus.layout(partition, vw, vh);
		_currentLayout = layout;
		_renderLayout(layout, nodes);

		// If we were focused, re-apply the focus state against the new layout.
		if (_focusedNodeId) {
			const id = _focusedNodeId;
			_focusedNodeId = null;
			_enterFocus(id);
		}
	}

	// ==================== Focus mode ====================

	function _onNodeClick(id) {
		if (_focusedNodeId === id) {
			_exitFocus();
		} else {
			_exitFocus();
			_enterFocus(id);
		}
	}

	function _enterFocus(id) {
		_focusedNodeId = id;
		svgEl.classList.add("gv-focused");

		const related = new Set([id]);
		for (const e of _currentGraphData.edges) {
			if (e.source === id) related.add(e.target);
			if (e.target === id) related.add(e.source);
		}
		for (const [nid, g] of _nodeEls) {
			g.classList.remove("gv-node-active", "gv-node-dim", "gv-node-focused");
			if (nid === id) {
				g.classList.add("gv-node-focused", "gv-node-active");
			} else if (related.has(nid)) {
				g.classList.add("gv-node-active");
			} else {
				g.classList.add("gv-node-dim");
			}
		}

		_renderEdges(id);
		_showFocusPill(id);
		_showNodePanel(id);
	}

	function _exitFocus() {
		_focusedNodeId = null;
		if (svgEl) svgEl.classList.remove("gv-focused");
		for (const g of _nodeEls.values()) {
			g.classList.remove("gv-node-active", "gv-node-dim", "gv-node-focused");
		}
		if (overlayLayerG) overlayLayerG.innerHTML = "";
		_hideFocusPill();
		_hideNodePanel();
	}

	function _renderEdges(id) {
		overlayLayerG.innerHTML = "";
		const color = _colorMap.get(id) || "#7dd3fc";
		for (const e of _currentGraphData.edges) {
			if (e.source !== id && e.target !== id) continue;
			const isSend = e.source === id;
			const sPos = _currentLayout.nodePositions.get(e.source);
			const tPos = _currentLayout.nodePositions.get(e.target);
			if (!sPos || !tPos) continue;

			const sC = { x: sPos.x + sPos.w / 2, y: sPos.y + sPos.h / 2 };
			const tC = { x: tPos.x + tPos.w / 2, y: tPos.y + tPos.h / 2 };
			const sA = _cardAnchor(sPos, tC.x, tC.y);
			const tA = _cardAnchor(tPos, sC.x, sC.y);
			const len = Math.hypot(tC.x - sC.x, tC.y - sC.y);
			const bendMag = Math.min(120, Math.max(40, len * 0.18));
			const bend = isSend ? bendMag : -bendMag;
			const d = _bezierPath(sA.x, sA.y, tA.x, tA.y, bend);
			const stroke = isSend ? color : "#7b8aa8";

			const path = _el("path", {
				d,
				fill: "none",
				stroke,
				"stroke-width": 2,
				"stroke-linecap": "round",
				"stroke-dasharray": isSend ? null : "6 4",
				"marker-end": "url(#gv-arrowhead)",
				class: isSend ? "gv-edge gv-edge-send" : "gv-edge gv-edge-receive",
			});
			path.style.color = stroke;
			overlayLayerG.appendChild(path);

			const mx = (sA.x + tA.x) / 2;
			const my = (sA.y + tA.y) / 2;
			const dx = tA.x - sA.x;
			const dy = tA.y - sA.y;
			const len2 = Math.hypot(dx, dy) || 1;
			const nx = -dy / len2;
			const ny = dx / len2;
			// Sit label at the curve's peak — quadratic bezier reaches
			// max offset of bend/2 at t=0.5 — then nudge a bit further
			// so it clears the endpoints instead of hugging them.
			const lx = mx + nx * bend * 0.6;
			const ly = my + ny * bend * 0.6;
			const labelText = `${e.count} ${e.count === 1 ? "msg" : "msgs"}`;
			const t = _text(labelText, {
				class: "gv-edge-label",
				x: lx.toFixed(1),
				y: ly.toFixed(1),
				"text-anchor": "middle",
				"dominant-baseline": "middle",
			});
			t.style.paintOrder = "stroke";
			t.style.stroke = "#07090f";
			t.style.strokeWidth = "4";
			t.style.strokeLinejoin = "round";
			t.style.fill = stroke;
			overlayLayerG.appendChild(t);
		}
	}

	// ==================== Bus load ====================

	function _loadBus(busPort) {
		currentBus = busPort;
		_focusedNodeId = null;

		overlayEl.querySelectorAll(".graph-bus-tab").forEach((btn) => {
			btn.classList.toggle("active", btn.dataset.bus === busPort);
		});
		overlayEl.querySelector(".graph-title").textContent =
			`CAN Graph — ${_busLabel(busPort)}`;

		_hideFocusPill();
		_hideNodePanel();

		const doc = window.GrcanDocument;
		if (!doc) return;

		const { nodes, edges } = doc.getGraphDataForBus(busPort);
		_currentGraphData = { nodes, edges };
		_colorMap = _assignColors(nodes);

		if (nodes.length === 0) {
			_clearSvg();
			svgEl.setAttribute("viewBox", `0 0 400 200`);
			const msg = _text("No routing defined for this bus.", {
				x: 200,
				y: 100,
				"text-anchor": "middle",
				class: "gv-empty",
			});
			baseLayerG.appendChild(msg);
			return;
		}
		_rerender();
	}

	// ==================== Node detail panel ====================

	function _showNodePanel(nodeId) {
		if (!_currentGraphData) return;
		const { nodes, edges } = _currentGraphData;
		const nd = nodes.find((n) => n.id === nodeId);
		if (!nd) return;

		nodePanelEl.innerHTML = "";

		const title = document.createElement("div");
		title.className = "graph-panel-title";
		title.textContent = nd.id;
		nodePanelEl.appendChild(title);

		if (nd.grId) {
			const gr = document.createElement("div");
			gr.className = "graph-panel-grid";
			gr.textContent = `GR ID: ${nd.grId}`;
			nodePanelEl.appendChild(gr);
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
		if (nodePanelEl) nodePanelEl.classList.remove("open");
	}

	// ==================== Open / close ====================

	function open(busList) {
		if (overlayEl) return;

		// Resolve the bus list before building the overlay, since the tab
		// builder reads from _busList. Fall back to the buses declared in
		// GRCAN.CANdo when no explicit list is provided.
		if (Array.isArray(busList) && busList.length) {
			_busList = _normalizeBusList(busList);
		} else if (
			window.GrcanDocument &&
			typeof window.GrcanDocument.getBusNames === "function"
		) {
			_busList = _normalizeBusList(window.GrcanDocument.getBusNames());
		} else {
			_busList = [];
		}

		if (!_busList.length) {
			console.warn(
				"GrcanGraphView.open(): no buses available. Is GRCAN.CANdo loaded?",
			);
			return;
		}

		overlayEl = _buildOverlay();
		document.body.appendChild(overlayEl);

		nodePanelEl = overlayEl.querySelector(".graph-node-panel");
		focusPillEl = overlayEl.querySelector(".graph-focus-pill");
		resetBtnEl = overlayEl.querySelector(".graph-fit-btn");
		const svgHost = overlayEl.querySelector("#graph-svg-host");

		_initSvg(svgHost);

		overlayEl.querySelectorAll(".graph-bus-tab").forEach((btn) => {
			btn.addEventListener("click", () => _loadBus(btn.dataset.bus));
		});
		resetBtnEl.addEventListener("click", () => {
			_resetViewport();
			_exitFocus();
		});
		overlayEl
			.querySelector(".graph-back-btn")
			.addEventListener("click", _close);
		overlayEl
			.querySelector(".graph-pill-close")
			.addEventListener("click", () => _exitFocus());

		_escHandler = (e) => {
			if (e.key === "Escape") {
				if (_focusedNodeId) _exitFocus();
				else _close();
			}
		};
		document.addEventListener("keydown", _escHandler);

		_loadBus(_busList[0].name);
	}

	function _close() {
		if (!overlayEl) return;
		if (_resizeObserver) {
			_resizeObserver.disconnect();
			_resizeObserver = null;
		}
		window.removeEventListener("mousemove", _onMouseMove);
		window.removeEventListener("mouseup", _onMouseUp);
		overlayEl.remove();
		overlayEl = null;
		svgEl = null;
		viewportG = null;
		baseLayerG = null;
		overlayLayerG = null;
		nodePanelEl = null;
		focusPillEl = null;
		resetBtnEl = null;
		_focusedNodeId = null;
		_currentGraphData = null;
		_currentLayout = null;
		_colorMap = null;
		_nodeEls.clear();
		_pan = { x: 0, y: 0 };
		_zoom = 1;
		_panState = null;
		_pinchState = null;
		if (_escHandler) {
			document.removeEventListener("keydown", _escHandler);
			_escHandler = null;
		}
	}

	// ==================== Init ====================

	const graphBtn = document.getElementById("graph-view-btn");
	if (graphBtn) graphBtn.addEventListener("click", () => open());

	return { open };
})();
