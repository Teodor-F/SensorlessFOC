from collections import deque
from PySide6.QtWidgets import QWidget, QVBoxLayout
import pyqtgraph as pg

from collections import deque
from PySide6.QtWidgets import QWidget, QVBoxLayout
import pyqtgraph as pg

class AlphaBetaCurrentsTab(QWidget):
    def __init__(self):
        super().__init__()

        self.plot = pg.PlotWidget(title="Clarke currents (mA)")
        self.plot.setYRange(-2500, 2500, padding=0)
        self.plot.setBackground("w")
        self.plot.showGrid(x=True, y=True, alpha=0.3)
        self.plot.addLegend()

        self.curve_i_alfa = self.plot.plot(
            pen=pg.mkPen((0, 0, 200), width=2), name="I_alfa"
        )
        self.curve_i_beta = self.plot.plot(
            pen=pg.mkPen((200, 0, 0), width=2), name="I_beta"
        )

        self.i_alfa = deque(maxlen=200)
        self.i_beta = deque(maxlen=200)

        self._scale_div = 0

        layout = QVBoxLayout()
        layout.addWidget(self.plot)
        self.setLayout(layout)

    def update(self, d):
        self.i_alfa.append(d["i_alfa_mA"])
        self.i_beta.append(d["i_beta_mA"])

        self.curve_i_alfa.setData(self.i_alfa)
        self.curve_i_beta.setData(self.i_beta)
