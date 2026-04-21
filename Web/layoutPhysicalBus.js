// Purpose: Deterministic layout for the physical-bus Graph View renderer.
// Pure function: takes a {top, bottom, bus} partition (from PhysicalGroups)
// plus a viewport size, and returns explicit coordinates for every node,
// stub, group box, and bus rail. No DOM access, no animation state.
// Exposed as: window.LayoutPhysicalBus.

(function () {
	"use strict";

	const NODE_WIDTH = 112;
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

	function _chooseCols(n) {
		if (n <= 3) return n;
		if (n <= 4) return 2;
		if (n <= 6) return 3;
		return 4;
	}

	function _groupBlockSize(nodeCount) {
		if (nodeCount === 0) return { w: 0, h: 0, cols: 0, rows: 0 };
		const cols = _chooseCols(nodeCount);
		const rows = Math.ceil(nodeCount / cols);
		const innerW = cols * NODE_WIDTH + (cols - 1) * NODE_GAP_X;
		const innerH = rows * NODE_HEIGHT + (rows - 1) * NODE_GAP_Y;
		return {
			w: innerW + 2 * GROUP_INNER_PAD,
			h: innerH + 2 * GROUP_INNER_PAD + GROUP_TITLE_HEIGHT,
			cols,
			rows,
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
			const { cols } = b.size;
			b.group.nodes.forEach((nodeId, i) => {
				const col = i % cols;
				const row = Math.floor(i / cols);
				const nx =
					cursorX +
					GROUP_INNER_PAD +
					col * (NODE_WIDTH + NODE_GAP_X);
				const ny =
					topY +
					GROUP_TITLE_HEIGHT +
					GROUP_INNER_PAD +
					row * (NODE_HEIGHT + NODE_GAP_Y);
				positions.push({
					id: nodeId,
					x: nx,
					y: ny,
					w: NODE_WIDTH,
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
			size: _groupBlockSize(g.nodes.length),
		}));
		const bottomBlocks = bottom.map((g) => ({
			group: g,
			size: _groupBlockSize(g.nodes.length),
		}));

		const topTotalW = _sideTotalWidth(topBlocks);
		const bottomTotalW = _sideTotalWidth(bottomBlocks);
		const busRowW =
			bus.length > 0
				? bus.length * NODE_WIDTH + (bus.length - 1) * NODE_GAP_X
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
			const startX = (logicalWidth - busRowW) / 2;
			bus.forEach((id, i) => {
				busNodes.push({
					id,
					x: startX + i * (NODE_WIDTH + NODE_GAP_X),
					y: busY - NODE_HEIGHT / 2,
					w: NODE_WIDTH,
					h: NODE_HEIGHT,
				});
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
			groups: [...topGroupLayouts, ...bottomGroupLayouts],
			busNodes,
			logicalWidth,
			logicalHeight,
		};
	}

	window.LayoutPhysicalBus = { layout };
})();
