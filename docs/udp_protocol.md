# UDP Protocol Documentation

## Current Text Protocol
Format: `timestamp|name1:value1|name2:value2|...`

Example:
```
1737123456|Battery Voltage, V:52.4|Battery Current, A:-15|Battery State of Charge, %:85|...
```

## Parsing Structure (Python)

```python
import json
from dataclasses import dataclass
from typing import Optional

@dataclass
class InverterData:
    timestamp: int
    battery_voltage: float
    battery_current: float
    battery_soc: float
    inverter_temperature: float
    load_power: float
    load_percentage: float
    ac_output_voltage: float
    inverter_work_mode: int
    pv_voltage: float
    fault_status: int
    
    @classmethod
    def from_udp(cls, data: str) -> Optional['InverterData']:
        """Parse UDP text protocol"""
        parts = data.split('|')
        if len(parts) < 2:
            return None
        
        timestamp = int(parts[0])
        values = {}
        
        for part in parts[1:]:
            if ':' in part:
                name, value = part.split(':', 1)
                values[name] = float(value)
        
        return cls(
            timestamp=timestamp,
            battery_voltage=values.get('Battery Voltage, V', 0.0),
            battery_current=values.get('Battery Current, A', 0.0),
            battery_soc=values.get('Battery State of Charge, %', 0.0),
            inverter_temperature=values.get('Inverter Temperature, °C', 0.0),
            load_power=values.get('Load Power, W', 0.0),
            load_percentage=values.get('Load Percentage, %', 0.0),
            ac_output_voltage=values.get('AC Output Voltage, V', 0.0),
            inverter_work_mode=int(values.get('Inverter Work Mode', 0)),
            pv_voltage=values.get('PV Voltage, V', 0.0),
            fault_status=int(values.get('Fault / Protection Status Bitmap', 0))
        )
    
    def to_json(self) -> str:
        """Convert to JSON for easy UI consumption"""
        return json.dumps({
            'timestamp': self.timestamp,
            'battery': {
                'voltage': self.battery_voltage,
                'current': self.battery_current,
                'soc': self.battery_soc
            },
            'inverter': {
                'temperature': self.inverter_temperature,
                'work_mode': self.inverter_work_mode
            },
            'load': {
                'power': self.load_power,
                'percentage': self.load_percentage
            },
            'ac_output': {
                'voltage': self.ac_output_voltage
            },
            'pv': {
                'voltage': self.pv_voltage
            },
            'status': {
                'fault': self.fault_status
            }
        })
```

## Parsing Structure (C++)

```cpp
#pragma once
#include <string>
#include <map>
#include <sstream>

struct InverterData {
    uint64_t timestamp;
    double battery_voltage;
    double battery_current;
    double battery_soc;
    double inverter_temperature;
    double load_power;
    double load_percentage;
    double ac_output_voltage;
    int inverter_work_mode;
    double pv_voltage;
    int fault_status;
    
    static InverterData fromUdp(const std::string& data) {
        InverterData result = {};
        std::map<std::string, double> values;
        
        size_t pos = 0;
        size_t next = data.find('|');
        result.timestamp = std::stoull(data.substr(0, next));
        pos = next + 1;
        
        while (pos < data.length()) {
            next = data.find('|', pos);
            if (next == std::string::npos) next = data.length();
            
            std::string part = data.substr(pos, next - pos);
            size_t colon = part.find(':');
            if (colon != std::string::npos) {
                std::string name = part.substr(0, colon);
                double value = std::stod(part.substr(colon + 1));
                values[name] = value;
            }
            pos = next + 1;
        }
        
        result.battery_voltage = values["Battery Voltage, V"];
        result.battery_current = values["Battery Current, A"];
        result.battery_soc = values["Battery State of Charge, %"];
        result.inverter_temperature = values["Inverter Temperature, °C"];
        result.load_power = values["Load Power, W"];
        result.load_percentage = values["Load Percentage, %"];
        result.ac_output_voltage = values["AC Output Voltage, V"];
        result.inverter_work_mode = static_cast<int>(values["Inverter Work Mode"]);
        result.pv_voltage = values["PV Voltage, V"];
        result.fault_status = static_cast<int>(values["Fault / Protection Status Bitmap"]);
        
        return result;
    }
};
```

## Parsing Structure (JavaScript/TypeScript)

```typescript
interface InverterData {
    timestamp: number;
    battery: {
        voltage: number;
        current: number;
        soc: number;
    };
    inverter: {
        temperature: number;
        workMode: number;
    };
    load: {
        power: number;
        percentage: number;
    };
    acOutput: {
        voltage: number;
    };
    pv: {
        voltage: number;
    };
    status: {
        fault: number;
    };
}

function parseUdpData(data: string): InverterData | null {
    const parts = data.split('|');
    if (parts.length < 2) return null;
    
    const timestamp = parseInt(parts[0]);
    const values: Record<string, number> = {};
    
    for (let i = 1; i < parts.length; i++) {
        const [name, value] = parts[i].split(':');
        if (name && value) {
            values[name] = parseFloat(value);
        }
    }
    
    return {
        timestamp,
        battery: {
            voltage: values['Battery Voltage, V'] || 0,
            current: values['Battery Current, A'] || 0,
            soc: values['Battery State of Charge, %'] || 0
        },
        inverter: {
            temperature: values['Inverter Temperature, °C'] || 0,
            workMode: values['Inverter Work Mode'] || 0
        },
        load: {
            power: values['Load Power, W'] || 0,
            percentage: values['Load Percentage, %'] || 0
        },
        acOutput: {
            voltage: values['AC Output Voltage, V'] || 0
        },
        pv: {
            voltage: values['PV Voltage, V'] || 0
        },
        status: {
            fault: values['Fault / Protection Status Bitmap'] || 0
        }
    };
}
```

## UDP Client Example (Python)

```python
import socket

def udp_client(host='127.0.0.1', port=9999):
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.bind((host, port))
    
    print(f"Listening on {host}:{port}")
    
    while True:
        data, addr = sock.recvfrom(4096)
        message = data.decode('utf-8')
        
        inverter_data = InverterData.from_udp(message)
        if inverter_data:
            print(f"\nBattery: {inverter_data.battery_voltage}V, {inverter_data.battery_soc}%")
            print(f"Load: {inverter_data.load_power}W ({inverter_data.load_percentage}%)")
            print(f"PV: {inverter_data.pv_voltage}V")
            print(inverter_data.to_json())

if __name__ == '__main__':
    udp_client()
```
