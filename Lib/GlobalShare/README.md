# Global Share

## Purpose

This directory contains shared definitions and interfaces used across multiple projects. The code here is platform-agnostic and provides common types, enums, and structures that ensure consistency across everything.

Please do not add source files to this directory, this is intended for only header files.

**Important**: All files in this directory are **manually maintained and hand-written**. Nothing here is auto-generated. For auto-generated files please see [Autogen](../../Autogen/).

Please ensure everything is clearly doc-commented.

## Adding New Shared Definitions

When adding new shared types or constants:

1. Create a new header file in `Inc/` with a descriptive name
2. Use include guards and follow the existing naming conventions
3. Document everything with clear doc-comments
4. Consider backward compatibility if modifying existing files
