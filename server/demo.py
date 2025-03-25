import asyncio
from bleak import BleakClient, BleakScanner
from bleak.backends.characteristic import BleakGATTCharacteristic

# Replace with your ESP32's service and characteristic UUIDs
SERVICE_UUID = "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
CHARACTERISTIC_UUID_RX = "beb5483e-36e1-4688-b7f5-ea07361b26a8"
CHARACTERISTIC_UUID_TX = "beb5483f-36e1-4688-b7f5-ea07361b26a8"

def notification_handler(characteristic: BleakGATTCharacteristic, data: bytearray):
    """Simple notification handler which prints the data received."""
    print(f"Notification from {characteristic.description}: {data.decode()}")

async def main():
    print("starting scan...")

    devices = await BleakScanner.discover()
    esp32_device = None
    for d in devices:
        if d.name and "TUTC2C" in d.name:  # Look for a device with "ESP32" in its name
            esp32_device = d
            break

    if esp32_device is None:
        print("Could not find ESP32 device.")
        return

    print(f"Connecting to {esp32_device.name} ({esp32_device.address})")

    async with BleakClient(esp32_device.address) as client:
        print(f"Connected: {client.is_connected}")

        # Enable notifications on the TX characteristic
        await client.start_notify(CHARACTERISTIC_UUID_TX, notification_handler)

        # Send messages in a loop
        counter = 0
        while True:
            message = f"Hello from computer! Count: {counter}"
            print(f"Sending: {message}")
            await client.write_gatt_char(CHARACTERISTIC_UUID_RX, message.encode())
            await asyncio.sleep(5.0)  # Wait 5 seconds between messages
            counter += 1
            if not client.is_connected:
                print("The device is disconnected!")
                break


if __name__ == "__main__":
    asyncio.run(main())