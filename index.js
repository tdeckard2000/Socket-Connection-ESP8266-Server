import express from 'express';
import path from 'path';
import { fileURLToPath } from 'url';
import { dirname } from 'path';
import net from 'net';

const __filename = fileURLToPath(import.meta.url);
const __dirname = dirname(__filename);
const app = express();
const serverPort = process.env.PORT || 3000;
const TCPServerPort = process.env.TCP_PORT || 4000;

const TCPServer = net.createServer(socket => {
    handleClientConnected(socket);
    socket.on('data', data => {
        handleTcpData(socket, data);
    });
    socket.on('error', (err) => {
        console.log("Socket error: ", err);
    });
    socket.on('end', (info) => {
        console.log('client disconnected: ', info);
    });
});

app.use(express.static(path.join(__dirname, 'dist')));

app.get('/', (req, res) => {
    res.sendFile(path.join(__dirname, 'dist/index.html'));
});

app.get('/connect', (req, res) => {
    console.log('req -- ', req)
    res.send('Hello, World!');
});

app.listen(serverPort, () => {
    console.log(`Server listening on port ${serverPort}`);
});

TCPServer.listen(TCPServerPort, () => {
    console.log(`TCP Server listening on port ${TCPServerPort}`);
});

const handleClientConnected = (socket) => {
    socket.write('Hello from server', (err) => {
        if (err) {
            console.log("Error while writing: ", err);
        } else {
            console.log("Wrote successfully");
        }
    });
}

const handleTcpData = (socket, data) => {
    try {
        console.log(data.toString());
    } catch (err) {
        console.log(err);
    }
}