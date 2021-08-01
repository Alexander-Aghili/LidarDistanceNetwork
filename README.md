# Multi-Drone Formation Network
The drone network has three main components. The head node, the host node, and the resultant client nodes.

1. The head node is the controlling node. It starts the WLAN network, hosts the server, and communicates with the clients. 
This drone with this node on it doesn't need to be connected to the network as it just needs to be controlled by any means, such as an RC, and have the head node device attached, such as the Raspberry Pi. Raspberry Pi has a Lidar sensor attached and takes in that data and processes it.

2. The host node is a static point that communicates with the head node. It can send data such as the target distance or formation type.

3. The last component are the client nodes. They recieve data from the head node and adjust their positioning based on that data. They also send relevant data such as IMU based altitude to the head node. 

![Node-Diagram](https://github.com/Alexander-Aghili/MultiDroneFormationNetwork/edit/main/diagrams/drone-swarm-structure.svg)
