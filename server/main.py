import bluetooth

# Option 1: Discover nearby devices and find the ESP32
print("Scanning for devices...")

nearby_devices = bluetooth.discover_devices(duration=8, lookup_names=True)
esp32_addr = None

for addr, name in nearby_devices:
    print(f"Found {name} - {addr}")
    if name == "TURC2C":  # Change to your ESP32 Bluetooth name if different
        esp32_addr = addr
        break

if esp32_addr is None:
    print("ESP32 device not found. Make sure it is powered on and discoverable.")
    exit(1)

# Option 2: If you know the MAC address, you can set it directly
esp32_addr = "24:DC:C3:99:0E:6C" # Replace with your ESP32's Bluetooth address

port = 1  # RFCOMM port; SPP usually uses port 1

# Create an RFCOMM Bluetooth socket and connect to the ESP32
sock = bluetooth.BluetoothSocket(bluetooth.RFCOMM)
try:
    sock.connect((esp32_addr, port))
    print(f"Connected to ESP32 at {esp32_addr}")

    # Send a message to the ESP32
    message = "Monkey 1 2 3"
    sock.send(message)
    print("Sent:", message)

    # Wait for a response (adjust buffer size and timeout as needed)
    data = sock.recv(1024)
    print("Received:", data.decode('utf-8'))

except bluetooth.btcommon.BluetoothError as err:
    print("Bluetooth error:", err)
finally:
    sock.close()
    print("Connection closed.")
