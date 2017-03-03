// Render Process
const {ipcRenderer} = require('electron')
const path = require('path');

var wrking_file;

wrking_file = '/Users/Oli/Documents/CRGS/Computer Science Society/AQA Assembler/AQA Assembler/test.asm';

ipcRenderer.send('selectfile', '');

ipcRenderer.on('selectedfile', (event, arg) => {
	console.log(arg[0]);
	document.getElementById('filename').innerHTML = path.basename(arg[0]);
	ipcRenderer.send('open', arg[0]);
});

ipcRenderer.on('opened', (event, arg) => {
	document.getElementById('code').value = arg;
});

ipcRenderer.on('registers', (event, arg) => {
	var registers = document.getElementsByClassName("register");
	for (register in registers) {
		console.log(registers[register].childNodes);
		if (registers[register].childNodes) {
			registers[register].childNodes[2].innerHTML = arg[register];
		}
	}
});

document.getElementById('run').onclick = function () {
	if (ipcRenderer.sendSync('save', {path: wrking_file, data: document.getElementById('code').value})) {
		ipcRenderer.send('run', wrking_file);
	}
}
