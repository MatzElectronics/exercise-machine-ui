// Import required libraries
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

// Replace with your network credentials
const char* ssid = "Nacho wireless";
const char* password = "P3pp3rj@ck";

bool state_led = 0;
bool state_tire = 1;
bool state_ekg = 0;
bool state_beep = 0;
bool state_btn_1 = 1;
bool state_btn_2 = 1;
bool state_btn_3 = 1;
bool state_btn_4 = 1;

#define PIN_LED          2
#define PIN_TIRE        33
#define PIN_EKG         36
#define PIN_POT1        34  // Resistance Feedback
#define PIN_POT2        35
#define PIN_BEEP        14
#define PIN_BUTTON_1     4
#define PIN_BUTTON_2    32
#define PIN_BUTTON_3    25
#define PIN_BUTTON_4    27
#define PIN_MOTOR_1     13
#define PIN_MOTOR_2     16  //26

int tire_debounce = 0;
int btn_1_debounce = 0;
int btn_2_debounce = 0;
int btn_3_debounce = 0;
int btn_4_debounce = 0;


int motor_move_ticks = 0;

int beep_count = 0;

int minimum_resistance = 188;
int resistance_goal = 500;
int last_res = resistance_goal;

uint8_t resistance_target = (uint8_t) map(resistance_goal, 1000, 0, 6, minimum_resistance);
int current_resistance = (analogRead(PIN_POT1) >> 4);

int info_tick = 0;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

/*
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>ESP Web Server</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="icon" href="data:,">
  <style>
  html {
    font-family: Arial, Helvetica, sans-serif;
    text-align: center;
  }
  h1 {
    font-size: 1.8rem;
    color: white;
  }
  h2{
    font-size: 1.5rem;
    font-weight: bold;
    color: #143642;
  }
  .topnav {
    overflow: hidden;
    background-color: #143642;
  }
  body {
    margin: 0;
  }
  .content {
    padding: 30px;
    max-width: 600px;
    margin: 0 auto;
  }
  .card {
    background-color: #F8F7F9;;
    box-shadow: 2px 2px 12px 1px rgba(140,140,140,.5);
    padding-top:10px;
    padding-bottom:20px;
  }
  .button {
    padding: 15px 50px;
    font-size: 24px;
    text-align: center;
    outline: none;
    color: #fff;
    background-color: #0f8b8d;
    border: none;
    border-radius: 5px;
    user-select: none;
   }
   .button:active {
     background-color: #0f8b8d;
     box-shadow: 2 2px #CDCDCD;
     transform: translateY(2px);
   }
   .state {
     font-size: 1.5rem;
     color:#8c8c8c;
     font-weight: bold;
   }
  </style>
<title>ESP Web Server</title>
<meta name="viewport" content="width=device-width, initial-scale=1">
<link rel="icon" href="data:,">
</head>
<body>
  <div class="topnav">
    <h1>ESP WebSocket Server</h1>
  </div>
  <div class="content">
    <div class="card">
      <h2>Output - GPIO 2</h2>
      <p class="state">state: <span id="state">%STATE%</span></p>
      <p><button id="button" class="button">Toggle</button></p>
      <p class="state">mark: <span id="mark">%MARK%</span></p>
    </div>
  </div>
<script>
  var gateway = `ws://${window.location.hostname}/ws`;
  var websocket;
  window.addEventListener('load', onLoad);
  function initWebSocket() {
    console.log('Trying to open a WebSocket connection...');
    websocket = new WebSocket(gateway);
    websocket.onopen    = onOpen;
    websocket.onclose   = onClose;
    websocket.onmessage = onMessage; // <-- add this line
  }
  function onOpen(event) {
    console.log('Connection opened');
  }
  function onClose(event) {
    console.log('Connection closed');
    setTimeout(initWebSocket, 2000);
  }
  function onMessage(event) {
    var data = JSON.parse(event.data);
    document.getElementById('state').innerHTML = data.state;
    document.getElementById('mark').innerHTML = data.mark;
  }
  function onLoad(event) {
    initWebSocket();
    initButton();
  }
  function initButton() {
    document.getElementById('button').addEventListener('click', toggle);
  }
  function toggle(){
    websocket.send('toggle');
  }
</script>
</body>
</html>
)rawliteral";
*/

const char index_html[] PROGMEM = R"rawliteral(
<!doctype html><html><head><title>Exercise Machine</title><link rel=preconnect href=https://fonts.googleapis.com><link rel=preconnect href=https://fonts.gstatic.com crossorigin><link href="https://fonts.googleapis.com/css2?family=Exo:ital,wght@0,400;0,700;1,400;1,700&display=swap" rel=stylesheet><style>

body {
    font-family: 'Exo', sans-serif;
    background-color: black;
    color: white;
    overflow: hidden;
}
button {
    width: 100%%;
    height: 100%%;
    background: rgba(0,0,0,0);
    border: none;
    color: #fff;
    font-weight: 700;
    font-family: 'Exo', sans-serif;
    font-size: 4vh;
    padding: 0;
}
.hidden {
    display: none;
}
p {
    margin: 1vh;
}
.right {
    text-align: right;
    padding-right: 2vh;
}
.center {
    text-align: center;
}
svg.center {
    margin: auto;
}
.number {
    font-size: 14vh;
    font-weight: 700;
}
.mini {
    font-size: 10vh;
    font-weight: 700;
}
.mini>.mini{
    font-size: 6vh;
    font-weight: 700;
}
.unit {
    font-size: 6vh;
    font-weight: 400;
}
.heart {
    width: 8vw;
    transition: all 0.15s ease-in-out;
}
.heart.beat {
    width: 10vw;
}
.machine, .track {
    font-size: 5vh;
    color: rgba(255,255,255,0.5);
    text-transform: uppercase;
    font-style: italic;
    margin-left: 4vw;
}
.track {
    font-size: 9vh;
}
.track-container {
    justify-content: flex-start;
}
.arrow-horizontal {
    height: 10vh;
}
.arrow-vertical {
    width: 10vh;
}
.timer-control>svg {
    height: 12vh;
}
button.text-button {
    width: unset;
    height: unset;
    border: 3px solid red;
    border-radius: 2vh;
    padding: 1vh;
    margin: 2vh 0.5vh;
}
#slope-profile {
    transform-origin: 0 100%%;
    transform: scaleY(1);
}
.parent {
    display: grid;
    grid-template-columns: repeat(2, .75fr) .5fr repeat(3, 1fr) .5fr;
    grid-template-rows: repeat(2, .375fr) .5fr repeat(2, .75fr);
    grid-column-gap: 0px;
    grid-row-gap: 0px;
    width: 100vw;
    height: 100vh;
}
.parent>div {
    display: flex;
    align-content: center;
    justify-content: center;
    flex-direction: column;
}
.parent>div.horizontal {
    flex-direction: row;
    justify-content: space-around;
}
.parent>div.horizontal.left {
    flex-direction: row;
    justify-content: flex-start;
}
.parent>div>div.vertical {
    display: flex;
    flex-direction: column;
    align-items: center;
    justify-content: center;
}
.padded {
    padding: 10px;
}
.padded>svg {
    width: 100%%;
    height: 100%%;
}
.div1 { grid-area: 1 / 1 / 2 / 2; }
.div2 { grid-area: 1 / 2 / 2 / 3; }
.div3 { grid-area: 2 / 1 / 3 / 3; }
.div4 { grid-area: 3 / 1 / 4 / 3; }
.div5 { grid-area: 3 / 2 / 4 / 3; }
.div6 { grid-area: 4 / 1 / 5 / 3; }
.div7 { grid-area: 5 / 1 / 6 / 2; }
.div8 { grid-area: 5 / 2 / 6 / 3; }
.div9 { grid-area: 1 / 3 / 5 / 8; }
.div10 { grid-area: 3 / 3 / 4 / 4; }
.div11 { grid-area: 3 / 7 / 4 / 8; }
.div12 { grid-area: 5 / 4 / 6 / 7; }
.div13 { grid-area: 5 / 7 / 6 / 8; }
.div14 { grid-area: 1 / 3 / 2 / 6; }
.div15 { grid-area: 1 / 6 / 2 / 8; }
.div16 { grid-area: 4 / 6 / 5 / 8; }
.div18 { grid-area: 5 / 3 / 6 / 4; }

</style><script src=https://cdnjs.cloudflare.com/ajax/libs/three.js/0.149.0/three.min.js></script><script src=https://cdn.jsdelivr.net/npm/three@0.137.2/examples/js/controls/OrbitControls.js></script><script src="https://cdnjs.cloudflare.com/ajax/libs/nosleep/0.3.0/NoSleep.min.js"></script></head><body><div class=parent><div id=heart-icon class=div1><svg id=heart-icon-svg viewBox="0 0 100.8 92.7" class="heart center"><path id=path-heart fill=red /></svg></div><div class=div2><p class="center unit">BPM</p><p id=heartrate class="number center">0</p></div><div class=div3><svg viewBox="0 0 500 96"><path id=bpm-profile fill=rgba(255,255,255,0.6) stroke=none d="M0 95 0 85h500v1095z"/><path fill=none stroke=rgba(255,255,255,0.6) stroke-width=2 d="M1 95h500"/><path fill=none stroke=rgba(255,0,0,0.6) stroke-width=2 d="M1 1h500"/><path fill=none stroke=rgba(255,0,0,0.6) stroke-width=2 d="M1 24h500" stroke-dasharray="5 5"/><path fill=none stroke=rgba(255,255,255,0.6) stroke-width=2 d="M1 71h500 M1 48h500" stroke-dasharray="5 5"/></svg></div><div class="div4 horizontal"><button id=button-setup class="text-button running">SETUP</button></div><div id=timer-output class="div6 horizontal"><button id=button-timer-down class=setup><svg class=arrow-horizontal viewBox="0 0 85 100"><path fill=red fill-rule=evenodd d="M85 100V0L0 50z"/></svg></button><p id=timer-display class="number center running">00:00<span class=mini>.0</span></p><p id=timer-setting class="number center setup">00</p><button id=button-timer-up class=setup><svg class=arrow-horizontal viewBox="0 0 85 100"><path fill=red fill-rule=evenodd d="M0 100V0l85 50z"/></svg></button></div><div class=div7><button id=button-timer-play class="timer-control running"><svg viewBox="0 0 27 27"><path id=path-play fill=red fill-rule=evenodd /></svg></button><button id=button-timer-pause class="timer-control running hidden"><svg viewBox="0 0 26.5 26.5"><path id=path-pause fill=red fill-rule=evenodd /></svg></button><button id=button-timer-check class="timer-control setup"><svg viewBox="0 0 26.5 26.5"><path id=path-check fill=red fill-rule=evenodd /></svg></button></div><div class=div8><button id=button-timer-reset class="timer-control running"><svg viewBox="0 0 26.5 26.5"><path id=path-reset-1 fill=red fill-rule=evenodd /></svg></button></div><div class=div9 id=track-display></div><div class=div10><button id=button-track-prev><svg class="arrow-horizontal setup" viewBox="0 0 85 100"><path fill=red fill-rule=evenodd d="M85 100V0L0 50z"/></svg></button></div><div class=div11><button id=button-track-next><svg class="arrow-horizontal setup" viewBox="0 0 85 100"><path fill=red fill-rule=evenodd d="M0 100V0l85 50z"/></svg></button></div><div class="div12 horizontal left"><div class=vertical><button id=button-resistance-up><svg class=arrow-vertical viewBox="0 0 100 85"><path fill=red fill-rule=evenodd d="M100 85H0L50 0z"/></svg></button><button id=button-resistance-down><svg class=arrow-vertical viewBox="0 0 100 85"><path fill=red fill-rule=evenodd d="M100 0H0L50 85z"/></svg></button></div><div class="vertical padded"><svg viewBox="0 0 588 96"><path id=slope-profile fill=rgba(255,255,255,0.6) stroke=none d="M0 95 0 85h588v1095z"/><path fill=none stroke=rgba(255,255,255,0.6) stroke-width=1 d="M1 95h588 M1 1h588"/><path fill=none stroke=rgba(255,255,255,0.6) stroke-width=1 d="M1 24h588 M1 48h588 M1 71h588" stroke-dasharray="5 5"/><path id=slope-line fill=none stroke=red stroke-width=2 d="M1 0v95"/></svg></div></div><div class=div13><button id=button-distance-up class=setup><svg class=arrow-vertical viewBox="0 0 100 85"><path fill=red fill-rule=evenodd d="M100 85H0L50 0z"/></svg></button><button id=button-distance-down class=setup><svg class=arrow-vertical viewBox="0 0 100 85"><path fill=red fill-rule=evenodd d="M100 0H0L50 85z"/></svg></button><button id=button-distance-reset class="timer-control running"><svg viewBox="0 0 26.5 26.5"><path id=path-reset-2 fill=red fill-rule=evenodd /></svg></button></div><div class=div14 class=track-container><p id=app-title class=machine></p><p id=track-title class=track></p></div><div class=div15><p id=speed-display class="right number running">0<span class=mini>.0</span><span class=unit> mph</span></p></div><div class=div16><p id=distance-display class="right number running">0<span class=mini>.00</span><span class=unit> mi</span></p><p id=distance-setting class="right number setup">1<span class=unit> mi</span></p></div><div class=div18><p id=resistance-display class="mini center running">5</p><p id=resistance-setting class="mini center setup">5</p></div></div><script>

/*
 * Prevent sleping
 */
let noSleep = new NoSleep();

function enableNoSleep() {
  noSleep.enable();
  document.removeEventListener('touchstart', enableNoSleep, false);
}

// Enable wake lock.
// (must be wrapped in a user input event handler e.g. a mouse or touch handler)
document.addEventListener('touchstart', enableNoSleep, false);

/*
 * Set constants and values
 */
const scene = new THREE.Scene();

const tracks = [
    {
        name: "Gentle Ride",
        cameraStart: new THREE.Vector3(-190, 130, -450),
        track: [
            new THREE.Vector3(37, 0, -79),
            new THREE.Vector3(174, 10, -76),
            new THREE.Vector3(215, 20, 116),
            new THREE.Vector3(6, 30, 204),
            new THREE.Vector3(-193, 20, 158),
            new THREE.Vector3(-192, 10, -71),
            new THREE.Vector3(-81, 0, -154),
            new THREE.Vector3(37, 0, -79),
        ],
        curve: null,
        slope: null,
        min: null,
        max: null,
        path: null,
    },
    {
        name: "Crossover",
        cameraStart: new THREE.Vector3(-380, 210, 270),
        track: [
            new THREE.Vector3(-207, 0, -229),
            new THREE.Vector3(128, 44, -192),
            new THREE.Vector3(17, 98, 32),
            new THREE.Vector3(-215, 125, 210),
            new THREE.Vector3(100, 77, 206),
            new THREE.Vector3(-64, 37, 8),
            new THREE.Vector3(-207, 0, -229),
        ],
        curve: null,
        slope: null,
        min: null,
        max: null,
        path: null,
    },
    {
        name: "Rolling Hills",
        cameraStart: new THREE.Vector3(370, 80, 380),
        track: [
            new THREE.Vector3(258, 15, -40),
            new THREE.Vector3(51, 110, -384),
            new THREE.Vector3(-255, 0, -205),
            new THREE.Vector3(-436, 55, 221),
            new THREE.Vector3(-44, 88, 203),
            new THREE.Vector3(258, 15, -40),
        ],
        curve: null,
        slope: null,
        min: null,
        max: null,
        path: null,
    }
];

const milesPerRev = 0.00124 * 3;  // really no idea...I thought I could base this on a 24" tire...

let ui = {
    dist: 1,
    trav: 0,
    track: 0,
    setRes: 0.5,
    curRes: 1,
    lastRes: 1,
    trackObj: null,
    mark: Date.now,
    chipClock: -1,

    BPMs: [],
    curBPM: 0,
    beatMark: -1,
    revMark: -1,
    speed: 0,
    speedAvg: []
}

let tc = {
    running: false,
    dir: 1,
    start: 0,
    current: 0,
    mark: Date.now(),
    heartMark: 0
}

let tm = {
    curve: null,
    start: null, 
    ball: null, 
    grid: null, 
    rendr: null, 
    ctrls: null, 
    cam: null, 
    view: null
} 



/*
 * Utility Functions
 */
function GEI(e) {return document.getElementById(e);}
function QSA(e) {return document.querySelectorAll(e);}
const average = array => array.reduce((a, b) => a + b) / array.length;


/*
 * Set HTML, Path values, visibility
 */
let pa = 'M13.2 0A13.2 13.2 0 0 0 0 13.2a13.2 13.2 0 0 0 13.2 13.3 13.2 13.2 0 0 0 13.3-13.3A13.2 13.2 0 0 0 13.2 0z';
let pb = 'M9.1 5.4l4 4 4.2-4L21.1 9 17 13.3l4 4-3.7 3.9-4.1-4.2L9 21.2l-3.8-3.8 4.1-4.1-4-4.3z';

GEI('app-title').innerHTML = 'Recumbent Bike';
GEI('path-reset-1').setAttribute('d', pa + pb);
GEI('path-reset-2').setAttribute('d', pa + pb);
GEI('path-check').setAttribute('d', pa + 'm6 6.7 3.7 3.8-8.2 8.2-3.8 3.8-3.8-3.8-3.5-3.5 3.8-3.8L11 15z');
GEI('path-pause').setAttribute('d', pa + 'M6.5 5.2h5.4v15.5H6.5zm8.3 0h5.4v15.5h-5.4z');
GEI('path-play').setAttribute('d', 'M13 0A13 13 0 0 0 0 13a13 13 0 0 0 13 13 13 13 0 0 0 13-13A13 13 0 0 0 13 0ZM7 5l14 8-14 8Z');
GEI('path-heart').setAttribute('d', 'M75.8 0c-9-.1-18.7 4.8-25.4 15.2C34.3-9.2 4.4-1.7.4 23.9c-1.4 8.8.9 16.4 5.4 23.5h31l4-3.5a2.4 2.4 0 0 1 4 1.2l.4 2 3.2-21.6a2.4 2.4 0 0 1 2.3-2 2.4 2.4 0 0 1 2.4 2l3.6 23.3a2.4 2.4 0 0 1 2-1.2h36a2.4 2.4 0 0 1 .1 0 33 33 0 0 0 5.6-23.7C98.1 8.5 87.4 0 75.8 0zm-25 42-2.6 17.9a2.4 2.4 0 0 1-4.7.2l-2.5-10-1.6 1.5a2.4 2.4 0 0 1-1.6.6H9.2c9.8 12.4 26 24.1 41.2 40.5 14.7-15.9 31-27.8 41-40.4H60l-2.7 4.8a2.4 2.4 0 0 1-4.4-.8l-2.2-14.4z');
QSA('.running').forEach(e=>{e.classList.add('hidden')});


/*
 * Setup button listeners
 */
GEI('heart-icon').onclick = ()=>{
    document.body.requestFullscreen();
    enableNoSleep();
}

GEI('button-track-prev').onclick = ()=>{
    ui.track = (ui.track + tracks.length - 1) %% tracks.length;
    setTrack(ui.track);
    beep();
}

GEI('button-track-next').onclick = ()=>{
    ui.track = (ui.track + 1) %% tracks.length;
    setTrack(ui.track);
    beep();
}

GEI('button-timer-up').onclick = ()=>{
    tc.start += 60 * 1000;
    if (tc.start > 99 * 60 * 1000) {
        tc = 99 * 60 * 1000;
    }
    GEI('timer-output').classList.remove('left');
    //GEI('timer-display').style.marginLeft = 'unset';			
    showTime(true);
    beep();
}

GEI('button-timer-down').onclick = ()=>{
    tc.start -= 60 * 1000;
    if (tc.start < 0) {
        tc.start = 0;
    }
    showTime(true);
    beep();
}

GEI('button-timer-reset').onclick = ()=>{
    GEI('button-timer-pause').classList.add('hidden');
    GEI('button-timer-play').classList.remove('hidden');
    showTime(true);
    beep();
}

GEI('button-setup').onclick = ()=>{
    QSA('.running').forEach(e=>{e.classList.add('hidden');});
    QSA('.setup').forEach(e=>{e.classList.remove('hidden');});
    tc.running = false;
    beep();
}

GEI('button-timer-check').onclick = ()=>{
    QSA('.setup').forEach(e=>{e.classList.add('hidden');});
    QSA('.running').forEach(e=>{ if (e.id !== 'button-timer-pause') { e.classList.remove('hidden'); } });
    showTime(true);
    beep();
}

GEI('button-timer-play').onclick = ()=>{
    GEI('button-timer-play').classList.add('hidden');
    GEI('button-timer-pause').classList.remove('hidden');
    tc.running = true;
    beep();
}

GEI('button-timer-pause').onclick = ()=>{
    GEI('button-timer-pause').classList.add('hidden');
    GEI('button-timer-play').classList.remove('hidden');
    tc.running = false;
    beep();
}

GEI('button-distance-up').onclick = ()=>{
    ui.dist = Math.floor(ui.dist) + 1;
    GEI('distance-setting').innerHTML = ui.dist + `<span class="unit"> mi</span>`;
    showDistance(true);
    beep();
}

GEI('button-distance-down').onclick = ()=>{
    ui.dist = Math.floor(ui.dist) - 1;
    if (ui.dist < 1) {
        ui.dist = 1;
    }
    GEI('distance-setting').innerHTML = ui.dist + `<span class="unit"> mi</span>`;
    showDistance(true);
    beep();
}

GEI('button-distance-reset').onclick = ()=>{
    showDistance(true);
    beep();
}

GEI('button-resistance-up').onclick = ()=>{
    ui.setRes = ui.setRes + 0.1;
    if (ui.setRes > 1.5) {
        ui.setRes = 1.5;
    }
    GEI('resistance-setting').innerHTML = Math.round(ui.setRes * 10);
    GEI('slope-profile').style.transform = `scaleY(${ui.setRes})`;
    beep();
}

GEI('button-resistance-down').onclick = ()=>{
    ui.setRes = ui.setRes - 0.1;
    if (ui.setRes < 0.1) {
        ui.setRes = 0.1;
    }
    GEI('resistance-setting').innerHTML = Math.round(ui.setRes * 10);
    GEI('slope-profile').style.transform = `scaleY(${ui.setRes})`;
    beep();
}

window.addEventListener('load', onLoad);
function onLoad(event) {
    initWebSocket();
}

window.addEventListener('resize', ()=>{
    tm.cam.aspect = tm.view.clientWidth / tm.view.clientHeight;
    tm.cam.updateProjectionMatrix();
    tm.rendr.setSize( tm.view.clientWidth, tm.view.clientHeight );
}, false );




/* 
 * Handy-dandy functions
 */
function calcTracks() {
    tracks.forEach(function (t) {
        let props = getTrackSlopes(t.track);
        t.min = props.min,
        t.max = props.max,
        t.slope = props.slope,
        t.curve = props.curve,
        t.path = getSlopePath(props)
    });
}

function showTime(reset) {
    if (reset) {
        ui.BPMs = [];

        tc.running = false;
        tc.current = tc.start;
        tc.dir = (tc.start === 0 ? 1 : -1)
        if (tc.dir === 1) {
            GEI('timer-output').classList.add('left');
        } else {
            GEI('timer-output').classList.remove('left');
        }
    }

    GEI('timer-setting').innerHTML = Math.round(tc.start / (60 * 1000));

    let mm = Math.floor(tc.current / 60000) %% 60;
    let ss = Math.floor(tc.current / 1000) %% 60;
    let pp = Math.floor(tc.current / 100) %% 10;

    let timeDisplay = '';

    if (tc.current < 50 && tc.dir === -1 && tc.running) {
        GEI('button-timer-pause').classList.add('hidden');
        GEI('button-timer-play').classList.remove('hidden');
        tc.current = 0;
        tc.running = false;
    } else if (tc.current > 99 * 60 * 1000 && tc.dir === 1) {
        GEI('button-timer-pause').classList.add('hidden');
        GEI('button-timer-play').classList.remove('hidden');
        tc.current = 0;
        tc.running = false;
    }

    if (mm > 0) {
        timeDisplay += (mm < 10 ? '0' : '') + mm.toString(10) + ':';
    }
    timeDisplay += (ss < 10 ? '0' : '') + ss.toString(10);
    if (tc.dir === 1) {
        timeDisplay += '<span class="mini">.' + pp.toString(10) + '</span>';
    }

    if (tc.dir === 1) {
        //GEI('timer-display').style.marginLeft = ((34 - timeDisplay.length) * 2) + 'vw';
    }
    GEI('timer-display').innerHTML = timeDisplay;

    let oldMark = tc.mark;
    tc.mark = Date.now();

    tc.heartMark++;

    if ((tc.heartMark %% 20 === 0) && tc.running) {  // TODO - mod this time based on user's timer setup
        let heartTime = tc.current;
        heartTime = tc.start - heartTime;
        ui.BPMs.push(ui.curBPM);
        drawBpmGraph();
    }

    if (tc.running) {
        tc.current += (tc.mark - oldMark) * tc.dir;
    }
}

function showDistance(reset) {
    if (reset) {
        ui.trav = 0;
    }

    let mi = Math.floor(ui.trav);
    let frac = Math.floor((ui.trav - mi) * 100);
    GEI('distance-display').innerHTML = `${mi}<span class="mini">.${frac < 10 ? '0' : ''}${frac}</span><span class="unit"> mi</span>`;
}


function drawBpmGraph() {
    let beatPath = `M0 95 `;
    let x = 0;
    if (ui.BPMs.length > 250) {
        ui.BPMs.shift();
    }
    ui.BPMs.forEach((b, i) => {
        let y = Math.round((b < 190 ? 95 - b / 2 : 0));
        x = i * 2;
        beatPath += ` ${x}, ${y}`;
    });
    beatPath += ` ${x} 95z`;

    GEI('bpm-profile').setAttribute('d', beatPath);
}


function setTrack(n) {
    n = (n || ui.track) %% tracks.length;

    ui.track = n;
    tm.curve = tracks[ui.track].curve;
    tm.cam.position.copy(tracks[ui.track].cameraStart);
    GEI('slope-profile').setAttribute('d', tracks[ui.track].path);
    GEI('slope-profile').style.transform = `scaleY(${ui.setRes})`;
    GEI('track-title').innerHTML = tracks[ui.track].name;

    if (ui.trackObj) {
        scene.remove(ui.trackObj);
        tm.start.position.copy(tm.curve.getPointAt(0));
    }

    const geometry = new THREE.TubeGeometry(tm.curve, 80, 2, 8, false);
    const material = new THREE.MeshBasicMaterial({
        color: 0xff0000
    });
    material.opacity = 0.7;
    material.transparent = true;
    ui.trackObj = new THREE.Mesh(geometry, material);
    scene.add(ui.trackObj);

    ui.mark = Date.now();
};


function viewTrack() {
    tm.view = GEI('track-display');
    tm.cam = new THREE.PerspectiveCamera(45, tm.view.clientWidth / tm.view.clientHeight, 1, 10000);

    const startGeo = new THREE.SphereGeometry(10, 32, 16);
    const startMat = new THREE.MeshBasicMaterial({
        color: 0xff0000
    });
    startMat.opacity = 0.5;
    startMat.transparent = true;
    tm.start = new THREE.Mesh(startGeo, startMat);
    scene.add(tm.start);
    tm.start.position.copy(tracks[ui.track].track[0]);

    const ballGeo = new THREE.SphereGeometry(12, 32, 16);
    const ballMat = new THREE.MeshBasicMaterial({
        color: 0xffffff
    });
    ballMat.opacity = 0.8;
    ballMat.transparent = true;
    tm.ball = new THREE.Mesh(ballGeo, ballMat);
    scene.add(tm.ball);


    tm.grid = new THREE.GridHelper(1000, 25);
    tm.grid.position.y = -50;
    tm.grid.material.opacity = 0.4;
    tm.grid.material.transparent = true;
    scene.add(tm.grid);


    tm.rendr = new THREE.WebGLRenderer();
    tm.rendr.setSize(tm.view.clientWidth, tm.view.clientHeight);
    tm.view.appendChild(tm.rendr.domElement);

    tm.ctrls = new THREE.OrbitControls(tm.cam, tm.rendr.domElement);
}


function getTrackSlopes(t) {
    const c = new THREE.CatmullRomCurve3(t, false);
    slope = [];
    for (let trackLoc = 0.01; trackLoc < 1; trackLoc += 0.01) {
        let dir1 = c.getTangentAt(trackLoc);
        let dir2 = new THREE.Vector3(dir1.x, 0, dir1.z).normalize();
        slope.push((dir1.angleTo(dir2) * 180 / Math.PI) * (dir1.y > 0 ? 1 : -1));
    }
    let max = slope.reduce((a, b) => Math.max(a, b), -Infinity);
    let min = slope.reduce((a, b) => Math.min(a, b), Infinity);
    if (max > 50) {
        let scale = max / 50;
        slope = slope.map(x => {x * scale});
    }
    if (min < -50) {
        let scale = min / -50;
        slope = slope.map(x => {x * scale});
    }
    return {
        min: min,
        max: max,
        slope: slope,
        curve: c
    }
}

function getSlopePath(s) {
    let p1 = 'M0 95';
    s.slope.forEach((y, i) => {
        p1 += ` ${i * 6} ${95 - Math.round((y - s.min) * 3)}`;
    });
    p1 += ` ${(s.slope.length - 1) * 6} 95z`;
    return p1;
}

function showResistance(loc) {
    let pos = Math.floor(tracks[ui.track].slope.length * loc);
    let res = (tracks[ui.track].slope[pos] - tracks[ui.track].min) * (ui.setRes || 1) / 3.2;
    if (res > 10) { res = 10; }
    ui.curRes = res;

    GEI('resistance-display').innerHTML = `${Math.floor(res)}.<span class="mini">${Math.floor((res - Math.floor(res)) * 10)}</span>`;
    GEI('slope-line').setAttribute('d', `M${Math.round(loc * 587) + 1} 0v95`);

    sendResistance();
}

function animate() {
    requestAnimationFrame(animate);

    tm.ctrls.update();

    let trackLoc = (ui.trav / ui.dist) %% 1;
    //ui.trav = trackLoc * ui.dist;

    showDistance();
    showResistance(trackLoc);

    tm.ball.position.copy(tm.curve.getPointAt(trackLoc).clone());
    tm.cam.lookAt(0, 50, 0);
    tm.rendr.render(scene, tm.cam);
}


function beatHeart(beat, bpm) {
    if (!bpm) {
        beat = beat || Date.now();
        if (ui.beatMark !== -1) {
            ui.curBPM = 60000 / (beat - ui.beatMark);
        }
        ui.beatMark = beat;
    } else {
        ui.curBPM = bpm;
    }

    GEI('heartrate').innerHTML = Math.round(ui.curBPM);
    GEI('heart-icon-svg').classList.add('beat'); 
    setTimeout(()=>{
        GEI('heart-icon-svg').classList.remove('beat');
    }, 200);
}


function wheelRev(zeroCheck, rev) {
    rev = rev || (Date.now() - (ui.chipClock === -1 ? 0 : ui.chipClock));
    updateAvg = false;
    if (ui.revMark !== -1) {
        if (!zeroCheck) {
            if (tc.running) {
                ui.trav += milesPerRev;
            }
            ui.speed = Math.round(10 * (3600 * 1000 * milesPerRev) / (rev - ui.revMark)) / 10;
            if (ui.speed < 1) {
                ui.speed = 0;
            }
            updateAvg = true;
        } else if (rev - ui.revMark > 5000) {
            ui.speed = 0;
            updateAvg = true;
        }

        if (updateAvg) {
            ui.speedAvg.push(ui.speed);
            let avgCount = Math.round(Math.ceil(ui.speed) * 1.25);
            let avgSum = 0;
            if (avgCount > ui.speedAvg.length) {
                avgCount = ui.speedAvg.length;
            }
            if (avgCount < 1) {
                avgSpeed = ui.speedAvg[ui.speedAvg.length - 1];
            } else {   
                for (i = ui.speedAvg.length; i > (ui.speedAvg.length - avgCount); i--) {
                    avgSum += ui.speedAvg[i - 1];
                }
                avgSpeed = avgSum / avgCount;
            }
        }

        let mph = Math.floor(avgSpeed);
        GEI('speed-display').innerHTML = `${mph}<span class="mini">.${Math.floor((avgSpeed - mph) * 10)}</span> <span class="unit">mph</span>`;
        if (ui.speedAvg.length > 20) { 
            ui.speedAvg.shift(); 
        }
    }
    if (!zeroCheck) {
        ui.revMark = rev;
    } 
}



let gateway = `ws://${window.location.hostname}/ws`;
let websocket;
function initWebSocket() {
    console.log('Trying to open a WebSocket connection...');
    websocket = new WebSocket(gateway);
    websocket.onopen    = onOpen;
    websocket.onclose   = onClose;
    websocket.onmessage = onMessage; // <-- add this line
}

function onOpen(event) {
    console.log('Connection opened');
}

function onClose(event) {
    console.log('Connection closed');
    setTimeout(initWebSocket, 2000);
}

function onMessage(event) {
    let chipClock = -1
    if (event.data) {
        try {
            let result = JSON.parse(event.data);

            if (result['beat']) {
                // heartbeat
                chipClock = parseInt(result['beat']);
                beatHeart(chipClock);
            }
            if (result['tire']) {
                // tire rotation
                chipClock = parseInt(result['tire'])
                wheelRev(false, chipClock);
            }
            if (result['button']) {
                // button press
            }
        } catch (e) {
            console.warn(e);
        }
    }

    if (chipClock !== -1) {
        ui.chipClock = Date.now() - chipClock;  // determine difference between the microcontroller's clock and the browser's clock
    }
}


function sendResistance() {
    if (ui.curRes !== ui.lastRes) {
        try {
            ui.lastRes = ui.curRes;
            websocket.send('setres' + Math.floor(ui.curRes * 100));
        } catch (e) {
            console.warn(e);
        }
    }
}

function beep() {
    try {
        websocket.send('beep');
    } catch (e) {
        console.warn(e);
    }
}

calcTracks();
viewTrack();

setTrack();                        // Set initial track
setInterval(showTime, 75);         // Start clock/timer manager
setInterval(wheelRev, 1000, true); // make sure speed isn't too low
animate();                         // Start 3D track visualizer (recursive);

</script></body></html>
)rawliteral";


void notifyClients(String action) {
  char buffer[50];
  if (action == "minres") {
    sprintf(buffer, "{\"%s\":%d}", minimum_resistance, millis());
  } else if (action == "state") {
    sprintf(buffer, "{\"state\":\"%s\",\"mark\":%d}", state_led ? "ON" : "OFF", millis());
  } else {
    sprintf(buffer, "{\"%s\":%d}", action, millis());
  }
  ws.textAll(buffer);
}

void setResistance(int16_t res) {
  Serial.print("Setting Resistance: ");
  last_res = res;
  resistance_target = (uint8_t) map(res, 1000, 0, 6, minimum_resistance);

  if (resistance_target > 188) {
    resistance_target = 188;
  } else if (resistance_target < 6) {
    resistance_target = 6;
  }

  Serial.print(resistance_target, DEC);
  current_resistance = analogRead(PIN_POT1) >> 4;

  int resistance_diff = current_resistance - ((int) resistance_target);
  if (resistance_diff < 0) {
    resistance_diff = 0 - resistance_diff;
  }

  motor_move_ticks = resistance_diff * 800;

  Serial.print("   Currently: ");
  Serial.println(current_resistance, DEC);
}



void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;

    char str[40];
    strcpy(str, (char*)data);

    if (strcmp((char*)data, "toggle") == 0) {
      state_led = !state_led;
      notifyClients("state");
    }
    if (str[0] == 's' && str[2] == 't' && str[3] == 'r' && str[4] == 'e' && str[5] == 's') {
      int16_t res;
      sscanf(str, "setres%d", &res);
      setResistance(res);
    }
    if (strcmp(str, "beep") == 0) {
      beep_count = 500;
    }
  }
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
             void *arg, uint8_t *data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      break;
    case WS_EVT_DATA:
      handleWebSocketMessage(arg, data, len);
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
  }
}

void initWebSocket() {
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}

String processor(const String& var){
  Serial.println(var);
  if(var == "STATE"){
    if (state_led){
      return "ON";
    }
    else{
      return "OFF";
    }
  }
  return String();
}

void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);

  //pinMode(PIN_LED, OUTPUT);
  //digitalWrite(PIN_LED, LOW);
  pinMode(PIN_BEEP, OUTPUT);
  digitalWrite(PIN_BEEP, LOW);

  pinMode(PIN_MOTOR_1, OUTPUT);
  pinMode(PIN_MOTOR_2, OUTPUT);
  digitalWrite(PIN_MOTOR_1, HIGH);
  digitalWrite(PIN_MOTOR_2, HIGH);
  
  pinMode(PIN_TIRE, INPUT_PULLUP);
  pinMode(PIN_EKG, INPUT);
  pinMode(PIN_POT1, INPUT);
  pinMode(PIN_POT2, INPUT);

  pinMode(PIN_BUTTON_1, INPUT_PULLUP);
  pinMode(PIN_BUTTON_2, INPUT_PULLUP);
  pinMode(PIN_BUTTON_3, INPUT_PULLUP);
  pinMode(PIN_BUTTON_4, INPUT_PULLUP);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  // Print ESP Local IP Address
  Serial.println(WiFi.localIP());

  initWebSocket();

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });

  // Start server
  server.begin();

  state_tire = digitalRead(PIN_TIRE);
}

void loop() {
  ws.cleanupClients();

  if (btn_1_debounce < 1) {
    bool btn1 = digitalRead(PIN_BUTTON_1);
    if (btn1 == 0 && state_btn_1 == 1) {
      notifyClients("button1");
      beep_count = 250;
      state_btn_1 = 0;
      btn_1_debounce = 1000;   // debounce
    } else if (btn1 == 1 && state_btn_1 == 0) {
      state_btn_1 = 1;
      btn_1_debounce = 1000;   // debounce
    }
  } else {
    btn_1_debounce--;
  }

  if (btn_2_debounce < 1) {
    bool btn2 = digitalRead(PIN_BUTTON_2);
    if (btn2 == 0 && state_btn_2 == 1) {
      notifyClients("button2");
      beep_count = 250;

      minimum_resistance -= 32;
      if (minimum_resistance < 736) {
        minimum_resistance = 736;
        beep_count = 0;
      }
      setResistance(last_res);

      state_btn_2 = 0;
      btn_2_debounce = 1000;   // debounce
    } else if (btn2 == 1 && state_btn_2 == 0) {
      state_btn_2 = 1;
      btn_2_debounce = 1000;   // debounce
    }
  } else {
    btn_2_debounce--;
  }

  if (btn_3_debounce < 1) {
    bool btn3 = digitalRead(PIN_BUTTON_3);
    if (btn3 == 0 && state_btn_3 == 1) {
      notifyClients("button3");
      beep_count = 250;

      minimum_resistance += 32;
      if (minimum_resistance > 3008) {
        minimum_resistance = 3008;
        beep_count = 0;
      }      
      setResistance(last_res);

      state_btn_3 = 0;
      btn_3_debounce = 1000;   // debounce
    } else if (btn3 == 1 && state_btn_3 == 0) {
      state_btn_3 = 1;
      btn_3_debounce = 1000;   // debounce
    }
  } else {
    btn_3_debounce--;
  }

  if (btn_4_debounce < 1) {
    bool btn4 = digitalRead(PIN_BUTTON_4);
    if (btn4 == 0 && state_btn_4 == 1) {
      notifyClients("button4");

      minimum_resistance = 3008;
      setResistance(last_res);

      beep_count = 250;
      state_btn_4 = 0;
      btn_4_debounce = 1000;   // debounce
    } else if (btn4 == 1 && state_btn_4 == 0) {
      state_btn_4 = 1;
      btn_4_debounce = 1000;   // debounce
    }
  } else {
    btn_4_debounce--;
  }


  if (tire_debounce < 1) {
    bool tire = digitalRead(PIN_TIRE);
    if (tire == 0 && state_tire == 1) {
      notifyClients("tire");
      state_tire = 0;
      tire_debounce = 1000;   // debounce
    } else if (tire == 1 && state_tire == 0) {
      state_tire = 1;
      tire_debounce = 1000;   // debounce
    }
  } else {
    tire_debounce--;
  }



  if (beep_count > 0) {
    digitalWrite(PIN_BEEP, HIGH);
    beep_count--;
  } else {
    digitalWrite(PIN_BEEP, LOW);
  }

  current_resistance = (analogRead(PIN_POT1) >> 4);
  motor_move_ticks--;
  if (motor_move_ticks < 0) {
    motor_move_ticks = 0;
  }

  if (current_resistance > ((int) resistance_target) && motor_move_ticks > 0) {
    digitalWrite(PIN_MOTOR_1, HIGH);
    digitalWrite(PIN_MOTOR_2, LOW);

  } else if (current_resistance < ((int) resistance_target) && motor_move_ticks > 0) {
    digitalWrite(PIN_MOTOR_1, LOW);
    digitalWrite(PIN_MOTOR_2, HIGH);

  } else {
    digitalWrite(PIN_MOTOR_1, HIGH);
    digitalWrite(PIN_MOTOR_2, HIGH);
  }

  info_tick++;
  if (info_tick % 5000 == 0) {
    Serial.printf("cur: %d, target: %d, tick: %d\n", current_resistance, resistance_target, motor_move_ticks);
  } 
}