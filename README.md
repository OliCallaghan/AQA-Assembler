# AQA-Assembler
An assembler emulator written in C++ for AQA assembler (similar to ARM). The aim of this project is to provide a simple emulation of the language for A level students looking to practice using the assembly language used in AQA Computer Science.

To use the assembler, specify the file path for the assembly code, and run the program. The eventual aim for the project is to mount it behind an Electron interface to allow users to write their code and test it in the same place.

To test the program, run the following commands:

```
git clone https://github.com/OliCallaghan/AQA-Assembler.git
cd AQA-Assembler/app
npm install
node-gyp configure
node-gyp build
electron .
```

## Keyboard Shortcuts

`TAB` cycles through the autocompletion menu

`SHIFT+TAB` cycles back through the autocompletion menu

`SHIFT+ENTER` inserts selected autocompletion code template, then, use `TAB` to jump to the next argument.

## Building the Project
Due to the project containing some C++ code, this must be compiled for the target system, so to do this run the following commands.
```
cd AQA-Assembler/app
npm install
node-gyp configure
node-gyp build
cd ..
npm install
npm run pack
```

This will build the project for only the target system and the resulting executable will be found in `dist` (still working on configuring electron-builder to rebuild the C++ addon for all target systems).
