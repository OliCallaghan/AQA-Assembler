// Import Native NodeJS Libraries
const path = require('path');

// Import Electron Dependencies
const {ipcRenderer} = require('electron');
const {remote} = require('electron');
const {Menu, MenuItem} = remote;

// Working File
var wrking_file;

// Prompt user for file to open on launch
// ipcRenderer.send('selectfile', '');

// Create new empty file without saving
document.getElementById('filename').innerHTML = 'untitled';
document.getElementById('code').value = '';

// User selected file
ipcRenderer.on('selectedfile', (event, arg) => {
    document.getElementById('filename').innerHTML = path.basename(arg[0]);
    wrking_file = arg[0];
    ipcRenderer.send('open', arg[0]);
});

// User opened file
ipcRenderer.on('opened', (event, arg) => {
    // Set textarea contents to opened file
    document.getElementById('code').value = arg;
});

// Real-time register data recieved
ipcRenderer.on('registers', (event, arg) => {
    // Update GUI with this data
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
    if (!wrking_file) {
        savefilefirsttime();

        ipcRenderer.send('run', wrking_file);
    } else {
        if (ipcRenderer.sendSync('save', {
                path: wrking_file,
                data: document.getElementById('code').value
            })) {
            // Run file
            ipcRenderer.send('run', wrking_file);
        }
    }
}

// Prompt user for filename when saving
function savefilefirsttime() {
    var savenewfile = ipcRenderer.sendSync('savenewfile', document.getElementById('code').value);
    if (savenewfile.err == undefined) {
        wrking_file = savenewfile.path;
        document.getElementById('filename').innerHTML = path.basename(wrking_file);
    }
}

function save() {
    ipcRenderer.sendSync('save', {
            path: wrking_file,
            data: document.getElementById('code').value
        });
}

function saveas() {
    savefilefirsttime();
}

function open() {
    ipcRenderer.send('selectfile', '');
}

function newfile() {
    document.getElementById('code').value = '';
    document.getElementById('filename').innerHTML = 'untitled';
    wrking_file = undefined;
}

// Configure application menu
const template = [
    {
      label: 'File',
      submenu: [
        {
            label: 'New File',
            accelerator: 'CmdOrCtrl+N',
            click () { newfile(); }
        },
        {
          type: 'separator'
        },
        {
            label: 'Open...',
            accelerator: 'CmdOrCtrl+O',
            click () { open(); }
        },
        {
            label: 'Save',
            accelerator: 'CmdOrCtrl+S',
          click () { console.log(wrking_file); if (wrking_file == undefined) { saveas(); } else {save();} }
        },
        {
            label: 'Save As...',
            accelerator: 'Shift+CmdOrCtrl+S',
          click () { saveas(); }
        },
        {
          type: 'separator'
        },
        {
            label: 'Close File',
            accelerator: 'CmdOrCtrl+W',
          role: 'close'
        }
      ]
    },
  {
    label: 'Edit',
    submenu: [
      {
          label: 'Undo',
          accelerator: 'CmdOrCtrl+Z',
        role: 'undo'
      },
      {
          label: 'Redo',
          accelerator: 'Shift+CmdOrCtrl+Z',
        role: 'redo'
      },
      {
        type: 'separator'
      },
      {
          label: 'Cut',
          accelerator: 'CmdOrCtrl+X',
        role: 'cut'
      },
      {
          label: 'Copy',
          accelerator: 'CmdOrCtrl+C',
        role: 'copy'
      },
      {
          label: 'Paste',
          accelerator: 'CmdOrCtrl+V',
        role: 'paste'
      },
      {
          label: 'Select All',
          accelerator: 'CmdOrCtrl+A',
        role: 'selectall'
      }
    ]
  },
  {
    label: 'View',
    submenu: [
      {
          label: 'Reload',
        role: 'reload'
      },
      {
          label: 'Force Reload',
        role: 'forcereload'
      },
      {
          label: 'Toggle Developer Tools',
        role: 'toggledevtools'
      },
      {
        type: 'separator'
      },
      {
          label: 'Reset Zoom',
        role: 'resetzoom'
      },
      {
          label: 'Zoom In',
        role: 'zoomin'
      },
      {
          label: 'Zoom Out',
        role: 'zoomout'
      },
      {
        type: 'separator'
      },
      {
          label: 'Toggle Fullscreen',
          accelerator: 'Alt+CmdOrCtrl+F',
        role: 'togglefullscreen'
      }
    ]
  },
  {
      label: 'Window',
    role: 'window',
    submenu: [
      {
          label: 'Minimize',
        role: 'minimize'
      },
      {
          label: 'Close',
        role: 'close'
      }
    ]
  },
  {
      label: 'Help',
    role: 'help',
    submenu: [
      {
        label: 'Learn More',
        click () { require('electron').shell.openExternal('http://crgs.co.uk') }
      }
    ]
  }
]

if (process.platform === 'darwin') {
  template.unshift({
    label: 'AQA',
    submenu: [
      {
          label: 'About',
        role: 'about'
      },
      {
        type: 'separator'
      },
      {
          label: 'Services',
        role: 'services',
        submenu: []
      },
      {
        type: 'separator'
      },
      {
          label: 'Hide',
        role: 'hide'
      },
      {
          label: 'Hide Others',
        role: 'hideothers'
      },
      {
          label: 'Unhide',
        role: 'unhide'
      },
      {
        type: 'separator'
      },
      {
          label: 'Quit',
          accelerator: 'CmdOrCtrl+Q',
        role: 'quit'
      }
    ]
  })
  // Edit menu.
  template[1].submenu.push(
    {
      type: 'separator'
    },
    {
      label: 'Speech',
      submenu: [
        {
          role: 'startspeaking'
        },
        {
          role: 'stopspeaking'
        }
      ]
    }
  )
  // Window menu.
  template[3].submenu = [
    {
      label: 'Close',
      accelerator: 'CmdOrCtrl+W',
      role: 'close'
    },
    {
      label: 'Minimize',
      accelerator: 'CmdOrCtrl+M',
      role: 'minimize'
    },
    {
      label: 'Maximise',
      role: 'zoom'
    },
    {
      type: 'separator'
    },
    {
      label: 'Bring All to Front',
      role: 'front'
    }
  ]
}

const menu = Menu.buildFromTemplate(template)
Menu.setApplicationMenu(menu)

window.addEventListener('contextmenu', (e) => {
  e.preventDefault()
  menu.popup(remote.getCurrentWindow())
}, false)
