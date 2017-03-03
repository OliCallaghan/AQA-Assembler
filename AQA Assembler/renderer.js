// Render Process
const {ipcRenderer} = require('electron')
const path = require('path');
ipcRenderer.on('registers', (event, arg) => {
	var registers = document.getElementsByClassName("register");
	for (register in registers) {
		console.log(registers[register].childNodes);
		if (registers[register].childNodes) {
			registers[register].childNodes[2].innerHTML = arg[register];
		}
	}
});
