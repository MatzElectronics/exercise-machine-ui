
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

const milesPerRev = 0.00124;

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
}

GEI('button-track-prev').onclick = ()=>{
    ui.track = (ui.track + tracks.length - 1) % tracks.length;
    setTrack(ui.track);
}

GEI('button-track-next').onclick = ()=>{
    ui.track = (ui.track + 1) % tracks.length;
    setTrack(ui.track);
}

GEI('button-timer-up').onclick = ()=>{
    tc.start += 60 * 1000;
    if (tc.start > 99 * 60 * 1000) {
        tc = 99 * 60 * 1000;
    }
    GEI('timer-output').classList.remove('left');
    GEI('timer-display').style.marginLeft = 'unset';			
    showTime(true);
}

GEI('button-timer-down').onclick = ()=>{
    tc.start -= 60 * 1000;
    if (tc.start < 0) {
        tc.start = 0;
    }
    showTime(true);
}

GEI('button-timer-reset').onclick = ()=>{
    GEI('button-timer-pause').classList.add('hidden');
    GEI('button-timer-play').classList.remove('hidden');
    showTime(true);
}

GEI('button-setup').onclick = ()=>{
    QSA('.running').forEach(e=>{e.classList.add('hidden');});
    QSA('.setup').forEach(e=>{e.classList.remove('hidden');});
    tc.running = false;
}

GEI('button-timer-check').onclick = ()=>{
    QSA('.setup').forEach(e=>{e.classList.add('hidden');});
    QSA('.running').forEach(e=>{ if (e.id !== 'button-timer-pause') { e.classList.remove('hidden'); } });
    showTime(true);
}

GEI('button-timer-play').onclick = ()=>{
    GEI('button-timer-play').classList.add('hidden');
    GEI('button-timer-pause').classList.remove('hidden');
    tc.running = true;
}

GEI('button-timer-pause').onclick = ()=>{
    GEI('button-timer-pause').classList.add('hidden');
    GEI('button-timer-play').classList.remove('hidden');
    tc.running = false;
}

GEI('button-distance-up').onclick = ()=>{
    ui.dist = Math.floor(ui.dist) + 1;
    GEI('distance-setting').innerHTML = ui.dist + `<span class="unit"> mi</span>`;
    showDistance(true);
}

GEI('button-distance-down').onclick = ()=>{
    ui.dist = Math.floor(ui.dist) - 1;
    if (ui.dist < 1) {
        ui.dist = 1;
    }
    GEI('distance-setting').innerHTML = ui.dist + `<span class="unit"> mi</span>`;
    showDistance(true);
}

GEI('button-distance-reset').onclick = ()=>{
    showDistance(true);
}

GEI('button-resistance-up').onclick = ()=>{
    ui.setRes = ui.setRes + 0.1;
    if (ui.setRes > 1.5) {
        ui.setRes = 1.5;
    }
    GEI('resistance-setting').innerHTML = Math.round(ui.setRes * 10);
    GEI('slope-profile').style.transform = `scaleY(${ui.setRes})`;
}

GEI('button-resistance-down').onclick = ()=>{
    ui.setRes = ui.setRes - 0.1;
    if (ui.setRes < 0.1) {
        ui.setRes = 0.1;
    }
    GEI('resistance-setting').innerHTML = Math.round(ui.setRes * 10);
    GEI('slope-profile').style.transform = `scaleY(${ui.setRes})`;
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



/**
 * Uses the array of points that defines a track 
 * and calls the functions that calculate things like 
 * the tangent slopes, minimums, maximums, etc. for the
 * tracks, and adds them back to the "tracks" object
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

/**
 * This function manages the timer display.  It can count up or down
 * and it makes sure it is displayed correctly.
 * It needs to be called repeatedly in an interval smaller than
 * the smallest unit of time displayed (0.1 seconds)
 * @param {boolean} reset reset the timer to 0 or the beginning of the countdown
 */
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

    let mm = Math.floor(tc.current / 60000) % 60;
    let ss = Math.floor(tc.current / 1000) % 60;
    let pp = Math.floor(tc.current / 100) % 10;

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
        GEI('timer-display').style.marginLeft = ((34 - timeDisplay.length) * 2) + 'vw';
    }
    GEI('timer-display').innerHTML = timeDisplay;

    let oldMark = tc.mark;
    tc.mark = Date.now();

    tc.heartMark++;

    if ((tc.heartMark % 20 === 0) && tc.running) {  // TODO - modify this time (mod) based on user's timer setup
        let heartTime = tc.current;
        heartTime = tc.start - heartTime;
        ui.BPMs.push(ui.curBPM);
        drawBpmGraph();
    }

    if (tc.running) {
        tc.current += (tc.mark - oldMark) * tc.dir;
    }
}

/**
 * This function manages the odometer display.  It must be called
 * whenever the distance travelled has changed.
 * @param {boolean} reset
 */
function showDistance(reset) {
    if (reset) {
        ui.trav = 0;
    }

    let mi = Math.floor(ui.trav);
    let frac = Math.floor((ui.trav - mi) * 100);
    GEI('distance-display').innerHTML = `${mi}<span class="mini">.${frac < 10 ? '0' : ''}${frac}</span><span class="unit"> mi</span>`;
}

/**
 * This functions updates and maintains an SVG
 * image that is a graph representing the BPM of the user.
 * It is called from within the showTime() function.
 */
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

/**
 * This function sets which track should be displayed and run.
 * @param {Number} n the index of the track within the "tracks" object
 */
function setTrack(n) {
    n = (n || ui.track) % tracks.length;

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

/**
 * This function builds the track visualization 
 */
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

/**
 * This function builds the track's curve and calculates the slopes,
 * minimum, and maximum. 
 * @param {Array[THREE.Vector3]} t 3D points that define the track
 * @returns {object} That contains the tracks curve and slopes
 */
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

/**
 * This function creates an SVG path that is used to visulaize the slope/steepness of
 * the points along the track
 * @param {Object} s Object containing track properties 
 * @returns 
 */
function getSlopePath(s) {
    let p1 = 'M0 95';
    s.slope.forEach((y, i) => {
        p1 += ` ${i * 6} ${95 - Math.round((y - s.min) * 3)}`;
    });
    p1 += ` ${(s.slope.length - 1) * 6} 95z`;
    return p1;
}

/**
 * This function calculates and displays the resistance setting for the 
 * machine at the specified point along the track
 * @param {Number} loc number from 0 to 1 representing a point along the track
 */
function showResistance(loc) {
    let pos = Math.floor(tracks[ui.track].slope.length * loc);
    let res = (tracks[ui.track].slope[pos] - tracks[ui.track].min) * (ui.setRes || 1) / 3.2;
    if (res > 10) { res = 10; }
    ui.curRes = res;

    GEI('resistance-display').innerHTML = `${Math.floor(res)}.<span class="mini">${Math.floor((res - Math.floor(res)) * 10)}</span>`;
    GEI('slope-line').setAttribute('d', `M${Math.round(loc * 587) + 1} 0v95`);

    sendResistance();
}

/**
 * This is a recursive function that updates and
 * displays the 3D visualization of the track
 */
function animate() {
    requestAnimationFrame(animate);

    tm.ctrls.update();

    let trackLoc = (ui.trav / ui.dist) % 1;
    //ui.trav = trackLoc * ui.dist;

    showDistance();
    showResistance(trackLoc);

    tm.ball.position.copy(tm.curve.getPointAt(trackLoc).clone());
    tm.cam.lookAt(0, 50, 0);
    tm.rendr.render(scene, tm.cam);
}

/**
 * This function calculates and displays
 * the user's heart rate.  It can use either the browser's
 * clock or the microcontroller's clock, depending on what
 * information the microcontroller sends
 * @param {Number} beat if included, is a time mark in milliseconds.  If 
 * not included use the browser's clock.
 * @param {Number} bpm if included, override the broswer's claculaton and
 * use this instead (calculated on the microcontroller)
 */
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

/**
 * This function calcuates the user's speed based on the signal from the
 * tachometer.  The more frequently it's called, the faster the user is going.
 * It is also called on a regular interval to make sure the speed is not zero
 * (pulses stop coming in)
 * @param {Boolean} zeroCheck if true, don't use this mark to calculate speed
 * @param {Number} rev if included, use this time mark instead of the browser's clock
 */
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


/*
 * Set up all of the websocket stuff
 */
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

// simulate hardware
//setInterval(wheelRev, 300);        // TODO: Replace with WEBSOCKET MESSAGE
//setInterval(beatHeart, 700);   // TODO: Replace with WEBSOCKET MESSAGE



