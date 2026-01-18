# MUST Connect - Setup Guide

## Prerequisites

### Install Qt6 Dependencies
```bash
sudo ./scripts/install-qt6-deps.sh
```

This installs:
- qt6-base-dev
- qt6-declarative-dev  
- qt6-charts-dev
- cmake
- build-essential

## Building

### Build Server
```bash
cd server
mkdir -p build && cd build
cmake ..
cmake --build .
```

### Build Client
```bash
cd client
mkdir -p build && cd build
cmake ..
cmake --build .
```

### Or use VSCode
- Press `Ctrl+Shift+B` to build all
- Press `F5` and select configuration to run/debug

## Running

### Start Server
```bash
./server/build/must-server
```

### Start Client
```bash
./client/build/must-client
```

### Or use VSCode
- Run and Debug panel → Select "Run Server + Client"

## Project Status

✅ **Server** - Complete (reads Modbus, sends UDP)
✅ **Client** - Complete (receives UDP, displays dashboard/charts/logs)
✅ **Build System** - CMake configured for both
✅ **VSCode Integration** - Launch configs and tasks ready

## Next Steps

1. Install Qt6: `sudo ./scripts/install-qt6-deps.sh`
2. Build: Press `Ctrl+Shift+B` in VSCode
3. Run: Press `F5` and select "Run Server + Client"
