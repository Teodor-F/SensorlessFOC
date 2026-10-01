from collections import deque
from PySide6.QtWidgets import QWidget, QVBoxLayout
import pyqtgraph as pg

from collections import deque
from PySide6.QtWidgets import QWidget, QVBoxLayout
import pyqtgraph as pg

class DQCurrentsTab(QWidget):
    def __init__(self):
        super().__init__()

        self.plot = pg.PlotWidget(title="Park currents (mA)")
        self.plot.setYRange(-100, 1500, padding=0)
        self.plot.setBackground("w")
        self.plot.showGrid(x=True, y=True, alpha=0.3)
        self.plot.addLegend()

        self.curve_id = self.plot.plot(
            pen=pg.mkPen((0, 0, 200), width=2), name="Id"
        )
        self.curve_iq = self.plot.plot(
            pen=pg.mkPen((200, 0, 0), width=2), name="Iq"
        )

        self.id = deque(maxlen=200)
        self.iq = deque(maxlen=200)

        self._scale_div = 0

        layout = QVBoxLayout()
        layout.addWidget(self.plot)
        self.setLayout(layout)

    def update(self, d):
        self.id.append(d["id_mA"])
        self.iq.append(d["iq_mA"])

        self.curve_id.setData(self.id)
        self.curve_iq.setData(self.iq)
