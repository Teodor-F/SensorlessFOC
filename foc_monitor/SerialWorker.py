import serial    
import struct
import inspect
from PySide6.QtCore import QThread, Signal

from FocProtocol import *


class SerialWorker(QThread):
    new_data = Signal(dict)
    status = Signal(str)

    def __init__(self):
        super().__init__()
        self.running = False
        self.ser = None

    def start_port(self, port, baud):
        self.port = port
        self.baud = baud
        self.running = True
        self.start()

    def run(self):
        try:
            self.ser = serial.Serial(self.port, self.baud, timeout=0.1)
            self.status.emit(f"Connected to {self.port}")
        except Exception as e:
            self.status.emit(str(e))
            return

        buf = bytearray()

        while self.running:
            buf += self.ser.read(256)

            while len(buf) >= FOC_TELEMETRY_FRAME_SIZE:
                # resync on header LSB
                if buf[0] != (FOC_FRAME_HEADER & 0xFF):
                    buf.pop(0)
                    continue

                frame = bytes(buf[:FOC_TELEMETRY_FRAME_SIZE])
                del buf[:FOC_TELEMETRY_FRAME_SIZE]

                d = parse_telemetry_frame(frame)
                if d is not None:
                    self.new_data.emit(d)

        self.ser.close()
        self.status.emit("Disconnected")

    def stop(self):
        self.running = False

    def send_start(self):
        if self.ser is None:
            return
        frame = create_start_frame()
        self.ser.write(frame)


    def send_stop(self):
        if self.ser is None:
            return
        frame = create_stop_frame()
        self.ser.write(frame)


    def send_speed(self, rpm):
        if self.ser is None:
            return
        frame = create_set_speed_frame(rpm)
        self.ser.write(frame)