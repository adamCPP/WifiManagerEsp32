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
      padding-top: 150px;
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
        border: 1px solid #ccc;
        margin-left: auto;
        margin-right: auto;
        margin-bottom: 5px;
        display: block
        cursor: pointer;
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
    <div class="tile" onclick="handleTileClick(this)">Kafelek 1</div>
    <div class="tile" onclick="handleTileClick(this)">Kafelek 2</div>
    <div class="tile" onclick="handleTileClick(this)">Kafelek 3</div>

  <form id="connection-form">
    <input type="text" id="ssid-input" placeholder="SSID" required><br>
    <input type="password" id="password-input" placeholder="Password" required><br>
    <button type="button" onclick="sendCredentials()">Connect</button>
  </form>

  <script>
    function sendCredentials() {
      console.log("sendCredentials odpalone.");
      var ssid = document.getElementById("ssid-input").value;
      var password = document.getElementById("password-input").value;
      var url = "http://4.3.2.1/postCredentials"; // Adres docelowy

      // Utworzenie obiektu XMLHttpRequest
      var xhr = new XMLHttpRequest();
      xhr.open("POST", url, true);
      xhr.setRequestHeader("Content-Type", "application/json");
      xhr.setRequestHeader('Access-Control-Allow-Headers', '*');
      xhr.setRequestHeader('Access-Control-Allow-Origin', '*');

      // Dane do wysłania
      var data = JSON.stringify({ "ssid": ssid, "password": password });

      // Obsługa odpowiedzi
      xhr.onreadystatechange = function () {
        if (xhr.readyState === 4 && xhr.status === 200) {
          console.log("Dane zostały wysłane pomyślnie.");
        }
      };

      // Wysłanie żądania
      xhr.send(data);
    }

     function handleTileClick(tile) {
            // Wszystkie kafelki
            var tiles = document.querySelectorAll('.tile');

            // Usunięcie aktywnego stanu ze wszystkich kafelków
            tiles.forEach(function (tile) {
                tile.classList.remove('active');
            });

            // Dodanie aktywnego stanu tylko do klikniętego kafelka
            tile.classList.add('active');
        }
  </script>
</body>
</html>
)!";