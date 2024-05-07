/**
 * Sends a restart command to the controller
 */
function restartDevice() {
    fetch("/api/system", {
        method: "post",
        headers: {
            'Content-Type': 'application/x-www-form-urlencoded'
        },

        body: "restart=true"
    })

    clearLogMessages();
    toast("Restarting device");
}

/**
 * Sends a toast message
 * @param {string} msg 
 */
function toast(msg) {
    let snackbar = document.getElementById("snackbar");
    snackbar.textContent = msg;
    snackbar.className = "show"
    setTimeout(function () { snackbar.className = snackbar.className.replace("show", ""); }, 3000);
}

function getEventLog() {
    var http = new XMLHttpRequest();
    http.onreadystatechange = function () {
        if (this.readyState == 4 && this.status == 200) {
            var log = this.responseText;
            log = log.replace(/(?:\r\n|\r|\n)/g, '<br />');

            document.getElementById("log").innerHTML = log;
            document.getElementById('spinner').style.display = "none";
            document.getElementById('log').style.display = "block";
        }
    };
    http.open("POST", "api", true);
    http.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
    http.send("cmd=eventlog");
}

function getConfig() {
    var http = new XMLHttpRequest();
    http.onreadystatechange = function () {
        if (this.readyState == 4 && this.status == 200) {
            
            document.getElementById('spinner').style.display = "none";
            document.getElementById('config').style.display = "block";
            let jsonData = JSON.parse(this.responseText);
            document.getElementById("ssid").value = jsonData["ssid"];
            document.getElementById("mqtt_broker_address").value = jsonData["brokerAddress"];
            document.getElementById("mqtt_broker_port").value = jsonData["brokerPort"];
            document.getElementById("mqtt_broker_username").value = jsonData["username"];
            document.getElementById("mqtt_broker_client_id").value = jsonData["clientID"];
            document.getElementById("mqtt_devicetopic").value = jsonData["deviceTopic"];
        }
    };
    http.open("GET", "api/system?getConfig", true);
    http.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
    http.send();
}