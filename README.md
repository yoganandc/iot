## A Distributed Approach to Traffic Management

This project serves as a demonstration for how distance vector algorithms could be used in road networks 
to manage traffic in a distributed fault-tolerant manner.

Nodes at every intersection are equipped with wireless radios that can communicate with nodes at adjacent
intersections and vehicles approaching the intersection. By estimating traffic outgoing in each direction
and exchanging information with other nodes, optimal paths are discovered for all destinations.

[![Demo](https://i.imgur.com/87mekYA.png)](https://vimeo.com/284643990 "Demo")

### The algorithm

The nodes implement a distance vector protocol based on the Bellman-Ford equation:

**D<sub>x</sub>(y) = min<sub>v</sub>{c(x,v) + D<sub>v</sub>(y)}**

where **D<sub>x</sub>** is the distance vector for node **x**, **v** is set of neighbors of **x**, and **c(x,v)** is the cost along edge **xv**. 

By exchanging distance vectors with its neighbors, each node's own distance vector converges to the shortest path without knowledge about the entire graph.

### Components

#### Computer

Within the scope of this project, nodes cannot discover their neighbors automatically nor can they sense the amount of traffic 
flowing along each outgoing edge. This information (about neighbors and link costs to them) is periodically sent from an XBee module attached to a computer running [this](main.py) script.

The computer is also used to start the car towards its destination, and update edge weights.

#### Nodes

Each node is equipped with an XBee module which it uses to listen for links and their cost, send out distance vectors to linked
nodes, and to respond to the car's request for directions.

#### Car

The car gets its command from the computer, and then sends out a request every time it hits an intersection to the corresponding
node asking it which direction to turn (left, right, or straight).

### Protocol

*Each block in the table is 8 bits in length. Each message begins with a one-byte long message-type identifier.*

**1. MSG_LINK**: Sent from the computer to nodes periodically.

1|2|3|4|5|&nbsp;|&nbsp;|&nbsp;|&nbsp;
:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:
1 | NEIGHBOR-1 | COST-1 | NEIGHBOR-2 | COST-2 | ... | ... | NEIGHBOR-N | COST-N

**2. MSG_DV**: Sent from one node to another neighboring node.

1|2|3|4|5|&nbsp;|&nbsp;|&nbsp;|&nbsp;
:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:
2 | DEST-1 | COST-1 | DEST-2 | COST-2 | ... | ... | DEST-N | COST-N

**3. MSG_DST**: Sent from the computer to car telling it to start heading towards a destination.

1|2|3|4
:---:|:---:|:---:|:---:
3 | SRC-NODE | NEXT-NODE | DEST-NODE

**4. MSG_REQ**: After detecting an intersection the car sends this message to the node at that intersection.

1|2|3
:---:|:---:|:---:
4 | PREV-NODE | DEST-NODE

**5. MSG_RES**: Sent back from the node to car in response to a MSG_REQ.

1|2|3
:---:|:---:|:---:
5 | NEXT-NODE | DIRECTION
