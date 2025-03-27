import sys
sys.coinit_flags = 0 # to fix them error

import asyncio
from bleak import BleakClient, BleakScanner
from bleak.backends.characteristic import BleakGATTCharacteristic
import cv2
import yaml
from ultralytics import YOLO
from time import time
from bleak.backends.winrt.util import uninitialize_sta



cfg = yaml.safe_load(open("cfg.yaml"))

def notification_handler(characteristic: BleakGATTCharacteristic, data: bytearray):
    """Simple notification handler which prints the data received."""
    print(f"Notification from {characteristic.description}: {data.decode()}")

def sent_message(client, x, y, xerror, yerror):
    message = f"TARGET,{x},{y},{xerror},{yerror},{time()}"
    # print(f"Sending: {message}")
    return client.write_gatt_char(cfg['CHARACTERISTIC_UUID_RX'], message.encode())
    


async def main():
    model = YOLO(cfg['MODEL']).to('cuda')

    async with BleakClient(cfg['ESP_MAC']) as client:
        print(f"Connected: {client.is_connected}")

        await client.start_notify(cfg['CHARACTERISTIC_UUID_TX'], notification_handler)

        cap = cv2.VideoCapture(cfg['CAM'])
        uninitialize_sta()

        while cap.isOpened():

            ret, frame = cap.read()
            if not ret:
                break

            # frame = cv2.flip(frame, 0)

            results = model(frame, verbose=False)

            for result in results:
                person_found = False
                for box in result.boxes:
                    x1, y1, x2, y2 = map(int, box.xyxy[0])
                    conf = box.conf[0].item()
                    cls = int(box.cls[0].item())

                    # if model.names[cls] != "person" or conf < 0.5 or person_found:
                    #     continue
                    if conf < 0.5 or person_found:
                        continue
                    person_found = True

                    label = f"{model.names[cls]} {conf:.2f}"

                    cv2.rectangle(frame, (x1, y1), (x2, y2), (0, 255, 0), 2)
                    cv2.putText(frame, label, (x1, y1 - 10), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 255, 0), 2)

                    # a ray from the center of the screen to the center of the target
                    cv2.arrowedLine(frame, (frame.shape[1] // 2, frame.shape[0] // 2), ((x1 + x2) // 2, (y1 + y2) // 2), (0, 0, 255), 2)


                    x = (x1 + x2) // 2
                    y = (y1 + y2) // 2
                    #  errors are calculated from the center of the screen
                    xerror = x - frame.shape[1] // 2
                    yerror = y - frame.shape[0] // 2

                    await sent_message(client, x, y, xerror, yerror)

                    # target is ploted on the screen
                    cv2.circle(frame, (x, y), 5, (0, 0, 255), -1)
                    cv2.putText(frame, "TARGET", (x, y - 10), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 0, 255), 2)

            cv2.imshow("YOLO Object Detection", frame)

            if cv2.waitKey(1) & 0xFF == ord('q'):
                break

        cap.release()
        cv2.destroyAllWindows()

if __name__ == "__main__":
    asyncio.run(main())
