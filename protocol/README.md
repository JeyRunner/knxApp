# Protocol
Protocol to communicate from client with server.
<hr>
<br>

## Protocol Structure
### Basic Packet
Byte    |Description
--------|------------
0       |Packettype
<br>

### Knx Tunnel Packet
Byte    |Description
--------|------------
0       |Packettype = '0x10'
1       |knxSource Address (High Byte)
2       |knxSource Address (Low  Byte)
3       |knxDestination Address (High Byte)
4       |knxDestination Address (Low  Byte)
5       |Datatype (see table below)
6       |Data
7       |Data
...     |Data

#### Knx Tunnel Packet Datatypes
Hex     |Description    |Data(6.Byte) (Hex) Description
--------|----------------------------------
01      |switch data    |00 - off \| 01 - on
02      |dim data       |

<br>
### Knx Device
Byte    |Description
--------|------------
0       |Packettype = '0x20'
...     |Device ...