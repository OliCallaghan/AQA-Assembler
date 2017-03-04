// Import Native NodeJS Libraries
const path = require('path');

// Import Electron Dependencies
const {ipcRenderer} = require('electron')

// Working File
var wrking_file;

// Prompt user for file to open on launch
ipcRenderer.send('selectfile', '');

// User selected file
ipcRenderer.on('selectedfile', (event, arg) => {
    document.getElementById('filename').innerHTML = path.basename(arg[0]);
    ipcRenderer.send('open', arg[0]);
});

// User opened file
ipcRenderer.on('opened', (event, arg) => {
    // Set textarea contents to opened file
    document.getElementById('code').value = arg;
});

// Real-time register data recieved
ipcRenderer.on('registers', (event, arg) => {
    // Update GUI with this datas
    var registers = document.getElementsByClassName("register");
    for (register in registers) {
        if (registers[register].childNodes) {
            registers[register].childNodes[2].innerHTML = arg[register];
        }
    }
});

// Add handler for run button
document.getElementById('run').onclick = function() {
    // Save file first
    if (ipcRenderer.sendSync('save', {
            path: wrking_file,
            data: document.getElementById('code').value
        })) {
        // Run file
        ipcRenderer.send('run', wrking_file);
    }
}
