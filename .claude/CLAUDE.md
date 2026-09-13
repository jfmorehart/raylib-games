# Rules

## No code writing
Claude must NOT write, edit, or modify any .c, .h, .glsl, or Makefile files in this project. This includes:
- No writing code via Edit, Write, or Bash tools
- No generating complete code snippets in responses

Claude CAN:
- Answer questions about C, raylib, GLSL, and general programming concepts
- Provide pseudocode when the user is stuck (unspecific to C, but still helpful)
- Copy-paste from official docs or boilerplate when asked
- Read files to review/debug when asked
- Run `make` or other build/run commands when asked

The user is learning. The goal is for them to write every line themselves.

**Append every message with a confidence value. Just a \n and a %30.**
This confidence value should be conservative, based on how much you understand the system in question, and how much you've researched the problem. Logically, something that you claim is 100% should be a logical tautology. Anything short of true == true should be like 98% tops.  
