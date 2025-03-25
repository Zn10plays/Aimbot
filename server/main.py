import asyncio
from bleak import BleakClient, BleakScanner
from bleak.backends.characteristic import BleakGATTCharacteristic
import cv2

ESP_MAC = "24:DC:C3:99:0E:6E"

SERVICE_UUID = "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
CHARACTERISTIC_UUID_RX = "beb5483e-36e1-4688-b7f5-ea07361b26a8"
CHARACTERISTIC_UUID_TX = "beb5483f-36e1-4688-b7f5-ea07361b26a8"

def handle_notification(characteristic: BleakGATTCharacteristic, data: bytearray):
    print(f"Notification from {characteristic.description}: {data.decode()}")

async def main():
    async with BleakClient(ESP_MAC) as client:
        print(f"Connected: {client.is_connected}")

        await client.start_notify(CHARACTERISTIC_UUID_TX, handle_notification)

        message = f'ERROR '