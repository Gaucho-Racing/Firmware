#!/usr/bin/env bash
#
# Cross-platform Unix STM32CubeMX custom-template path configurator.
# Compatible with macOS Bash 3.2+ and common Linux Bash versions.
#
# Expected repository structure:
#
# Firmware/
# ├── tools/
# │   └── configure_cubemx_paths_linux.sh
# ├── CUBEMXTESTING/
# │   └── CUBEMXTESTING.ioc
# └── Lib/
#     └── CubeMXTemplates/
#         └── Autogen/
#             └── Inc/
#                 └── can_cfg_h.ftl
#
# Usage:
#   bash tools/configure_cubemx_paths_linux.sh
#
# Optional overrides:
#   IOC_FILE=/path/project.ioc \
#   PROJECT_OUTPUT_DIR=/path/project \
#   TEMPLATE_ROOT=/path/CubeMXTemplates \
#   bash tools/configure_cubemx_paths_linux.sh
#
# Behavior:
#   - validates all expected paths;
#   - removes stale or duplicate CubeMX template properties;
#   - writes exactly one correct copy of each property;
#   - creates an .ioc.bak backup;
#   - writes through a temporary file;
#   - verifies the result from disk;
#   - is idempotent.

set -euo pipefail

EXPECTED_OS="linux"

die() {
    printf 'ERROR: %s\n' "$*" >&2
    exit 1
}

info() {
    printf '%s\n' "$*"
}

detect_os() {
    case "$(uname -s)" in
        Darwin) printf '%s\n' "macos" ;;
        Linux)  printf '%s\n' "linux" ;;
        *)      printf '%s\n' "unsupported" ;;
    esac
}

CURRENT_OS="$(detect_os)"
if [ "$CURRENT_OS" != "$EXPECTED_OS" ]; then
    die "This script is intended for $EXPECTED_OS, but detected $CURRENT_OS."
fi

# Resolve the physical directory containing this script without requiring
# realpath, which is not included with older macOS installations.
SCRIPT_DIR="$(
    CDPATH= cd -- "$(dirname -- "$0")" >/dev/null 2>&1
    pwd -P
)"
REPOSITORY_ROOT="$(
    CDPATH= cd -- "$SCRIPT_DIR/.." >/dev/null 2>&1
    pwd -P
)"

IOC_FILE="${IOC_FILE:-$REPOSITORY_ROOT/CUBEMXTESTING/CUBEMXTESTING.ioc}"
PROJECT_OUTPUT_DIR="${PROJECT_OUTPUT_DIR:-$REPOSITORY_ROOT/CUBEMXTESTING}"
TEMPLATE_ROOT="${TEMPLATE_ROOT:-$REPOSITORY_ROOT/Lib/CubeMXTemplates}"
TEMPLATE_FILE="$TEMPLATE_ROOT/Autogen/Inc/can_cfg_h.ftl"

[ -f "$IOC_FILE" ] ||
    die "CubeMX .ioc file was not found: $IOC_FILE"

[ -d "$PROJECT_OUTPUT_DIR" ] ||
    die "CubeMX project output directory was not found: $PROJECT_OUTPUT_DIR"

[ -d "$TEMPLATE_ROOT" ] ||
    die "CubeMX template root was not found: $TEMPLATE_ROOT"

[ -f "$TEMPLATE_FILE" ] ||
    die "CubeMX FreeMarker template was not found: $TEMPLATE_FILE"

# Convert directories to physical absolute paths.
PROJECT_OUTPUT_DIR="$(
    CDPATH= cd -- "$PROJECT_OUTPUT_DIR" >/dev/null 2>&1
    pwd -P
)"
TEMPLATE_ROOT="$(
    CDPATH= cd -- "$TEMPLATE_ROOT" >/dev/null 2>&1
    pwd -P
)"

DESTINATION_KEY='ProjectManager.TemplateDestinationPath'
SOURCE_KEY='ProjectManager.TemplateSourcePath'
TEMPLATES_KEY='ProjectManager.TemplatesList'

DESTINATION_LINE="${DESTINATION_KEY}=${PROJECT_OUTPUT_DIR}"
SOURCE_LINE="${SOURCE_KEY}=${TEMPLATE_ROOT}"
TEMPLATES_LINE="${TEMPLATES_KEY}=/Autogen/Inc/can_cfg_h.ftl,"

IOC_DIRECTORY="$(dirname -- "$IOC_FILE")"
IOC_BASENAME="$(basename -- "$IOC_FILE")"
BACKUP_FILE="${IOC_FILE}.bak"
TEMP_FILE="$(mktemp "${IOC_DIRECTORY}/.${IOC_BASENAME}.tmp.XXXXXX")"

cleanup() {
    if [ -n "${TEMP_FILE:-}" ] && [ -f "$TEMP_FILE" ]; then
        rm -f -- "$TEMP_FILE"
    fi
}
trap cleanup EXIT HUP INT TERM

# Back up the current file before any modification.
cp -p -- "$IOC_FILE" "$BACKUP_FILE"

# Remove every existing copy of the three properties and append one stable,
# complete block. awk preserves all unrelated lines verbatim at the text level,
# including semicolons and similarly named properties.
awk \
    -v destination_key="$DESTINATION_KEY" \
    -v source_key="$SOURCE_KEY" \
    -v templates_key="$TEMPLATES_KEY" \
    -v destination_line="$DESTINATION_LINE" \
    -v source_line="$SOURCE_LINE" \
    -v templates_line="$TEMPLATES_LINE" '
BEGIN {
    destination_prefix = destination_key "="
    source_prefix = source_key "="
    templates_prefix = templates_key "="
}
{
    # Remove a possible CR from CRLF input so Unix output is consistently LF.
    sub(/\r$/, "", $0)

    if (index($0, destination_prefix) == 1) {
        next
    }

    if (index($0, source_prefix) == 1) {
        next
    }

    if (index($0, templates_prefix) == 1) {
        next
    }

    lines[++count] = $0
}
END {
    # Remove trailing blank lines before appending the generated block.
    while (count > 0 && lines[count] == "") {
        count--
    }

    for (i = 1; i <= count; i++) {
        print lines[i]
    }

    if (count > 0) {
        print ""
    }

    print destination_line
    print source_line
    print templates_line
}
' "$IOC_FILE" > "$TEMP_FILE"

[ -s "$TEMP_FILE" ] ||
    die "Generated temporary .ioc file is empty: $TEMP_FILE"

verify_exact_line_once() {
    expected_line="$1"
    description="$2"

    # grep -F treats the path as literal text. -x requires the full line.
    count="$(
        grep -Fxc -- "$expected_line" "$TEMP_FILE" 2>/dev/null || true
    )"

    [ "$count" -eq 1 ] ||
        die "$description verification failed; expected exactly once, found $count."
}

verify_key_once() {
    key="$1"
    description="$2"

    count="$(
        awk -v prefix="${key}=" '
            index($0, prefix) == 1 { count++ }
            END { print count + 0 }
        ' "$TEMP_FILE"
    )"

    [ "$count" -eq 1 ] ||
        die "$description key verification failed; expected exactly one property, found $count."
}

verify_key_once "$DESTINATION_KEY" "Destination path"
verify_key_once "$SOURCE_KEY" "Source path"
verify_key_once "$TEMPLATES_KEY" "Template list"

verify_exact_line_once "$DESTINATION_LINE" "Destination path"
verify_exact_line_once "$SOURCE_LINE" "Source path"
verify_exact_line_once "$TEMPLATES_LINE" "Template list"

# Replace the original only after all temporary-file checks pass.
mv -f -- "$TEMP_FILE" "$IOC_FILE"
TEMP_FILE=""

# Verify the file again after it has been moved into place.
verify_disk_line_once() {
    expected_line="$1"
    description="$2"

    count="$(
        grep -Fxc -- "$expected_line" "$IOC_FILE" 2>/dev/null || true
    )"

    [ "$count" -eq 1 ] ||
        die "Post-write $description verification failed; found $count copies."
}

verify_disk_line_once "$DESTINATION_LINE" "destination path"
verify_disk_line_once "$SOURCE_LINE" "source path"
verify_disk_line_once "$TEMPLATES_LINE" "template list"

info ""
info "CubeMX template configuration updated and verified."
info ""
info "$DESTINATION_LINE"
info "$SOURCE_LINE"
info "$TEMPLATES_LINE"
info ""
info "Backup:"
info "  $BACKUP_FILE"
