// Creates a new Socket.IO client
// Arguments:
// - transports: array of transport methods (e.g., ["websocket"], ["polling"])
//   "websocket" = full-duplex, low-latency
//   "polling" = fallback HTTP-based transport
//   GET ws://localhost:5050/socket.io/?userId=abc&EIO=4&transport=websocket
const socket = io({
    // "localhost:5050",
    transports: ["websocket"],
    // path: "/socket/io",
    // query: {userId: "abc"},
    // reconnection: true,
    // reconnectionAttempts: 2,
    // reconnectionDelay: 100,
    // timeout: 1000,
    // autoConnect: false,
    // forceNew: true,
});

console.log("Connecting to server...");

// Registers a listener for the 'connect' event
// Arguments:
// - event: "connect"
// - callback: function to call when connected
socket.on('connect', handleConnectionEstablished);

// Registers a listener for 'message' event
// Arguments:
// - event: "message"
// - callback: function to handle incoming message data
socket.on('message', displayIncomingMessage);

// Sends a chat message to the server
// Arguments: none
// Relies on DOM input fields to retrieve username and message text
function sendMessage() {
    const usernameInput = document.getElementById("username");
    const messageInput = document.getElementById("message");

    const name = usernameInput.value.trim();
    const message = messageInput.value.trim();

    if (name && message) {
        socket.send(`${name}: ${message}`);
        messageInput.value = "";
    }
}

// Callback for when WebSocket connection is established
// Arguments: none
function handleConnectionEstablished() {
    console.log("WebSocket connected");
}

// Handles incoming messages from the server
// Arguments:
// - msg: the message string sent from the server
function displayIncomingMessage(msg) {
    console.log("Received:", msg);

    const messageList = document.getElementById("messages");
    const li = document.createElement("li");
    li.innerText = msg;
    messageList.appendChild(li);
}

// Called when a key is pressed in the message input field
// Arguments:
// - event: a KeyboardEvent object. We check `event.key === "Enter"`
function handleEnterKeyPress(event) {
    if (event.key === "Enter") {
        sendMessage();
    }
}

// Attaches the keypress event listener once the DOM is ready
// Arguments: none
function setupEventListeners() {
    const messageInput = document.getElementById("message");
    messageInput.addEventListener("keypress", handleEnterKeyPress);
}

// Fires when the initial HTML document is fully loaded and parsed
// Arguments:
// - event: implicitly handled by DOMContentLoaded
document.addEventListener("DOMContentLoaded", setupEventListeners);