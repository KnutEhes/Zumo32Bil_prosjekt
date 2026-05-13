const canvas = document.getElementById("mapCanvas");
const ctx = canvas.getContext("2d");

const logEl = document.getElementById("log");
const uiStatusEl = document.getElementById("uiStatus");
const deviceStatusEl = document.getElementById("deviceStatus");
const positionTextEl = document.getElementById("positionText");
const seqTextEl = document.getElementById("seqText");
const timestampTextEl = document.getElementById("timestampText");
const anchorInfoEl = document.getElementById("anchorInfo");

let roomWidth = 480;
let roomHeight = 650;
let anchors = {
  A1: { x: 15, y: 5 },
  A2: { x: 290, y: 5 },
  A3: { x: 165, y: 625 }
};

let currentPosition = null;
let latestAnchorData = null;
let path = [];

function log(msg) {
  logEl.textContent += msg + "\n";
  logEl.scrollTop = logEl.scrollHeight;
}

function wsUrl(path) {
  const proto = location.protocol === "https:" ? "wss" : "ws";
  return `${proto}://${location.host}${path}`;
}

const ws = new WebSocket(wsUrl("/ws/ui"));

ws.onopen = () => {
  uiStatusEl.textContent = "Tilkoblet";
  log("UI websocket connected");
};

ws.onclose = () => {
  uiStatusEl.textContent = "Frakoblet";
  log("UI websocket disconnected");
};

ws.onerror = () => {
  uiStatusEl.textContent = "Feil";
  log("WebSocket error");
};

ws.onmessage = (event) => {
  let data;

  try {
    data = JSON.parse(event.data);
  } catch {
    log("RX raw: " + event.data);
    return;
  }

  handleMessage(data);
};

function handleMessage(data) {
  if (data.type === "config") {
    roomWidth = data.room.width;
    roomHeight = data.room.height;
    anchors = data.anchors;
    drawMap();
    return;
  }

  if (data.type === "status") {
    log("[STATUS] " + data.message);

    if (data.message.startsWith("Device connected:")) {
      deviceStatusEl.textContent = data.message.replace("Device connected: ", "");
    }

    if (data.message.startsWith("Device disconnected:")) {
      deviceStatusEl.textContent = "Ingen";
    }

    if (data.seq !== undefined) {
      seqTextEl.textContent = data.seq;
    }
    if (data.timestamp_ms !== undefined) {
      timestampTextEl.textContent = data.timestamp_ms;
    }

    if (data.anchors) {
      latestAnchorData = data.anchors;
      renderAnchorInfo();
      drawMap();
    }

    return;
  }

  if (data.type === "position") {
    currentPosition = { x: data.x, y: data.y };
    latestAnchorData = data.anchors || null;

    path.push({ x: data.x, y: data.y });
    if (path.length > 200) {
      path.shift();
    }

    positionTextEl.textContent = `x=${data.x.toFixed(1)} cm, y=${data.y.toFixed(1)} cm`;
    seqTextEl.textContent = data.seq ?? "–";
    timestampTextEl.textContent = data.timestamp_ms ?? "–";

    renderAnchorInfo();
    drawMap();
    return;
  }

  if (data.type === "telemetry") {
    log("[TELEMETRY] " + JSON.stringify(data.payload));
    return;
  }

  if (data.type === "raw") {
    log("[RAW] " + JSON.stringify(data.payload));
    return;
  }

  if (data.type === "error") {
    log("[ERROR] " + data.message);
    return;
  }

  log("[UNKNOWN] " + JSON.stringify(data));
}

function sendCmd(cmd) {
  if (ws.readyState !== WebSocket.OPEN) {
    log("Kan ikke sende kommando: UI websocket ikke åpen");
    return;
  }

  const msg = {
    type: "cmd",
    target: "hub-1",
    cmd: cmd
  };

  ws.send(JSON.stringify(msg));
  log("[TX] " + JSON.stringify(msg));
}

function renderAnchorInfo() {
  anchorInfoEl.innerHTML = "";

  if (!latestAnchorData) {
    anchorInfoEl.textContent = "Ingen anchor-data ennå";
    return;
  }

  Object.entries(latestAnchorData).forEach(([name, a]) => {
    const div = document.createElement("div");
    div.className = "anchor-box";

    const validClass = a.valid ? "anchor-valid" : "anchor-invalid";
    const validText = a.valid ? "VALID" : "INVALID";

    div.innerHTML = `
      <strong>${name}</strong><br>
      distance: ${a.distance}<br>
      rssi: ${a.rssi}<br>
      age_ms: ${a.age_ms}<br>
      source: ${a.source || "-"}<br>
      <span class="${validClass}">${validText}</span>
    `;

    anchorInfoEl.appendChild(div);
  });
}

function toCanvasCoords(x, y) {
  const cx = (x / roomWidth) * canvas.width;
  const cy = canvas.height - (y / roomHeight) * canvas.height;
  return { x: cx, y: cy };
}

function drawGrid() {
  ctx.strokeStyle = "#dddddd";
  ctx.lineWidth = 1;

  const step = 50;

  for (let x = 0; x <= roomWidth; x += step) {
    const p1 = toCanvasCoords(x, 0);
    const p2 = toCanvasCoords(x, roomHeight);
    ctx.beginPath();
    ctx.moveTo(p1.x, p1.y);
    ctx.lineTo(p2.x, p2.y);
    ctx.stroke();
  }

  for (let y = 0; y <= roomHeight; y += step) {
    const p1 = toCanvasCoords(0, y);
    const p2 = toCanvasCoords(roomWidth, y);
    ctx.beginPath();
    ctx.moveTo(p1.x, p1.y);
    ctx.lineTo(p2.x, p2.y);
    ctx.stroke();
  }
}

function drawAnchors() {
  const colors = {
    A1: "#2e7d32",
    A2: "#1565c0",
    A3: "#8e24aa"
  };

  for (const [name, anchor] of Object.entries(anchors)) {
    const p = toCanvasCoords(anchor.x, anchor.y);

    ctx.fillStyle = colors[name] || "#000";
    ctx.beginPath();
    ctx.moveTo(p.x, p.y - 10);
    ctx.lineTo(p.x - 10, p.y + 10);
    ctx.lineTo(p.x + 10, p.y + 10);
    ctx.closePath();
    ctx.fill();

    ctx.fillStyle = "#000";
    ctx.font = "16px Arial";
    ctx.fillText(name, p.x + 12, p.y - 8);

    if (latestAnchorData && latestAnchorData[name]) {
      const rssi = latestAnchorData[name].rssi;
      const valid = latestAnchorData[name].valid;
      ctx.fillStyle = valid ? "#0a0" : "#a00";
      ctx.fillText(`${rssi.toFixed(1)} dBm`, p.x + 12, p.y + 12);
    }
  }
}

function drawPath() {
  if (path.length < 2) return;

  ctx.strokeStyle = "#1565c0";
  ctx.lineWidth = 2;
  ctx.beginPath();

  for (let i = 0; i < path.length; i++) {
    const p = toCanvasCoords(path[i].x, path[i].y);

    if (i === 0) {
      ctx.moveTo(p.x, p.y);
    } else {
      ctx.lineTo(p.x, p.y);
    }
  }

  ctx.stroke();
}

function drawPosition() {
  if (!currentPosition) return;

  const p = toCanvasCoords(currentPosition.x, currentPosition.y);

  ctx.fillStyle = "#d32f2f";
  ctx.beginPath();
  ctx.arc(p.x, p.y, 8, 0, Math.PI * 2);
  ctx.fill();

  ctx.fillStyle = "#000";
  ctx.font = "16px Arial";
  ctx.fillText("Tag", p.x + 10, p.y - 10);
}

function drawRoomBorder() {
  ctx.strokeStyle = "#000";
  ctx.lineWidth = 2;
  ctx.strokeRect(0, 0, canvas.width, canvas.height);
}

function drawMap() {
  ctx.clearRect(0, 0, canvas.width, canvas.height);
  ctx.fillStyle = "#fafafa";
  ctx.fillRect(0, 0, canvas.width, canvas.height);

  drawGrid();
  drawPath();
  drawAnchors();
  drawPosition();
  drawRoomBorder();
}

drawMap();
renderAnchorInfo();