// Purpose: Lightweight side-by-side diff modal shown before download.
// Computes a line-based diff between original and modified CANdo text using a
// greedy lookahead algorithm, renders it as a two-column HTML table with red
// (removed/changed old) and green (added/changed new) highlighting, and calls
// onConfirm or onCancel callbacks based on user action.
// Exposed as: window.DiffViewer  →  DiffViewer.show({ oldText, newText, onConfirm, onCancel })

(function () {
	"use strict";

	function esc(text) {
		return String(text || "")
			.replace(/&/g, "&amp;")
			.replace(/</g, "&lt;")
			.replace(/>/g, "&gt;");
	}

	function buildRows(oldText, newText) {
		const a = String(oldText || "").split("\n");
		const b = String(newText || "").split("\n");
		const rows = [];
		let i = 0;
		let j = 0;
		const LOOKAHEAD = 24;

		while (i < a.length || j < b.length) {
			const left = i < a.length ? a[i] : null;
			const right = j < b.length ? b[j] : null;
			if (left === right && left !== null) {
				rows.push({ t: "ctx", l: left, r: right, li: i + 1, ri: j + 1 });
				i++;
				j++;
				continue;
			}

			let ai = -1;
			let bj = -1;
			for (let k = 1; k <= LOOKAHEAD && i + k < a.length; k++) {
				if (a[i + k] === right) {
					ai = k;
					break;
				}
			}
			for (let k = 1; k <= LOOKAHEAD && j + k < b.length; k++) {
				if (b[j + k] === left) {
					bj = k;
					break;
				}
			}

			if (ai !== -1 && (bj === -1 || ai <= bj)) {
				for (let k = 0; k < ai; k++) {
					rows.push({ t: "del", l: a[i], r: "", li: i + 1, ri: "" });
					i++;
				}
				continue;
			}
			if (bj !== -1) {
				for (let k = 0; k < bj; k++) {
					rows.push({ t: "add", l: "", r: b[j], li: "", ri: j + 1 });
					j++;
				}
				continue;
			}

			if (left !== null && right !== null) {
				rows.push({ t: "mod", l: left, r: right, li: i + 1, ri: j + 1 });
				i++;
				j++;
			} else if (left !== null) {
				rows.push({ t: "del", l: left, r: "", li: i + 1, ri: "" });
				i++;
			} else {
				rows.push({ t: "add", l: "", r: right, li: "", ri: j + 1 });
				j++;
			}
		}
		return rows;
	}

	function renderRows(rows) {
		return rows
			.map((r) => {
				const lc = r.t === "del" || r.t === "mod" ? "dv-del" : "";
				const rc = r.t === "add" || r.t === "mod" ? "dv-add" : "";
				return (
					'<tr><td class="dv-ln ' +
					lc +
					'">' +
					r.li +
					'</td><td class="dv-code ' +
					lc +
					'">' +
					esc(r.l) +
					'</td><td class="dv-ln ' +
					rc +
					'">' +
					r.ri +
					'</td><td class="dv-code ' +
					rc +
					'">' +
					esc(r.r) +
					"</td></tr>"
				);
			})
			.join("");
	}

	function show(opts) {
		const oldText = opts.oldText || "";
		const newText = opts.newText || "";
		const onConfirm = opts.onConfirm || function () {};
		const onCancel = opts.onCancel || function () {};

		const rows = buildRows(oldText, newText);
		const overlay = document.createElement("div");
		overlay.className = "dv-overlay";
		overlay.innerHTML =
			'<div class="dv-modal"><div class="dv-head"><h2>Review Changes</h2><button class="dv-close">&times;</button></div><div class="dv-body"><table class="dv-table"><thead><tr><th class="dv-ln">Old#</th><th>Original GRCAN.CANdo</th><th class="dv-ln">New#</th><th>Modified GRCAN.CANdo</th></tr></thead><tbody>' +
			renderRows(rows) +
			'</tbody></table></div><div class="dv-foot"><button class="dv-btn" data-act="cancel">Cancel</button><button class="dv-btn dv-btn-primary" data-act="confirm">Download</button></div></div>';
		document.body.appendChild(overlay);

		function close() {
			if (overlay && overlay.parentNode) overlay.remove();
		}
		overlay.querySelector(".dv-close").addEventListener("click", () => {
			close();
			onCancel();
		});
		overlay.addEventListener("click", (e) => {
			if (e.target === overlay) {
				close();
				onCancel();
			}
		});
		overlay.querySelector('[data-act="cancel"]').addEventListener("click", () => {
			close();
			onCancel();
		});
		overlay.querySelector('[data-act="confirm"]').addEventListener("click", () => {
			close();
			onConfirm();
		});
	}

	window.DiffViewer = { show };
})();

