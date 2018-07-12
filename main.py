#!/usr/bin/env python3

import threading
import queue
import cmd
import json
from digi.xbee.devices import Raw802Device
from digi.xbee.models.address import XBee16BitAddress
from constants import *


class Graph:
    """
    Represents a thread-safe basic graph implementation
    """
    def __init__(self):
        self._graph = {
            1: [[2, 60], [3, 1]],
            2: [[1, 60], [3, 50]],
            3: [[1, 1], [2, 50]]
        }
        self._lock = threading.Lock()
        
    def snapshot(self):
        ret = {}
        with self._lock:
            for node in self._graph:
                ret[node] = []
                for edge in self._graph[node]:
                    ret[node].append([edge[0], edge[1]])
        return ret
    
    def update(self, src, dest, weight):
        with self._lock:
            for edge in self._graph[src]:
                if edge[0] == dest:
                    edge[1] = weight


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
        self._stop_flag = True
        self._dispatcher.join()
        self._q.join()
        self._xbee.close()
        
    def send_link_update(self, to, snapshot):
        self._q.put((to, snapshot))
        
    def _dispatch(self):
        dispatch_table = {
            MSG_LINK: self._send_link_update
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
        snapshot = msg[1]
        
        pkt = bytearray(MSG_LINK.to_bytes(1, BYTEORDER))
        for edge in snapshot:
            pkt += edge[0].to_bytes(2, BYTEORDER)
            pkt += edge[1].to_bytes(2, BYTEORDER)
            
        self._xbee.send_data_16(XBee16BitAddress.from_hex_string(ADDRESS[to]), pkt)
        
        
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
    def __init__(self, graph):
        super().__init__()
        self._graph = graph
        
    intro = 'Welcome to the traffic manager shell. Type help or ? to list commands.\n'
    prompt = '> '

    def do_update(self, arg1, arg2, arg3):
        """Update edge from weight from src dst: update 1 2 50"""
        self._graph.update(int(arg1), int(arg2), int(arg3))
        print("Updated edge from %s to %s to %s" % (arg1, arg2, arg3))
        
    def do_show(self):
        """Display graph"""
        print(json.dumps(self._graph.snapshot(), indent=4))


def main():
    graph = Graph()
    xbee = Raw802Device(PORT, BAUD)
    xbee.open()
    
    XBeeDaemon(graph, xbee).start()
    Prompt(graph).cmdloop()


if __name__ == "__main__":
    main()
