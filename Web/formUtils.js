// Purpose: Shared UI utilities for all editor modal/form components.
// Provides low-level DOM builders (createModal, makeInput, makeSelect, makeFormRow,
// makeBtn), shared SVG icon strings, and field validators (parseNumericText,
// typeBitWidth, isValidMapEquation). Consumed by every form*.js file and by editor.js
// for its icon-button creators.
// Exposed as: window.FormUtils

(function () {
	"use strict";

	// ==================== SVG Icon Strings ====================

	const PENCIL_SVG =
		'<svg xmlns="http://www.w3.org/2000/svg" width="14" height="14" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M17 3a2.85 2.83 0 1 1 4 4L7.5 20.5 2 22l1.5-5.5Z"/><path d="m15 5 4 4"/></svg>';
	const TRASH_SVG =
		'<svg xmlns="http://www.w3.org/2000/svg" width="14" height="14" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M3 6h18"/><path d="M19 6v14c0 1-1 2-2 2H7c-1 0-2-1-2-2V6"/><path d="M8 6V4c0-1 1-2 2-2h4c1 0 2 1 2 2v2"/><line x1="10" x2="10" y1="11" y2="17"/><line x1="14" x2="14" y1="11" y2="17"/></svg>';
	const PLUS_SVG =
		'<svg xmlns="http://www.w3.org/2000/svg" width="14" height="14" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><line x1="12" x2="12" y1="5" y2="19"/><line x1="5" x2="19" y1="12" y2="12"/></svg>';

	// ==================== Modal Builder ====================

	function closeOverlay(overlay, opts) {
		if (!overlay || !overlay.parentNode) return;
		const force = !!(opts && opts.force);
		const isDirty = !!overlay.__editorDirty;
		if (!force && isDirty) {
			const discard = window.confirm(
				"You have unsaved changes. Discard them and close this popup?",
			);
			if (!discard) return;
		}
		overlay.remove();
	}

	function createModal(title) {
		const overlay = document.createElement("div");
		overlay.className = "editor-overlay";
		overlay.__editorDirty = false;
		overlay.addEventListener("click", (e) => {
			if (e.target === overlay) closeOverlay(overlay);
		});

		const modal = document.createElement("div");
		modal.className = "editor-modal";
		function markDirty(e) {
			const t = e && e.target ? e.target : null;
			if (!t) return;
			const tag = (t.tagName || "").toLowerCase();
			if (tag === "input" || tag === "textarea" || tag === "select") {
				overlay.__editorDirty = true;
			}
		}
		modal.addEventListener("input", markDirty, true);
		modal.addEventListener("change", markDirty, true);

		const header = document.createElement("div");
		header.className = "editor-modal-header";
		const h2 = document.createElement("h2");
		h2.textContent = title;
		header.appendChild(h2);
		const closeBtn = document.createElement("button");
		closeBtn.type = "button";
		closeBtn.className = "editor-modal-close";
		closeBtn.innerHTML = "&times;";
		closeBtn.setAttribute("aria-label", "Close");
		closeBtn.addEventListener("click", () => closeOverlay(overlay));
		header.appendChild(closeBtn);

		const body = document.createElement("div");
		body.className = "editor-modal-body";
		const footer = document.createElement("div");
		footer.className = "editor-modal-footer";

		modal.appendChild(header);
		modal.appendChild(body);
		modal.appendChild(footer);
		overlay.appendChild(modal);
		document.body.appendChild(overlay);

		return { overlay, body, footer };
	}

	// ==================== Form Element Builders ====================

	function makeInput(type, value, placeholder) {
		const el =
			type === "textarea"
				? document.createElement("textarea")
				: document.createElement("input");
		el.className = "editor-input";
		if (type !== "textarea") el.type = type || "text";
		else el.rows = 2;
		el.value = value || "";
		if (placeholder) el.placeholder = placeholder;
		return el;
	}

	function makeSelect(choices, selected) {
		const sel = document.createElement("select");
		sel.className = "editor-input";
		choices.forEach((c) => {
			const opt = document.createElement("option");
			opt.value = c;
			opt.textContent = c;
			if (c === selected) opt.selected = true;
			sel.appendChild(opt);
		});
		return sel;
	}

	function makeFormRow(label, inputEl, required) {
		const row = document.createElement("div");
		row.className = "editor-form-row";
		const lbl = document.createElement("label");
		lbl.className = "editor-label";
		lbl.textContent = label;
		if (required) {
			const s = document.createElement("span");
			s.className = "editor-required";
			s.textContent = " *";
			lbl.appendChild(s);
		}
		const errEl = document.createElement("div");
		errEl.className = "editor-error";
		row.appendChild(lbl);
		row.appendChild(inputEl);
		row.appendChild(errEl);
		return { row, input: inputEl, error: errEl };
	}

	function makeBtn(text, cls) {
		const btn = document.createElement("button");
		btn.className = "editor-btn" + (cls ? " " + cls : "");
		btn.textContent = text;
		return btn;
	}

	// ==================== Shared Validators ====================

	function parseNumericText(raw) {
		const v = String(raw || "").trim();
		if (!v) return null;
		const normalized = v.replace(/,/g, "");
		if (!/^-?\d+(\.\d+)?$/.test(normalized)) return null;
		const n = Number(normalized);
		return Number.isFinite(n) ? n : null;
	}

	function typeBitWidth(type) {
		const m = {
			b: 1,
			u4: 4,
			s4: 4,
			u8: 8,
			s8: 8,
			u16: 16,
			s16: 16,
			i16: 16,
			u32: 32,
			s32: 32,
			i32: 32,
		};
		return m[type] || null;
	}

	function isValidMapEquation(raw) {
		const eq = String(raw || "")
			.trim()
			.replace(/\s+/g, "");
		if (!eq) return true;
		return (
			/^x$/i.test(eq) ||
			/^\d+(\.\d+)?x$/i.test(eq) ||
			/^\d+(\.\d+)?x[+-]\d+(\.\d+)?$/i.test(eq) ||
			/^x-\d+(\.\d+)?$/i.test(eq) ||
			/^x\/\d+(\.\d+)?$/i.test(eq) ||
			/^\d+(\.\d+)?x\/\d+(\.\d+)?$/i.test(eq) ||
			/^abs\(x\)\/\d+(\.\d+)?$/i.test(eq)
		);
	}

	window.FormUtils = {
		PENCIL_SVG,
		TRASH_SVG,
		PLUS_SVG,
		closeOverlay,
		createModal,
		makeInput,
		makeSelect,
		makeFormRow,
		makeBtn,
		parseNumericText,
		typeBitWidth,
		isValidMapEquation,
	};
})();
