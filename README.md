# Multi-Drone Formation Network

This is designed at an open source drone networked non-relient on GPS for formation to ensure short distances operate. The solution I came up with is to use Lidar in a similar method to ground-based radar Air Traffic Control uses. What does this mean? It means that some leader, referred to as the Head Node, will use its Altitude and Lidar data to create a map, along with hosting a WLAN Network and Server. Client Nodes can connect to that Network and Server, giving their altitude data to the head node and receiving positional data from the head node. This allows the head node to determine whether a client has left its needed position, and send that client information to move. Host nodes can also connect to the head nodes server. The can send formation alterations and receive data but they are not required for the drone swarm to function. The head node also doesn't need to communicate with the lead drone itself. If you have access to the flight computer and can host the required code on the computer, then there is no problem with the head node communcating with the lead drone. However, as long as the head node is attached to the lead drone and has access to Lidar and altitude data, whether that is third-party or otherwise, it need not be connected. Altitude data is required from client nodes as well. 

The drone network has three main components. The head node, the host node, and the resultant client nodes.

1. The head node is the controlling node. It starts the WLAN network, hosts the server, and communicates with the clients. 
This drone with this node on it doesn't need to be connected to the network as it just needs to be controlled by any means, such as an RC, and have the head node device attached, such as the Raspberry Pi. Raspberry Pi has a Lidar sensor attached and takes in that data and processes it.

2. The host node is a static point that communicates with the head node. It can send data such as the target distance or formation type.

3. The last component are the client nodes. They recieve data from the head node and adjust their positioning based on that data. They also send relevant data such as IMU based altitude to the head node. 

![Node-Diagram](https://github.com/Alexander-Aghili/MultiDroneFormationNetwork/blob/main/diagrams/Drone%20Swarm%20Structure.svg)
