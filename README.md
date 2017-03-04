# AQA-Assembler
An assembler emulator written in C++ for AQA assembler (similar to ARM). The aim of this project is to provide a simple emulation of the language for A level students looking to practice using the assembly language used in AQA Computer Science.

To use the assembler, specify the file path for the assembly code, and run the program. The eventual aim for the project is to mount it behind an Electron interface to allow users to write their code and test it in the same place.

To test the program, run the following commands:

```
git clone https://github.com/OliCallaghan/AQA-Assembler.git
cd AQA-Assembler/AQA\ Assembler
npm install
node-gyp build
electron .
```

There are a few simple keyboard shortcuts with this program:

`TAB` cycles through the autocompletion menu
`SHIFT+TAB` cycles back through the autocompletion menu
`SHIFT+ENTER` inserts selected autocompletion code template, then, use `TAB` to jump to the next argument.
