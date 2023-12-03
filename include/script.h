#pragma once

const auto script = R"!(
var socketAPs;
if(!socketAPs || socketAPs.readyState !== WebSocket.OPEN)
{
    socketAPs = new WebSocket("ws://4.3.2.1/ws");
} 
socketAPs.addEventListener('message',handleApListMessage);
socketAPs.addEventListener('open', function (event) {
refreshAps();});
var socketCustmoParams;
if(!socketCustmoParams || socketCustmoParams.readyState !== WebSocket.OPEN)
{
    socketCustmoParams = new WebSocket("ws://4.3.2.1/cp");
} 
socketCustmoParams.addEventListener('message',handleCustomParamsMessage);
socketCustmoParams.addEventListener('open', function (event) {
    refreshCustomParams();});
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

function refreshAps()
{
    socketAPs.send("ss");
    apsList = document.getElementById("APs");
    apsList.innerHTML= '';
}

function refreshCustomParams()
{
    socketCustmoParams.send("ping");

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
function handleCustomParamsMessage(event)
{
    console.log(event.data);
} 
)!";