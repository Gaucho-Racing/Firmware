#!/bin/bash

echo "Generating compile commands for all firmware boards..."

echo "Configuring main project..."
cmake --preset=Debug

echo "Building compile commands for each board..."
cmake --build --preset=Debug --target ECU --parallel &
ECU_PID=$!
cmake --build --preset=Debug --target DashPanel --parallel &
DASH_PID=$!
cmake --build --preset=Debug --target CCU --parallel &
CCU_PID=$!

echo "Waiting for compile command generation..."
wait $ECU_PID
echo "✓ ECU compile commands ready"
wait $DASH_PID  
echo "✓ DashPanel compile commands ready"
wait $CCU_PID
echo "✓ CCU compile commands ready"

echo ""
echo "=== Compile Commands Status ==="
echo "Root project: $([ -f "build/Debug/compile_commands.json" ] && echo "✓" || echo "✗") build/Debug/compile_commands.json"

echo ""
echo "Board compile commands:"
echo "ECU: $([ -f "ECU/build/compile_commands.json" ] && echo "✓" || echo "✗") ECU/build/compile_commands.json"
echo "DashPanel: $([ -f "DashPanel/build/compile_commands.json" ] && echo "✓" || echo "✗") DashPanel/build/compile_commands.json"
echo "CCU: $([ -f "CCU/build/compile_commands.json" ] && echo "✓" || echo "✗") CCU/build/compile_commands.json"

echo ""
echo "To use IntelliSense:"
echo "1. Use Ctrl+Shift+P → 'C/C++: Select a Configuration'"
echo "2. Choose the appropriate board configuration"
echo "3. Or use 'All Projects (Root CMake)' for general development"
