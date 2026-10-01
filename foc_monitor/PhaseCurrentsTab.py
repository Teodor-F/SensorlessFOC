from collections import deque
from PySide6.QtWidgets import QWidget, QVBoxLayout
import pyqtgraph as pg

from collections import deque
from PySide6.QtWidgets import QWidget, QVBoxLayout
import pyqtgraph as pg

class PhaseCurrentsTab(QWidget):
    def __init__(self):
        super().__init__()

        self.plot = pg.PlotWidget(title="Phase Currents (mA)")
        self.plot.setYRange(-3500, 3500, padding=0)
        self.plot.setBackground("w")
        self.plot.showGrid(x=True, y=True, alpha=0.3)
        self.plot.addLegend()

        self.curve_ia = self.plot.plot(
            pen=pg.mkPen((0, 0, 200), width=2), name="Ia"
        )
        self.curve_ib = self.plot.plot(
            pen=pg.mkPen((200, 0, 0), width=2), name="Ib"
        )
        self.curve_ic = self.plot.plot(
            pen=pg.mkPen((0, 200, 0), width=2), name="Ic"
        )

        self.ia = deque(maxlen=200)
        self.ib = deque(maxlen=200)
        self.ic = deque(maxlen=200)

        self._scale_div = 0

        layout = QVBoxLayout()
        layout.addWidget(self.plot)
        self.setLayout(layout)

    def update(self, d):
        self.ia.append(d["ia_mA"])
        self.ib.append(d["ib_mA"])
        self.ic.append(d["ic_mA"])

        self.curve_ia.setData(self.ia)
        self.curve_ib.setData(self.ib)
        self.curve_ic.setData(self.ic)
