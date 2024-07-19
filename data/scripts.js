/**
 * Indicates whether the user has changed something in the sensor config textarea
 */
let sensorConfigChangedFlag = false;

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

function setSensorConfigChangedFlag(){
    sensorConfigChangedFlag = true;
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

function sleep(ms) {
    return new Promise(resolve => setTimeout(resolve, ms));
  }

function getEventLog() {
    let http = new XMLHttpRequest();
    http.onreadystatechange = function () {
        if (this.readyState == 4 && this.status == 200) {
            let log = "";
            let logJson = JSON.parse(this.responseText);
            for(let i = 0; i < logJson.length; i++){
                let logEntry = logJson[i];
                log += logEntry["time"] + ": " + logEntry["msg"] + "<br/>";
            }

            document.getElementById("log").innerHTML = log;
            document.getElementById('spinner').style.display = "none";
            document.getElementById('log').style.display = "block";
        }
    };
    http.open("GET", "api/log?from=-200&to=-1", true);
    http.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
    http.send();
}

function getConfig() {
    // sensor settings
    let http1 = new XMLHttpRequest();
    http1.onreadystatechange = function () {
        if (this.readyState == 4 && this.status == 200) {
            document.getElementById("sensor_config").value = this.responseText;
        }
    }
    http1.open("GET", "api/file?sensorConfig", true);
    http1.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
    http1.send();

    // general settings
    let http2 = new XMLHttpRequest();
    http2.onreadystatechange = function () {
        if (this.readyState == 4 && this.status == 200) {

            document.getElementById('spinner').style.display = "none";
            document.getElementById('config').style.display = "block";
            let jsonData = JSON.parse(this.responseText);
            document.getElementById("ssid").value = jsonData["ssid"];
            document.getElementById("hostname").value = jsonData["hostname"];
            document.getElementById("mqtt_broker_address").value = jsonData["brokerAddress"];
            document.getElementById("mqtt_broker_port").value = jsonData["brokerPort"];
            document.getElementById("mqtt_broker_username").value = jsonData["username"];
            document.getElementById("mqtt_broker_client_id").value = jsonData["clientID"];
            document.getElementById("mqtt_devicetopic").value = jsonData["deviceTopic"];
        }
    };
    http2.open("GET", "api/system?getConfig", true);
    http2.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
    http2.send();
}

function submitSettingsForm() {
    let bodyContent = "ssid=" + document.getElementById("ssid").value;
    let wifiPassword = document.getElementById("password").value;
    if(wifiPassword != ""){
        bodyContent += "wifiPassword=" + wifiPassword;
    }

    bodyContent += "&hostname=" + document.getElementById("hostname").value
        + "&brokerAddress=" + document.getElementById("mqtt_broker_address").value
        + "&brokerPort=" + document.getElementById("mqtt_broker_port").value
        + "&username=" + document.getElementById("mqtt_broker_username").value;
    let mqttPassword = document.getElementById("mqtt_broker_password").value
    if(mqttPassword != ""){
        bodyContent += "&mqttPassword=" + mqttPassword;
    }
    bodyContent += "&clientID=" + document.getElementById("mqtt_broker_client_id").value
        + "&deviceTopic=" + document.getElementById("mqtt_devicetopic").value;
    if(sensorConfigChangedFlag){
        bodyContent += "&sensorConfig=" + document.getElementById("sensor_config").value;
    }

    fetch("/api/system", {
        method: "post",
        headers: {
            'Content-Type': 'application/x-www-form-urlencoded'
        },

        body: bodyContent
    })

    toast("Settings saved");
    // reset sensor config changed flag
    sensorConfigChangedFlag = false;
    sleep(3000).then(() => getConfig());
}