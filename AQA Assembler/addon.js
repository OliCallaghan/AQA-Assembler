var addon = require('bindings')('addon.node');

console.log('This should be eight:', addon.run("/Users/Oli/Documents/CRGS/Computer Science Society/AQA Assembler/AQA Assembler/test.asm"))
