#pragma once

const auto html_page = R"!(
<!DOCTYPE html>
<html>
<head>
  <title>Universal WiFi Manager</title>
  <style>
    body {
      background: linear-gradient(to bottom, #808080, #c0c0c0);
      height: 100vh;
      background-size: cover;
      background-repeat: no-repeat;
      text-align: center;
      padding-top: 100px;
    }

    input {
      padding: 10px;
      margin: 5px;
    }

    button {
      padding: 10px 20px;
      margin-top: 10px;
    }
    .tile {
        width: 50vw;
        height: 10vh;
        border: 1px solid #729bd6;
        background-color: #808080;
        margin-left: auto;
        margin-right: auto;
        margin-bottom: 5px;
        display: block;
        transition: background-color 0.3s;
    }
    .tile:hover {
    background-color: #3498db;
}
    .tile.active {
        background-color: #3498db;
        color: #fff;
    }
  </style>
</head>
<body>
  <h1>Universal WiFi Manager</h1>
  <h2>Available APs</h2>
  <div id="APs">None</div>
    <button type="button" onclick="refresh()">Refresh</button>

  <form id="connection-form">
    <input type="text" id="ssid-input" placeholder="SSID" required><br>
    <input type="password" id="password-input" placeholder="Password" required><br>
    <button type="button" onclick="sendCredentials()">Connect</button>
  </form>

  <script>
    var socket;
    if(!socket || socket.readyState !== WebSocket.OPEN)
    {
      socket = new WebSocket("ws://4.3.2.1/ws");
    } 
    socket.addEventListener('message',handleApListMessage);
    function sendCredentials() {
      var ssid = document.getElementById("ssid-input").value;
      var password = document.getElementById("password-input").value;
      var url = "http://4.3.2.1/postCredentials";

      var xhr = new XMLHttpRequest();
      xhr.open("POST", url, true);
      xhr.setRequestHeader("Content-Type", "application/json");
      xhr.setRequestHeader('Access-Control-Allow-Headers', '*');
      xhr.setRequestHeader('Access-Control-Allow-Origin', '*');
      var data = JSON.stringify({ "ssid": ssid, "password": password });
      xhr.send(data);
    }

    function handleTileClick(tile) {
        var tiles = document.querySelectorAll('.tile');

        tiles.forEach(function (tile) {
            tile.classList.remove('active');
        });

        tile.classList.add('active');
        document.getElementById("ssid-input").value=tile.innerHTML;
    }

    function refresh()
    {
      socket.send("ss");
      apsList = document.getElementById("APs");
      apsList.innerHTML= '';
    }

    function handleApListMessage(event)
    {
        console.log(event.data);
        apsList = document.getElementById("APs");
        jsonObject = JSON.parse(event.data);
        for (var key in jsonObject) {
        if (jsonObject.hasOwnProperty(key)) {
          var newDiv = document.createElement('div');
          newDiv.className = 'tile';
          newDiv.textContent = key;
          newDiv.onclick = function() {handleTileClick(this);};
          apsList.appendChild(newDiv);
        }
      }
    }
  </script>
</body>
</html>
)!";