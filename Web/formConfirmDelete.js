// Purpose: "Confirm Delete" modal dialog.
// Generic confirmation prompt used before any destructive in-memory deletion.
// Calls the provided deleteFn only when the user explicitly confirms, then
// triggers a re-render. Reminds the user that changes are in-memory only.
// Depends on: formUtils.js (FormUtils), editor.js (GrcanEditor).
// Registers: window.GrcanEditor.confirmAndDelete

(function () {
	"use strict";

	function confirmAndDelete(itemDesc, deleteFn) {
		const editor = window.GrcanEditor;
		const fu = window.FormUtils;
		const { overlay, body, footer } = fu.createModal("Confirm Delete");

		const msg = document.createElement("p");
		msg.className = "editor-confirm-msg";
		msg.textContent = 'Delete "' + itemDesc + '"?';
		body.appendChild(msg);

		const warn = document.createElement("p");
		warn.className = "editor-confirm-warn";
		warn.textContent =
			"This modifies the CANdo file in memory. Download to save.";
		body.appendChild(warn);

		const cancelBtn = fu.makeBtn("Cancel");
		cancelBtn.addEventListener("click", () => fu.closeOverlay(overlay));
		const delBtn = fu.makeBtn("Delete", "editor-btn-danger");
		footer.appendChild(cancelBtn);
		footer.appendChild(delBtn);

		delBtn.addEventListener("click", () => {
			deleteFn();
			fu.closeOverlay(overlay, { force: true });
			editor.triggerReRender();
		});
	}

	window.GrcanEditor.confirmAndDelete = confirmAndDelete;
})();
