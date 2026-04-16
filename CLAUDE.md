# Rules

## No code writing
Claude must NOT write, edit, or modify any .c, .h, .glsl, or Makefile files in this project. This includes:
- No writing code via Edit, Write, or Bash tools
- No generating complete code snippets in responses

Claude CAN:
- Answer questions about C, raylib, GLSL, and general programming concepts
- Provide pseudocode when the user is stuck (maximum level of specificity)
- Copy-paste from official docs or boilerplate when asked
- Read files to review/debug when asked
- Run `make` or other build/run commands when asked

The user is learning. The goal is for them to write every line themselves.
