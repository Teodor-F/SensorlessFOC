import sys

from PySide6.QtWidgets import (
    QApplication, QMainWindow, QWidget,
    QVBoxLayout, QHBoxLayout,
    QComboBox, QPushButton, QTabWidget
)
import serial.tools.list_ports

from SerialWorker import SerialWorker
from PhaseCurrentsTab import PhaseCurrentsTab
from AlphaBetaCurrentsTab import AlphaBetaCurrentsTab
from DQCurrentsTab import DQCurrentsTab
from AlphaBetaEmfTab import AlphaBetaEmfTab
from SpeedControlTab import SpeedControlTab
from PositionTab import PositionTab
from SpeedControlTab import SpeedControlTab

from PySide6.QtGui import QPalette, QColor
from PySide6.QtCore import Qt

BAUDRATE = 921600


def list_com_ports():
    return [p.device for p in serial.tools.list_ports.comports()]


class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("STM32G4 FOC Monitor")
        self.resize(900, 600)

        # ---------- Top bar ----------
        self.port_box = QComboBox()
        self.refresh_btn = QPushButton("Refresh")
        self.connect_btn = QPushButton("Connect")

        self.refresh_btn.clicked.connect(self.refresh_ports)
        self.connect_btn.clicked.connect(self.toggle_connection)

        top_bar = QHBoxLayout()
        top_bar.addWidget(self.port_box)
        top_bar.addWidget(self.refresh_btn)
        top_bar.addWidget(self.connect_btn)

        # ---------- Tabs ----------
        self.tabs = QTabWidget()
        self.tab_currents = PhaseCurrentsTab()
        self.tab_alfa_beta_currents = AlphaBetaCurrentsTab()
        self.tab_dq_currents = DQCurrentsTab()
        self.tab_emf      = AlphaBetaEmfTab()
        self.tab_position = PositionTab()
        self.tab_speed_control = SpeedControlTab()

        self.tabs.addTab(self.tab_currents,             "3-Phase Currents")
        self.tabs.addTab(self.tab_alfa_beta_currents,   "Alfa-Beta Currents")
        self.tabs.addTab(self.tab_dq_currents,          "DQ Currents")
        self.tabs.addTab(self.tab_emf,                  "Alpha-Beta EMF")
        self.tabs.addTab(self.tab_position,             "Electrical Angle")
        self.tabs.addTab(self.tab_speed_control,        "Speed Control")

        # ---------- Main layout ----------
        layout = QVBoxLayout()
        layout.addLayout(top_bar)
        layout.addWidget(self.tabs)

        central = QWidget()
        central.setLayout(layout)
        self.setCentralWidget(central)

        # ---------- Serial worker ----------
        self.worker = SerialWorker()
        self.worker.new_data.connect(self.on_data)
        self.worker.status.connect(self.statusBar().showMessage)

        self.tab_speed_control.startRequested.connect(self.worker.send_start)
        self.tab_speed_control.stopRequested.connect(self.worker.send_stop)
        self.tab_speed_control.speedChanged.connect(self.worker.send_speed)

        self.refresh_ports()

    # ---------- UI callbacks ----------
    def refresh_ports(self):
        self.port_box.clear()
        self.port_box.addItems(list_com_ports())

    def toggle_connection(self):
        if self.worker.isRunning():
            self.worker.stop()
            self.connect_btn.setText("Connect")
        else:
            port = self.port_box.currentText()
            if not port:
                self.statusBar().showMessage("No COM port selected")
                return

            self.worker.start_port(port, BAUDRATE)
            self.connect_btn.setText("Disconnect")

    # ---------- Data dispatch ----------
    def on_data(self, d):
        self.tab_currents.update(d)
        self.tab_alfa_beta_currents.update(d)
        self.tab_dq_currents.update(d)
        self.tab_emf.update(d)
        self.tab_position.update(d)
        self.tab_speed_control.update(d)

    def closeEvent(self, event):
        if self.worker.isRunning():
            self.worker.stop()
        event.accept()


def apply_light_theme(app: QApplication):
    palette = QPalette()
    palette.setColor(QPalette.Window, QColor(245, 245, 245))
    palette.setColor(QPalette.WindowText, Qt.black)
    palette.setColor(QPalette.Base, QColor(255, 255, 255))
    palette.setColor(QPalette.Text, Qt.black)
    palette.setColor(QPalette.Button, QColor(240, 240, 240))
    palette.setColor(QPalette.ButtonText, Qt.black)
    palette.setColor(QPalette.Highlight, QColor(76, 163, 224))
    palette.setColor(QPalette.HighlightedText, Qt.white)
    app.setPalette(palette)


if __name__ == "__main__":
    app = QApplication(sys.argv)
    apply_light_theme(app)
    w = MainWindow()
    w.show()
    sys.exit(app.exec())