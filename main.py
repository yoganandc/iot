#!/usr/bin/env python3

import threading
import queue
import cmd
import time
from digi.xbee.devices import Raw802Device
from digi.xbee.exception import XBeeException
from digi.xbee.models.address import XBee16BitAddress
from constants import *


class Graph:
    """
    Represents a thread-safe basic graph implementation
    """
    def __init__(self):
        self._graph = {
            0: [[8, 8, NORTH], [1, 10, EAST], [2, 8, SOUTH], [9, 24, WEST]],
            1: [[7, 2, NORTH], [6, 15, EAST], [3, 52, SOUTH], [0, 10, WEST]],
            2: [[0, 8, NORTH], [3, 22, EAST], [4, 60, SOUTH], [10, 17, WEST]],
            3: [[1, 52, NORTH], [5, 7, EAST], [4, 13, SOUTH], [2, 22, WEST]],
            4: [[2, 60, NORTH], [3, 13, EAST], [12, 11, SOUTH], [11, 23, WEST]],
            5: [[3, 7, WEST]],
            6: [[1, 15, WEST]],
            7: [[1, 2, SOUTH]],
            8: [[0, 8, SOUTH]],
            9: [[0, 24, EAST]],
            10: [[2, 17, EAST]],
            11: [[4, 23, EAST]],
            12: [[4, 11, NORTH]]
        }
        self._paths = [[None for _ in range(len(self._graph))] for _ in range(len(self._graph))]
        self._costs = [None for _ in range(len(self._graph))]
        self._compute()
        self._lock = threading.Lock()
        
    def __len__(self):
        return len(self._graph)
        
    def snapshot(self):
        ret = {}
        with self._lock:
            for node in self._graph:
                ret[node] = []
                for edge in self._graph[node]:
                    ret[node].append([edge[0], edge[1], edge[2]])
        return ret
    
    def update(self, src, dest, weight):
        with self._lock:
            for edge in self._graph[src]:
                if edge[0] == dest:
                    edge[1] = weight
            
            self._compute()
                    
    def path(self, src, dst):
        return self._costs[src][dst], self._paths[src][dst]

    def _djikstra(self, source):
        n = len(self._graph)
    
        parent = [None for _ in range(n)]
        dist = [INF for _ in range(n)]
        dist[source] = 0
    
        completed = set()
    
        def delete_min():
            min_dist = INF
            lowest = -1
        
            for i in range(n):
                if dist[i] < min_dist and i not in completed:
                    min_dist = dist[i]
                    lowest = i
        
            completed.add(lowest)
            return lowest
    
        while len(completed) != n:
            node = delete_min()
        
            for edge in self._graph[node]:
                neighbor = edge[0]
                weight = edge[1]
            
                if dist[neighbor] > dist[node] + weight:
                    dist[neighbor] = dist[node] + weight
                    parent[neighbor] = node
    
        return dist, parent
    
    def _compute(self):
        for node in self._graph:
            dist, parents = self._djikstra(node)
            self._costs[node] = dist
            
            for dst in self._graph:
                path = [dst]
                parent = parents[dst]  # type: int
                while parent is not None:
                    path.insert(0, parent)
                    parent = parents[parent]
                    
                self._paths[node][dst] = path


class XBeeTransmitter:
    """
    This class maintains a connection to the XBee
    module attached via serial port and sends messages
    to all nodes present on the network in a thread-safe
    manner.
    """
    def __init__(self, xbee):
        # serial connection
        self._xbee = xbee
        
        # queue to messages to be sent
        self._q = queue.Queue()
        
        # thread to dispatch messages
        self._dispatcher = threading.Thread(target=self._dispatch)
        
        # flag to tell dispatcher to stop
        self._stop_flag = False
        
    def start(self):
        self._dispatcher.start()
        
    def stop(self):
        # Stop dispatcher
        self._stop_flag = True
        self._dispatcher.join()
        
        # Throw out remaining items
        while not self._q.empty():
            self._q.get()
            self._q.task_done()
        
        # Get the queue to join
        self._q.join()
        
    def send_link_update(self, to, snapshot):
        self._q.put((MSG_LINK, (to, snapshot)))
        
    def send_dst(self, dst):
        self._q.put((MSG_DST, dst))
        
    def _dispatch(self):
        dispatch_table = {
            MSG_LINK: self._send_link_update,
            MSG_DST: self._send_dst
        }
        
        while not self._stop_flag:
            try:
                msg = self._q.get(timeout=0.1)
                dispatch_table[msg[0]](msg[1])
                self._q.task_done()
            except queue.Empty:
                pass
        
    def _send_link_update(self, msg):
        to = msg[0]
        if to >= len(ADDRESS):  # don't send to nodes that don't have addresses
            return
        
        snapshot = msg[1]
        
        pkt = bytearray(MSG_LINK.to_bytes(1, BYTEORDER))
        for edge in snapshot:
            pkt += edge[0].to_bytes(1, BYTEORDER)
            pkt += edge[1].to_bytes(1, BYTEORDER)
            pkt += edge[2].to_bytes(1, BYTEORDER)
            
        def delayed_send():
            delay = to * UPDATE_INTERVAL / len(ADDRESS)
            time.sleep(delay)
            try:
                self._xbee.send_data_16(XBee16BitAddress.from_hex_string(ADDRESS[to]), pkt)
            except XBeeException:
                pass
            
        threading.Thread(target=delayed_send).start()
        
    def _send_dst(self, msg):
        pkt = bytearray(MSG_DST.to_bytes(1, BYTEORDER))
        pkt += msg.to_bytes(1, BYTEORDER)
        try:
            self._xbee.send_data_16(XBee16BitAddress.from_hex_string(ADDRESS_CAR), pkt)
        except XBeeException:
            print('XBeeException')
            pass


class XBeeReceiver:
    """
    polls xbee to see if messages have been received and dumps to log
    """
    def __init__(self, xbee):
        self._xbee = xbee
        
    def start(self):
        pass
    
    def stop(self):
        pass

        
class XBeeDaemon:
    """
    This daemon sends out link messages to all
    routers every n seconds, and also responds
    to requests sent via CLI
    """
    def __init__(self, graph, xbee):
        self._graph = graph
        self._tx = XBeeTransmitter(xbee)
        self._rx = XBeeReceiver(xbee)
        self._timer = RepeatingTimer(self._send_link_updates, UPDATE_INTERVAL)
        
    def _send_link_updates(self):
        graph = self._graph.snapshot()
        for node in graph:
            self._tx.send_link_update(node, graph[node])
        
    def start(self):
        self._rx.start()
        self._tx.start()
        self._timer.start()
    
    def stop(self):
        self._timer.stop()
        self._tx.stop()
        self._rx.stop()
        
    def send_dst(self, dst):
        self._tx.send_dst(dst)


class RepeatingTimer:
    def __init__(self, func, interval):
        self._stopped = threading.Event()
        self._stopped.set()
        
        self._t = None
        self._func = func
        self._interval = interval
    
    def is_running(self):
        return not self._stopped.is_set()
    
    def start(self):
        assert self._stopped.is_set()
        
        self._stopped.clear()
        self._t = threading.Thread(target=self.run)
        self._t.start()
    
    def stop(self):
        assert not self._stopped.is_set()
        
        self._stopped.set()
        self._t.join()
    
    def reset(self):
        self.stop()
        self.start()
    
    def run(self):
        while not self._stopped.wait(self._interval):
            self._func()
            

class Prompt(cmd.Cmd):
    """
    Server is wrapped in a prompt that accepts commands to interact with the graph
    """
    def __init__(self, graph, daemon):
        super().__init__()
        self._graph = graph
        self._daemon = daemon
        
    intro = 'Welcome to the traffic manager shell. Type help or ? to list commands.\n'
    prompt = '> '

    def do_update(self, arg):
        """Update edge from weight from src to dst: update 1 2 50"""
        try:
            args = arg.split()
            src = int(args[0])
            dst = int(args[1])
            weight = int(args[2])
            
            if src < 0 or src > len(self._graph) or dst < 0 or dst > len(self._graph):
                raise ValueError
            
            if weight < 0 or weight >= INF:
                raise ValueError
            
        except (ValueError, IndexError):
            print("unable to parse input")
            return False
            
        self._graph.update(src, dst, weight)
        print("Updated edge from %d to %d to %d" % (src, dst, weight))
        return False
        
    def do_show(self, _):
        """Display graph"""
        snapshot = self._graph.snapshot()
        for node in snapshot:
            print(str(node) + ": ", end='')
            print(snapshot[node])
        return False
    
    def do_path(self, arg):
        """Get shortest path and cost from src to dst: path 1 2"""
        try:
            args = arg.split()
            src = int(args[0])
            dst = int(args[1])

            if src < 0 or src >= len(self._graph) or dst < 0 or dst >= len(self._graph):
                raise ValueError

        except (ValueError, IndexError):
            print("unable to parse input")
            return False
        
        cost, path = self._graph.path(src, dst)
        print("Cost:", cost)
        print("Path:", path)
        return False
    
    def do_dst(self, arg):
        """Update the car's destination"""
        try:
            dst = int(arg)
            
            # Keep dst to terminal nodes for now
            if dst < len(ADDRESS) or dst >= len(self._graph):
                raise ValueError

        except (ValueError, IndexError):
            print("unable to parse input")
            return False
        
        self._daemon.send_dst(dst)
        return False
    
    @staticmethod
    def do_EOF(_):
        print('terminating...')
        return True
    
    help_exit = help_EOF = lambda _: print("stop and exit gracefully")
    do_exit = do_EOF


def main():
    graph = Graph()
    xbee = Raw802Device(PORT, BAUD)
    xbee.open()
    daemon = XBeeDaemon(graph, xbee)
    
    try:
        daemon.start()
        Prompt(graph, daemon).cmdloop()
    finally:
        daemon.stop()
        xbee.close()


if __name__ == "__main__":
    main()
