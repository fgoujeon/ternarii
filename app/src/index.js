/*
Copyright 2018 - 2019 Florian Goujeon

This file is part of Ternarii.

Ternarii is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Ternarii is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Ternarii.  If not, see <https://www.gnu.org/licenses/>.
*/

"use strict";

var Module = {
    preRun: [],
    postRun: [],

    arguments: [],

    printErr: function(message) {
        console.error(Array.prototype.slice.call(arguments).join(' '));
    },

    print: function(message) {
        console.log(Array.prototype.slice.call(arguments).join(' '));
    },

    onAbort: function() {
        Module.canvas.style.opacity = 0.333;
        Module.canvas.style.zIndex = -1;
        Module.setStatus("Oops :(");
        Module.setStatusDescription("The app crashed. Please refresh the page.");
    },

    canvas: document.getElementById('module'),

    setStatus: function(message) {
        var status = document.getElementById('status');
        /* Emscripten calls setStatus("") after a timeout even if the app
           aborts. That would erase the crash message, so don't allow that */
        if(status && status.innerHTML != "Oops :(")
            status.innerHTML = message;
    },

    setStatusDescription: function(message) {
        var statusDescription = document.getElementById('status-description');
        if(statusDescription) statusDescription.innerHTML = message;
    },

    totalDependencies: 0,

    monitorRunDependencies: function(left) {
        this.totalDependencies = Math.max(this.totalDependencies, left);

        if(left) {
            Module.setStatus('Downloading...');
            Module.setStatusDescription((this.totalDependencies - left) + ' / ' + this.totalDependencies);
        } else {
            Module.setStatus('Download complete');
            Module.setStatusDescription('');
        }
    }
};

Module.setStatus('Downloading...');
