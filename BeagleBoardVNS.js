// From Getting Started With node.js and socket.io 
// http://codehenge.net/blog/2011/12/getting-started-with-node-js-and-socket-io-v0-7-part-2/
"use strict";

var http = require('http'),
    url = require('url'),
    fs = require('fs'),
    exec = require('child_process').exec,
    server,
    connectCount = 0;	// Number of connections to server

server = http.createServer(function (req, res) {
// server code
    var path = url.parse(req.url).pathname;
    console.log("path: " + path);
    switch (path) {
    case '/':
        res.writeHead(200, {'Content-Type': 'text/html'});
        res.write('<h1>Hello!</h1>Try<ul><li><a href="/BeagleBoardVNS.html">BeagleBoard Voice Notification System Preferences</a></li></ul>');

        res.end();
        break;

    default:		// This is so all the files will be sent.
        fs.readFile(__dirname + path, function (err, data) {
            if (err) {return send404(res); }
//            console.log("path2: " + path);
            res.write(data, 'utf8');
            res.end();
        });
        break;

    }
});

var send404 = function (res) {
    res.writeHead(404);
    res.write('404');
    res.end();
};

server.listen(8081);

// socket.io, I choose you
var io = require('socket.io').listen(server);
io.set('log level', 2);

// on a 'connection' event
io.sockets.on('connection', function (socket) {
    console.log("Connection " + socket.id + " accepted.");

    // now that we have our connected 'socket' object, we can 
    // define its event handlers

    socket.on('postPreferences', function (sendTo, sendFrom, password, subject, message) {
        fs.writeFile("PREFERENCES.py", "SEND_TO= '" + sendTo + "'\nSEND_FROM= '" + sendFrom + "'\nPASSWORD= '" + password + "'\nSUBJECT= '" + subject + "'\nMESSAGE= '" + message + "'", function(err, 
data) 
{
            if (err) throw err;
//            socket.emit('gpio', data);
            console.log("Send To: " + sendTo + "Send From: " + sendFrom + "Password: " + password + "Subject: " + subject + "Message: " + message);
        });
    });

    socket.on('getPreferences', function (gpioNum) {
        var gpioPath = "/sys/class/gpio/gpio" + gpioNum + "/value";
        fs.readFile(gpioPath, 'base64', function(err, data) {
            if (err) throw err;
            socket.emit('gpio', data);
//            console.log('emitted gpio: ' + data);
        });
    });

    socket.on('disconnect', function () {
        console.log("Connection " + socket.id + " terminated.");
        connectCount--;
        if(connectCount === 0) {
        }
        console.log("connectCount = " + connectCount);
    });

    connectCount++;
    console.log("connectCount = " + connectCount);
});

