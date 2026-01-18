# MUST Connect

Modbus RTU to UDP bridge for MUST inverters. Reads inverter data via Modbus serial and broadcasts it over UDP for monitoring applications.
This is my home pet-project for fun and is not intended to support wide range of similar devices. But if someone find it useful we can think of extend supported devices list.
Example of client app is here: **TBD**

## Features

- **Modbus RTU Reader** - Connects to MUST inverters via serial port
- **UDP Broadcaster** - Sends real-time data over UDP
- **Configurable Registers** - JSON-based register configuration with scaling
- **Cross-Platform** - Linux support with libmodbus

## Architecture

```
┌─────────────────┐      ┌──────────────────┐      ┌─────────────┐
│  MUST Inverter  │─────▶│  Modbus Reader   │─────▶│ UDP Sender  │
│   (Serial RTU)  │      │  (Thread-Safe    │      │             │
└─────────────────┘      │      Queue)      │      └─────────────┘
                         └──────────────────┘             │
                                                           ▼
                                                    ┌─────────────┐
                                                    │ UDP Clients │
                                                    │  (Monitor)  │
                                                    └─────────────┘
```

## Prerequisites

- C++11 compiler (g++, clang++)
- CMake 3.10+
- libmodbus-dev
- Linux (tested on Ubuntu/Debian)

### Install Dependencies

```bash
sudo apt-get update
sudo apt-get install build-essential cmake libmodbus-dev
```

## Building

### Quick Build

```bash
cd server
mkdir -p build && cd build
cmake ..
cmake --build .
```

### VSCode Build

Press `Ctrl+Shift+B` to build all targets.

## Configuration

Edit `config.json`:

```json
{
  "modbus": {
    "port": "/dev/ttyUSB0",
    "baudrate": 19200,
    "parity": "N",
    "dataBits": 8,
    "stopBits": 1,
    "deviceId": 4
  },
  "udp": {
    "host": "127.0.0.1",
    "port": 9999
  },
  "pollInterval": 1000,
  "registers": [
    {
      "address": 10104,
      "name": "Battery Voltage, V",
      "scale": 0.1,
      "isSigned": false,
      "dataType": "UInt16"
    }
  ]
}
```

## Running

### Command Line

```bash
./server/build/must-server --config ../../config.json
```

### VSCode

Run and Debug panel → Select "Run Server"

## UDP Protocol

Format: `timestamp|name1:value1|name2:value2|...`

Example:
```
1737123456|Battery Voltage, V:52.4|Battery Current, A:-15|Battery State of Charge, %:85
```

See [docs/udp_protocol.md](docs/udp_protocol.md) for parsing examples in Python, C++, and JavaScript.

## Testing

### Test Register Reading

```bash
cd scripts
python3 test-registers.py [device_id]
```

### Scan Registers

```bash
cd scripts
python3 scan-registers.py [device_id] > output.txt
```

## Project Structure

```
must-connect/
├── server/              # Modbus to UDP server
│   ├── src/
│   │   ├── main.cpp
│   │   ├── modbus_reader.cpp
│   │   ├── udp_sender.cpp
│   │   ├── config.cpp
│   │   └── threadsafe_queue.h
│   └── CMakeLists.txt
├── scripts/             # Python test utilities
│   ├── test-registers.py
│   └── scan-registers.py
├── docs/                # Documentation
│   ├── udp_protocol.md
│   └── SETUP.md
└── config.json          # Configuration file
```

## Supported Registers

- Battery Voltage, Current, SOC
- Inverter Temperature, Work Mode
- Load Power, Percentage
- AC Output Voltage
- PV Voltage
- Fault/Protection Status

## Troubleshooting

### Connection Timeout

Add delay between register reads (already implemented: 50ms).

### Permission Denied on Serial Port

```bash
sudo usermod -a -G dialout $USER
# Logout and login again
```

### No Data Received

Check device ID and serial port settings match your inverter.

## License

MIT

## Contributing

Pull requests welcome. For major changes, open an issue first.
