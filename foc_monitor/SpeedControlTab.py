from collections import deque

from PySide6.QtCore import Qt, Signal
from PySide6.QtGui import QFont

from PySide6.QtWidgets import (
    QWidget,
    QVBoxLayout,
    QLabel,
    QSlider,
    QPushButton,
    QHBoxLayout
)

import pyqtgraph as pg


class SpeedControlTab(QWidget):

    startRequested = Signal()
    stopRequested = Signal()
    speedChanged = Signal(int)

    def __init__(self):
        super().__init__()

        rpm_font = QFont()
        rpm_font.setPointSize(28)
        rpm_font.setBold(True)

        self.rpm_display = QLabel("0 RPM")
        self.rpm_display.setFont(rpm_font)
        self.rpm_display.setAlignment(Qt.AlignCenter)

        small_font = QFont()
        small_font.setPointSize(9)

        self.actual_label = QLabel("Actual Speed: 0 RPM")
        self.actual_label.setFont(small_font)
        self.actual_label.setAlignment(Qt.AlignCenter)

        self.speed_label = QLabel("Reference: 500 RPM")
        self.speed_label.setFont(small_font)
        self.speed_label.setAlignment(Qt.AlignCenter)


        self.slider = QSlider(Qt.Horizontal)

        self.slider.setRange(10, 80)
        self.slider.setValue(10)

        self.slider.setTickPosition(QSlider.TicksBelow)
        self.slider.setTickInterval(1)

        self.slider.valueChanged.connect(self.on_slider)

        self.start_btn = QPushButton("Start")
        self.stop_btn = QPushButton("Stop")


        self.start_btn.clicked.connect(self.start_clicked)
        self.stop_btn.clicked.connect(self.stop_clicked)
        self.stop_btn.setEnabled(False)
        self.slider.setEnabled(False)

     

        btn_layout = QHBoxLayout()
        btn_layout.addWidget(self.start_btn)
        btn_layout.addWidget(self.stop_btn)


        self.plot = pg.PlotWidget(title="Mechanical Speed")
        self.plot.setYRange(0, 4500, padding=0)
        self.plot.setBackground("w")
        self.plot.showGrid(x=True, y=True, alpha=0.3)
        self.plot.addLegend()

        self.curve_pll_speed_rpm = self.plot.plot(
            pen=pg.mkPen((180, 0, 180), width=2),
            name="Observer Speed [RPM]"
        )

        self.curve_ref_speed_rpm = self.plot.plot(
            pen=pg.mkPen((0, 0, 180), width=2),
            name="Reference Speed [RPM]"
        )

        self.pll_speed_rpm = deque(maxlen=200)
        self.ref_speed_rpm = deque(maxlen=200)


        layout = QVBoxLayout()

        layout.addWidget(self.rpm_display)
        layout.addWidget(self.actual_label)
        layout.addWidget(self.speed_label)

        layout.addWidget(self.slider)
        layout.addLayout(btn_layout)

        layout.addWidget(self.plot)

        self.setLayout(layout)

    def on_slider(self, value):

        rpm = value * 50

        self.speed_label.setText(
            f"Reference: {rpm} RPM"
        )

        self.speedChanged.emit(rpm)

    def update(self, d):

        rpm = int(d["speed_observer_rpm"])

        self.rpm_display.setText(
            f"{rpm} RPM"
        )

        self.actual_label.setText(
            f"Actual Speed: {rpm} RPM"
        )

        self.pll_speed_rpm.append(
            d["speed_observer_rpm"]
        )

        self.ref_speed_rpm.append(
            d["speed_reference_rpm"]
        )

        self.curve_pll_speed_rpm.setData(
            self.pll_speed_rpm
        )

        self.curve_ref_speed_rpm.setData(
            self.ref_speed_rpm
        )

    def start_clicked(self):
        self.start_btn.setEnabled(False)
        self.stop_btn.setEnabled(True)
        self.slider.setEnabled(True)
        self.startRequested.emit()


    def stop_clicked(self):
        self.start_btn.setEnabled(True)
        self.stop_btn.setEnabled(False)
        self.slider.setValue(10)
        self.slider.setEnabled(False)
        self.stopRequested.emit()