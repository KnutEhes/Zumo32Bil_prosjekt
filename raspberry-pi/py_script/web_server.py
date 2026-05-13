import json
from pathlib import Path

import numpy as np
from scipy.optimize import least_squares
from fastapi import FastAPI, WebSocket, WebSocketDisconnect
from fastapi.responses import FileResponse
from fastapi.staticfiles import StaticFiles

BASE_DIR = Path(__file__).resolve().parent
STATIC_DIR = BASE_DIR / "static"

app = FastAPI()
app.mount("/static", StaticFiles(directory=STATIC_DIR), name="static")

ROOM_WIDTH = 480.0
ROOM_HEIGHT = 650.0

ANCHORS = {
    "A1": {"x": 15.0, "y": 5.0},
    "A2": {"x": 290.0, "y": 5.0},
    "A3": {"x": 165.0, "y": 625.0},
}

ui_clients = set()
devices = {}
last_position_payload = None


def trilaterate_generic(anchor_points, distances):
    def residuals(p):
        x, y = p
        return [
            np.sqrt((x - ax) ** 2 + (y - ay) ** 2) - d
            for (ax, ay), d in zip(anchor_points, distances)
        ]

    initial_guess = np.mean(anchor_points, axis=0)

    try:
        result = least_squares(residuals, initial_guess, method="lm")
        return result.x if result.success else None
    except Exception as e:
        print(f"Trilateration error: {e}")
        return None


async def send_json(ws: WebSocket, payload: dict):
    await ws.send_text(json.dumps(payload))


async def broadcast_to_ui(payload: dict):
    dead = []
    msg = json.dumps(payload)

    for ws in ui_clients:
        try:
            await ws.send_text(msg)
        except Exception:
            dead.append(ws)

    for ws in dead:
        ui_clients.discard(ws)


@app.get("/")
async def index():
    return FileResponse(STATIC_DIR / "index.html")


@app.websocket("/ws/ui")
async def ws_ui(ws: WebSocket):
    global last_position_payload

    await ws.accept()
    ui_clients.add(ws)

    await send_json(
        ws,
        {
            "type": "config",
            "room": {"width": ROOM_WIDTH, "height": ROOM_HEIGHT},
            "anchors": ANCHORS,
        },
    )

    if last_position_payload is not None:
        await send_json(ws, last_position_payload)

    await broadcast_to_ui({"type": "status", "message": "UI client connected"})

    try:
        while True:
            msg = await ws.receive_text()

            try:
                data = json.loads(msg)
            except json.JSONDecodeError:
                await send_json(ws, {"type": "error", "message": "Invalid JSON from UI"})
                continue

            if data.get("type") == "cmd":
                target = data.get("target")
                device_ws = devices.get(target)

                if device_ws:
                    await device_ws.send_text(json.dumps(data))
                    await broadcast_to_ui(
                        {
                            "type": "status",
                            "message": f"Forwarded command '{data.get('cmd')}' to {target}",
                        }
                    )
                else:
                    await send_json(
                        ws,
                        {
                            "type": "error",
                            "message": f"No device connected with id '{target}'",
                        },
                    )

    except WebSocketDisconnect:
        ui_clients.discard(ws)
        await broadcast_to_ui({"type": "status", "message": "UI client disconnected"})


@app.websocket("/ws/device")
async def ws_device(ws: WebSocket):
    global last_position_payload

    await ws.accept()
    device_id = None

    try:
        hello_msg = await ws.receive_text()
        hello = json.loads(hello_msg)

        if hello.get("type") != "hello":
            await send_json(
                ws,
                {"type": "error", "message": "First message must be type='hello'"},
            )
            await ws.close()
            return

        device_id = hello.get("device", "unknown")
        devices[device_id] = ws

        await broadcast_to_ui(
            {"type": "status", "message": f"Device connected: {device_id}"}
        )

        while True:
            msg = await ws.receive_text()

            try:
                data = json.loads(msg)
            except json.JSONDecodeError:
                await broadcast_to_ui(
                    {
                        "type": "raw",
                        "device": device_id,
                        "payload": msg,
                    }
                )
                continue

            msg_type = data.get("type")

            if msg_type == "tracking":
                anchors_in = data.get("anchors", {})
                valid_anchor_points = []
                valid_distances = []
                used_anchors = {}
                all_anchor_info = {}

                for anchor_id, anchor_data in anchors_in.items():
                    if anchor_id not in ANCHORS:
                        continue

                    distance = float(anchor_data.get("distance", 0))
                    rssi = float(anchor_data.get("rssi", -999))
                    valid = bool(anchor_data.get("valid", False))
                    source = anchor_data.get("source", "")
                    age_ms = int(anchor_data.get("age_ms", -1))

                    all_anchor_info[anchor_id] = {
                        "distance": distance,
                        "rssi": rssi,
                        "valid": valid,
                        "source": source,
                        "age_ms": age_ms,
                    }

                    if valid:
                        valid_anchor_points.append(
                            [ANCHORS[anchor_id]["x"], ANCHORS[anchor_id]["y"]]
                        )
                        valid_distances.append(distance)
                        used_anchors[anchor_id] = all_anchor_info[anchor_id]

                if len(valid_anchor_points) < 3:
                    await broadcast_to_ui(
                        {
                            "type": "status",
                            "message": f"Not enough valid anchors ({len(valid_anchor_points)})",
                            "device": device_id,
                            "seq": data.get("seq"),
                            "timestamp_ms": data.get("timestamp_ms"),
                            "anchors": all_anchor_info,
                        }
                    )
                    continue

                pos = trilaterate_generic(
                    np.array(valid_anchor_points, dtype=float),
                    valid_distances,
                )

                if pos is not None:
                    x_cm, y_cm = pos.tolist()

                    payload = {
                        "type": "position",
                        "device": device_id,
                        "tag_id": data.get("tag_id", "unknown"),
                        "timestamp_ms": data.get("timestamp_ms"),
                        "seq": data.get("seq"),
                        "x": x_cm,
                        "y": y_cm,
                        "anchors": all_anchor_info,
                        "used_anchors": used_anchors,
                    }

                    last_position_payload = payload
                    await broadcast_to_ui(payload)
                else:
                    await broadcast_to_ui(
                        {
                            "type": "status",
                            "message": "Trilateration failed",
                            "device": device_id,
                            "seq": data.get("seq"),
                            "timestamp_ms": data.get("timestamp_ms"),
                        }
                    )

            elif msg_type == "telemetry":
                await broadcast_to_ui(
                    {
                        "type": "telemetry",
                        "device": device_id,
                        "payload": data,
                    }
                )

            else:
                await broadcast_to_ui(
                    {
                        "type": "raw",
                        "device": device_id,
                        "payload": data,
                    }
                )

    except WebSocketDisconnect:
        pass
    finally:
        if device_id and devices.get(device_id) is ws:
            devices.pop(device_id, None)

        await broadcast_to_ui(
            {"type": "status", "message": f"Device disconnected: {device_id}"}
        )