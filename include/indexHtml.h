#pragma once

const auto html_page = R"!(
<!DOCTYPE html>
<html>
<head>
  <title>Hydroponika</title>
  <style>
    body {
      background-image: url("https://apteka-melissa.pl/blog_zdjecia/medyczna-marihuana-koniec-tabu-rozwiewamy-watpliwosci-i-obawy.jpg");
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
  </style>
</head>
<body>
  <h1>Hydroponika</h1>
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
  </script>
</body>
</html>
)!";