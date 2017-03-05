// Import Native NodeJS Libraries
const path = require('path');
const url = require('url');
const fs = require('fs');

// Import AQA-Assembler Emulator
var addon = require('bindings')('addon.node');

// Import Electron Dependencies
const electron = require('electron');
const {ipcMain} = require('electron');
const {dialog} = require('electron');
const {globalShortcut} = require('electron');
const {Menu} = require('electron');

// Initialise Electron App
const app = electron.app;

// Initialise Window Controller
const BrowserWindow = electron.BrowserWindow

// Prevent window closing from JS garbage collector :O
let mainWindow

// Called when the Electron app initialises
function createWindow() {
    // Create the browser window
    mainWindow = new BrowserWindow({
        width: 800,
        height: 600
    });

    // Load HTML for browser window
    mainWindow.loadURL(url.format({
        pathname: path.join(__dirname, 'index.html'),
        protocol: 'file:',
        slashes: true
    }));

    // Open the DevTools.
    // mainWindow.webContents.openDevTools()

    // IPC Communications with the Render Process
    // Listen on channel to run ASM
    ipcMain.on('run', (event, arg) => {
        addon.run(arg, function(rtrn) {
            if (typeof(rtrn[0]) == 'string') {
                event.sender.send('error', rtrn);
            } else {
                event.sender.send('registers', rtrn);
            }
        });
    });

    // Listen on channel to save file contents
    ipcMain.on('save', (event, arg) => {
        fs.writeFile(arg.path, arg.data, function(err) {
            event.returnValue = true;
        });
    });

    // Listen on channel to prompt user to select file
    ipcMain.on('selectfile', (event, arg) => {
        selectFile(event.sender);
    });

    // Listen on channel to open file
    ipcMain.on('open', (event, arg) => {
        fs.readFile(arg, function(err, data) {
            // Return contents to render process
            event.sender.send('opened', data);
        });
    });

    ipcMain.on('savenewfile', (event, arg) => {
        var path = dialog.showSaveDialog({ title: 'Create New AQA Assembler File' });
        if (path) {
            fs.writeFile(path, arg, function(err) {
                event.returnValue = { path: path, err: err };
            });
        } else {
            event.returnValue = { err: 'no_selection' }
        }

    });

    // Clean up main window
    mainWindow.on('closed', function() {
        mainWindow = null;
    });
}

function selectFile(e) {
    var res = dialog.showOpenDialog({
        properties: ['openFile']
    });

    if (res) {
        e.send('selectedfile', res);
    } else {
        return false;
    }
}

// Electron app has initialised
app.on('ready', createWindow);

// Exit electron app when all windows are closed (except on Mac)
app.on('window-all-closed', function() {
    if (process.platform !== 'darwin') {
        app.quit();
    }
});

// Re-open window on Mac if Main Window was closed and dock icon was clicked.
app.on('activate', function() {
    if (mainWindow === null) {
        // Create new window
        createWindow();
    }
});

app.on('will-quit', function () {
    // Unregister all keyboard shortcuts
    globalShortcut.unregisterAll();
});
