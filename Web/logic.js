const GR_PINK = "#EF0DA1";
const GR_PURPLE = "#7920FF";
const GR_NAVY = "#195297";
const GR_GRAY = "#9AA3B0";

const GITHUB_API = "https://api.github.com/repos/Gaucho-Racing/Firmware";
const CANDO_PATH = "Autogen/CAN/Doc/GRCAN.CANdo";

const refInput = document.getElementById("ref-combobox");
const refList = document.getElementById("ref-list");
const candoBox = document.getElementById("cando-box");

function isValidRef(val) {
  for (let i = 0; i < refList.options.length; ++i) {
    if (refList.options[i].value === val) return true;
  }
  return isValidSha(val);
}

function updateInputValidityColor() {
  if (isValidRef(refInput.value)) {
    refInput.classList.remove("invalid");
  } else {
    refInput.classList.add("invalid");
  }
}

refInput.addEventListener("focus", function () {
  const val = this.value;
  if (isValidRef(val)) {
    this.value = "";
    setTimeout(() => {
      this.value = val;
    }, 0);
  }
});

refInput.addEventListener("input", updateInputValidityColor);
refInput.addEventListener("change", updateInputValidityColor);

updateInputValidityColor();

refInput.addEventListener("input", function () {
  this.style.width = "1ch";
  this.style.width = this.value.length + 2 + "ch";
});

function isValidSha(str) {
  if (typeof str !== "string") return false;
  if (str.length < 7 || str.length > 40) return false;
  for (let i = 0; i < str.length; ++i) {
    const c = str[i];
    if (
      !(
        (c >= "0" && c <= "9") ||
        (c >= "a" && c <= "f") ||
        (c >= "A" && c <= "F")
      )
    ) {
      return false;
    }
  }
  return true;
}

async function fetchBranches() {
  try {
    const res = await fetch(`${GITHUB_API}/branches`);
    if (!res.ok) {
      return "RATE_LIMITED";
    }
    const branches = await res.json();
    return branches.map((b) => b.name);
  } catch (e) {
    return "RATE_LIMITED";
  }
}

async function fetchTags() {
  try {
    const res = await fetch(`${GITHUB_API}/tags`);
    if (!res.ok) {
      return "RATE_LIMITED";
    }
    const tags = await res.json();
    return tags.map((t) => t.name);
  } catch (e) {
    return "RATE_LIMITED";
  }
}

async function fetchCando(ref) {
  try {
    const res = await fetch(
      `${GITHUB_API}/contents/${CANDO_PATH}?ref=${encodeURIComponent(ref)}`,
    );
    if (res.status === 403) {
      return { content: "[GitHub API rate limited]", notFound: false };
    }
    if (res.status === 404) {
      return {
        content: "[Unable to load GRCAN.CANdo for this reference]",
        notFound: true,
      };
    }
    if (!res.ok) throw new Error("File not found");
    const data = await res.json();
    if (data.encoding === "base64") {
      const decoded = atob(data.content.replace(/\n/g, ""));
      return { content: decoded, notFound: false };
    } else {
      return { content: "[Unsupported file encoding]", notFound: true };
    }
  } catch (e) {
    return {
      content: "[Unable to load GRCAN.CANdo for this reference]",
      notFound: true,
    };
  }
}

function setLoadingState(isLoading) {
  candoBox.innerHTML = `<span class="placeholder">${isLoading ? "Loading..." : "Type or select a branch, tag, or SHA to view GRCAN.CANdo"}</span>`;
}

async function onRefInputChange() {
  const ref = refInput.value.trim();
  if (!ref) {
    candoBox.innerHTML =
      '<span class="placeholder">Please enter a branch, tag, or commit SHA</span>';
    return;
  }
  setLoadingState(true);
  const result = await fetchCando(ref);
  candoBox.textContent = result.content;
  if (result.notFound) {
    refInput.classList.add("invalid");
  } else {
    refInput.classList.remove("invalid");
  }
  if (typeof window.regenerateAndDrawBg === "function") {
    window.regenerateAndDrawBg();
  }
}

refInput.addEventListener("change", onRefInputChange);
refInput.addEventListener("keydown", function (e) {
  if (e.key === "Enter") {
    onRefInputChange();
  }
});

async function init() {
  setLoadingState(true);
  const [branches, tags] = await Promise.all([fetchBranches(), fetchTags()]);
  if (branches === "RATE_LIMITED" || tags === "RATE_LIMITED") {
    setLoadingState(false);
    const refInputRow = document.getElementById("ref-input-row");
    if (refInputRow) refInputRow.classList.add("hidden");
    candoBox.innerHTML =
      '<span class="placeholder">GitHub API unavailable or rate limited. Please try again in a few minutes.</span>';
    return;
  }
  refList.innerHTML = "";
  let mainFound = false;
  if (branches.length === 0 && tags.length === 0) {
    setLoadingState(false);
    return;
  }
  branches.forEach((branch) => {
    const opt = document.createElement("option");
    opt.value = branch;
    refList.appendChild(opt);
    if (branch === "main") mainFound = true;
  });
  tags.forEach((tag) => {
    const opt = document.createElement("option");
    opt.value = tag;
    refList.appendChild(opt);
  });
  if (mainFound) {
    refInput.value = "main";
    updateInputValidityColor();
    setLoadingState(true);
    const result = await fetchCando("main");
    candoBox.textContent = result.content;
    if (result.notFound) {
      refInput.classList.add("invalid");
    } else {
      refInput.classList.remove("invalid");
    }
  } else {
    setLoadingState(false);
  }
}

window.addEventListener("DOMContentLoaded", init);
