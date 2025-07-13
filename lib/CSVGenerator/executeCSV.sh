#!/bin/sh
set -e;

# Get the directory where this script is located
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
# Get the lib directory (parent of CSVGenerator)
LIB_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"
# Set output directory for headers
OUTPUT_DIR="$LIB_DIR/CAN"

# Create output directory if it doesn't exist
mkdir -p "$OUTPUT_DIR"

if test -d "$SCRIPT_DIR"
then
    echo "Generating shared CAN ID headers in $OUTPUT_DIR..."
    python3 "$SCRIPT_DIR/grCSVtoH.py" "$SCRIPT_DIR/GR-ID.csv" "$OUTPUT_DIR/grIDs.h"
    python3 "$SCRIPT_DIR/msgCSVtoH.py" "$SCRIPT_DIR/MSG-ID.csv" "$OUTPUT_DIR/msgIDs.h"
    python3 "$SCRIPT_DIR/customIdCSVtoH.py" "$SCRIPT_DIR/CUSTOM-ID.csv" "$OUTPUT_DIR/customIDs.h"
    echo "Headers generated successfully in $OUTPUT_DIR"
else
    echo "CSVGenerator directory not found at $SCRIPT_DIR!"
    exit 1
fi
