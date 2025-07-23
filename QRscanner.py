import cv2
from pyzbar.pyzbar import decode
import numpy as np
import json
import sys
import os

JSON_FILE = "decoded.json"

def save_json_to_file(data_dict):
    with open(JSON_FILE, "w") as f:
        json.dump(data_dict, f, indent=4)
    print(f"✅ Processed QR code data saved to {JSON_FILE}")

def parse_qr_data(qr_data):
    try:
        parsed = json.loads(qr_data)
        if isinstance(parsed, dict):
            return parsed.get("data", parsed)
    except json.JSONDecodeError:
        pass

    data = {}
    for item in qr_data.split(","):
        if ":" in item:
            key, value = item.split(":", 1)
            data[key.strip()] = value.strip()
        else:
            print(f"⚠️ Skipping invalid entry: {item}")
    return data

def open_camera():
    # Try different indices/backends if default fails
    candidates = [
        (0, cv2.CAP_ANY),
        (0, cv2.CAP_V4L2),
        (0, cv2.CAP_DSHOW),
        (1, cv2.CAP_V4L2),
        (-1, cv2.CAP_ANY)
    ]
    for idx, backend in candidates:
        cap = cv2.VideoCapture(idx, backend)
        if cap.isOpened():
            print(f"🎥 Camera opened with index {idx}, backend {backend}")
            return cap
    return None

def read_qr_from_camera():
    if os.path.exists(JSON_FILE):
        os.remove(JSON_FILE)

    cap = open_camera()
    if cap is None:
        print("❌ Cannot open camera. Check index/backends/permissions.")
        sys.exit(1)

    cap.set(cv2.CAP_PROP_FRAME_WIDTH, 1280)
    cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 720)

    print("📷 Scanning for QR Code. Press 'q' to quit.")
    cv2.namedWindow('QR Code Detection', cv2.WINDOW_NORMAL)

    while True:
        ret, frame = cap.read()
        if not ret:
            print("❌ Can't receive frame. Exiting ...")
            break

        gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
        gray = cv2.convertScaleAbs(gray, alpha=1.5, beta=0)
        gray = cv2.bilateralFilter(gray, 9, 75, 75)
        thresh = cv2.adaptiveThreshold(gray, 255,
                                       cv2.ADAPTIVE_THRESH_GAUSSIAN_C,
                                       cv2.THRESH_BINARY, 11, 2)
        thresh_color = cv2.cvtColor(thresh, cv2.COLOR_GRAY2BGR)

        qr_codes = decode(thresh) + decode(frame)
        if qr_codes:
            for qr in qr_codes:
                qr_data = qr.data.decode('utf-8')
                print(f"🔍 Raw QR Code Data: {qr_data}")
                parsed_data = parse_qr_data(qr_data)
                save_json_to_file(parsed_data)
                cap.release()
                cv2.destroyAllWindows()
                return

        combined = np.hstack((cv2.resize(frame, (640, 480)),
                              cv2.resize(thresh_color, (640, 480))))
        cv2.imshow('QR Code Detection', combined)

        if cv2.waitKey(1) & 0xFF == ord('q'):
            break

    cap.release()
    cv2.destroyAllWindows()

if __name__ == "__main__":
    read_qr_from_camera()
