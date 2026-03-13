// Purpose: Decorative background canvas animation.
// Generates and draws a static field of brand-colored dot clusters onto a fixed
// fullscreen canvas. Redraws on scroll and resize to keep the pattern aligned.
// Exposes window.regenerateAndDrawBg() so viewer.js can force a refresh after a
// ref change (which may alter page height).

const colors = ["#195297", "#7920FF", "#EF0DA1"];
const SQUARE_SIZE = 18;
const DENSITY = 0.18;
const CIRCLE_RADIUS = 120;
let NUM_CIRCLES = 200;
let circles = [];
let lastW = 0;
let lastH = 0;

function randomCircle(w, h) {
	const radius = 50 + Math.random() * 150;
	const cx = Math.random() * w;
	const cy = Math.random() * h;
	const color = Math.floor(Math.random() * colors.length);
	const localDensity = 0.08 + Math.random() * 0.2;
	const squares = [];
	for (let y = -radius; y <= radius; y += SQUARE_SIZE) {
		for (let x = -radius; x <= radius; x += SQUARE_SIZE) {
			const dist = Math.sqrt(x * x + y * y);
			if (dist < radius && Math.random() < localDensity) {
				squares.push({
					x: cx + x,
					y: cy + y,
					dist,
				});
			}
		}
	}
	return { color, squares, radius };
}

let bgSquares = null;

let staticCircles = null;

function regenerateStaticCircles() {
	const w = Math.max(
		window.innerWidth,
		document.documentElement.scrollWidth,
		document.body.scrollWidth,
	);
	const h = Math.max(
		window.innerHeight,
		document.documentElement.scrollHeight,
		document.body.scrollHeight,
	);
	NUM_CIRCLES = Math.max(5, Math.floor(w * h * 0.000015));
	staticCircles = { w, h, circles: generateBgSquares(w, h) };
}

function generateBgSquares(w, h) {
	const circles = [];
	let totalSquares = 0;
	let attempts = 0;
	while (circles.length < NUM_CIRCLES && attempts < NUM_CIRCLES * 4) {
		const c = randomCircle(w, h);
		if (c.squares.length > 0) {
			circles.push(c);
			totalSquares += c.squares.length;
		}
		attempts++;
	}
	return circles;
}

function drawStaticSquaresBg() {
	const canvas = document.getElementById("bg-canvas");
	if (!canvas) return;
	const ctx = canvas.getContext("2d");
	const viewW = window.innerWidth;
	const viewH = window.innerHeight;
	canvas.width = viewW;
	canvas.height = viewH;
	canvas.style.width = viewW + "px";
	canvas.style.height = viewH + "px";
	canvas.style.position = "fixed";
	canvas.style.top = "0";
	canvas.style.left = "0";
	canvas.style.zIndex = "0";
	ctx.clearRect(0, 0, viewW, viewH);
	ctx.fillStyle = "#000";
	ctx.fillRect(0, 0, viewW, viewH);

	const pageW = Math.max(
		window.innerWidth,
		document.documentElement.scrollWidth,
		document.body.scrollWidth,
	);
	const pageH = Math.max(
		window.innerHeight,
		document.documentElement.scrollHeight,
		document.body.scrollHeight,
	);
	if (
		!staticCircles ||
		staticCircles.w !== pageW ||
		staticCircles.h !== pageH
	) {
		regenerateStaticCircles();
	}
	const circles = staticCircles.circles;
	const scrollX = window.scrollX || window.pageXOffset || 0;
	const scrollY = window.scrollY || window.pageYOffset || 0;
	for (let i = 0; i < circles.length; ++i) {
		const circ = circles[i];
		for (let j = 0; j < circ.squares.length; ++j) {
			const sq = circ.squares[j];
			const sx = sq.x - scrollX;
			const sy = sq.y - scrollY;
			if (
				sx + SQUARE_SIZE < 0 ||
				sx > viewW ||
				sy + SQUARE_SIZE < 0 ||
				sy > viewH
			)
				continue;
			let edgeAlpha = 1 - sq.dist / circ.radius;
			edgeAlpha = Math.max(0, Math.min(1, edgeAlpha));
			ctx.globalAlpha = edgeAlpha * 0.95 + 0.05;
			ctx.fillStyle = colors[circ.color];
			ctx.fillRect(sx, sy, SQUARE_SIZE, SQUARE_SIZE);
			ctx.globalAlpha = 1;
		}
	}
}

window.regenerateAndDrawBg = function () {
	regenerateStaticCircles();
	drawStaticSquaresBg();
};

document.addEventListener("DOMContentLoaded", function () {
	window.regenerateAndDrawBg();
});

window.addEventListener("scroll", function () {
	drawStaticSquaresBg();
});
window.addEventListener("resize", function () {
	drawStaticSquaresBg();
});
