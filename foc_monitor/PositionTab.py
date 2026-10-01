from collections import deque
from PySide6.QtWidgets import QWidget, QVBoxLayout
import pyqtgraph as pg


class PositionTab(QWidget):
    def __init__(self):
        super().__init__()

        self.plot = pg.PlotWidget(title="Electrical Angle")
        self.plot.setBackground("w")
        self.plot.showGrid(x=True, y=True, alpha=0.3)
        self.plot.addLegend()
        
        self.curve_theta_observer = self.plot.plot( pen=pg.mkPen((180, 0, 180), width=2), name="θ observer")
        self.curve_theta_ref= self.plot.plot( pen=pg.mkPen((0, 180, 180), width=2), name="θ reference")

        self.theta_observer = deque(maxlen=200)
        self.theta_ref = deque(maxlen=200)
        self._scale_div = 0

        layout = QVBoxLayout()
        layout.addWidget(self.plot)
        self.setLayout(layout)

    def update(self, d):
        self.theta_observer.append(d["theta_observer_rad"])
        self.theta_ref.append(d["theta_reference_rad"])
        self.curve_theta_observer.setData(self.theta_observer)
        self.curve_theta_ref.setData(self.theta_ref)
