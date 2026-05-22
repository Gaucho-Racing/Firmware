// Purpose: Deterministic layout for the physical-bus Graph View renderer.
// Pure function: takes a {top, bottom, bus} partition (from PhysicalGroups)
// plus a viewport size, and returns explicit coordinates for every node,
// stub, group box, and bus rail. No DOM access, no animation state.
// Exposed as: window.LayoutPhysicalBus.

(function () {
	"use strict";

	const NODE_MIN_WIDTH = 80;
	const NODE_HEIGHT = 54;
	const NODE_GAP_X = 16;
	const NODE_GAP_Y = 14;
	const GROUP_INNER_PAD = 14;
	const GROUP_OUTER_GAP = 28;
	const GROUP_TITLE_HEIGHT = 22;
	const STUB_MIN = 34;
	const RAIL_GAP = 8;
	const PAGE_PAD_X = 56;
	const PAGE_PAD_Y = 60;

	// Approximate character width for 12px monospace in SVG user units,
	// plus horizontal padding for the node rect.
	const CHAR_WIDTH = 7.5;
	const NODE_H_PAD = 24; // total left+right padding inside the node rect

	function _nodeWidth(name) {
		return Math.max(
			NODE_MIN_WIDTH,
			Math.ceil((name || "").length * CHAR_WIDTH) + NODE_H_PAD,
		);
	}

	function _chooseCols(n) {
		if (n <= 3) return n;
		if (n <= 4) return 2;
		if (n <= 6) return 3;
		return 4;
	}

	// Accepts the array of node name strings so each column can be sized to
	// the widest name it contains.
	function _groupBlockSize(nodes) {
		const nodeCount = nodes.length;
		if (nodeCount === 0) return { w: 0, h: 0, cols: 0, rows: 0, colWidths: [] };
		const cols = _chooseCols(nodeCount);
		const rows = Math.ceil(nodeCount / cols);

		// Per-column max width.
		const colWidths = new Array(cols).fill(NODE_MIN_WIDTH);
		nodes.forEach((name, i) => {
			const col = i % cols;
			colWidths[col] = Math.max(colWidths[col], _nodeWidth(name));
		});

		const innerW =
			colWidths.reduce((s, w) => s + w, 0) + (cols - 1) * NODE_GAP_X;
		const innerH = rows * NODE_HEIGHT + (rows - 1) * NODE_GAP_Y;
		return {
			w: innerW + 2 * GROUP_INNER_PAD,
			h: innerH + 2 * GROUP_INNER_PAD + GROUP_TITLE_HEIGHT,
			cols,
			rows,
			colWidths,
		};
	}

	function _sideTotalWidth(blocks) {
		if (blocks.length === 0) return 0;
		let w = 0;
		for (const b of blocks) w += b.size.w;
		w += (blocks.length - 1) * GROUP_OUTER_GAP;
		return w;
	}

	function _maxBlockHeight(blocks) {
		let h = 0;
		for (const b of blocks) if (b.size.h > h) h = b.size.h;
		return h;
	}

	function _placeSide(blocks, totalW, topY, logicalWidth) {
		const startX = (logicalWidth - totalW) / 2;
		let cursorX = startX;
		const result = [];
		for (const b of blocks) {
			const positions = [];
			const { cols, colWidths } = b.size;
			// Running x-offset within the group for each column.
			const colOffsets = [];
			let colCursor = 0;
			for (let c = 0; c < cols; c++) {
				colOffsets.push(colCursor);
				colCursor += colWidths[c] + NODE_GAP_X;
			}
			b.group.nodes.forEach((nodeId, i) => {
				const col = i % cols;
				const row = Math.floor(i / cols);
				const nx = cursorX + GROUP_INNER_PAD + colOffsets[col];
				const ny =
					topY +
					GROUP_TITLE_HEIGHT +
					GROUP_INNER_PAD +
					row * (NODE_HEIGHT + NODE_GAP_Y);
				positions.push({
					id: nodeId,
					x: nx,
					y: ny,
					w: colWidths[col],
					h: NODE_HEIGHT,
				});
			});
			result.push({
				name: b.group.name,
				x: cursorX,
				y: topY,
				w: b.size.w,
				h: b.size.h,
				positions,
			});
			cursorX += b.size.w + GROUP_OUTER_GAP;
		}
		return result;
	}

	function layout(partition, viewportW, viewportH) {
		const { top, bottom, bus } = partition;

		const topBlocks = top.map((g) => ({
			group: g,
			size: _groupBlockSize(g.nodes),
		}));
		const bottomBlocks = bottom.map((g) => ({
			group: g,
			size: _groupBlockSize(g.nodes),
		}));

		const topTotalW = _sideTotalWidth(topBlocks);
		const bottomTotalW = _sideTotalWidth(bottomBlocks);

		// Bus-spine nodes each get their own measured width.
		const busRowW =
			bus.length > 0
				? bus.reduce((s, id) => s + _nodeWidth(id), 0) +
					(bus.length - 1) * NODE_GAP_X
				: 0;

		const contentW = Math.max(topTotalW, bottomTotalW, busRowW);
		const logicalWidth = Math.max(viewportW, contentW + 2 * PAGE_PAD_X);

		const topMaxH = _maxBlockHeight(topBlocks);
		const bottomMaxH = _maxBlockHeight(bottomBlocks);

		const busY = PAGE_PAD_Y + topMaxH + STUB_MIN + NODE_HEIGHT / 2;
		const bottomTopY = busY + NODE_HEIGHT / 2 + STUB_MIN;
		const logicalHeight = Math.max(
			viewportH,
			bottomTopY + bottomMaxH + PAGE_PAD_Y,
		);

		const topGroupLayouts = _placeSide(
			topBlocks,
			topTotalW,
			PAGE_PAD_Y,
			logicalWidth,
		);
		topGroupLayouts.forEach((gl) => (gl.side = "top"));
		const bottomGroupLayouts = _placeSide(
			bottomBlocks,
			bottomTotalW,
			bottomTopY,
			logicalWidth,
		);
		bottomGroupLayouts.forEach((gl) => (gl.side = "bottom"));

		const busNodes = [];
		if (bus.length > 0) {
			let bx = (logicalWidth - busRowW) / 2;
			bus.forEach((id) => {
				const nw = _nodeWidth(id);
				busNodes.push({
					id,
					x: bx,
					y: busY - NODE_HEIGHT / 2,
					w: nw,
					h: NODE_HEIGHT,
				});
				bx += nw + NODE_GAP_X;
			});
		}

		const nodePositions = new Map();
		for (const gl of topGroupLayouts)
			for (const p of gl.positions) nodePositions.set(p.id, p);
		for (const gl of bottomGroupLayouts)
			for (const p of gl.positions) nodePositions.set(p.id, p);
		for (const p of busNodes) nodePositions.set(p.id, p);

		const stubs = [];
		for (const gl of topGroupLayouts) {
			for (const p of gl.positions) {
				stubs.push({
					id: p.id,
					x: p.x + p.w / 2,
					y1: p.y + p.h,
					y2: busY,
				});
			}
		}
		for (const gl of bottomGroupLayouts) {
			for (const p of gl.positions) {
				stubs.push({
					id: p.id,
					x: p.x + p.w / 2,
					y1: p.y,
					y2: busY,
				});
			}
		}

		const busGeometry = {
			x1: PAGE_PAD_X,
			x2: logicalWidth - PAGE_PAD_X,
			yHigh: busY - RAIL_GAP / 2,
			yLow: busY + RAIL_GAP / 2,
			yCenter: busY,
		};

		return {
			nodePositions,
			busGeometry,
			stubs,
			busNodes,
			logicalWidth,
			logicalHeight,
		};
	}

	window.LayoutPhysicalBus = { layout };
})();
