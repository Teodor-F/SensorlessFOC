from collections import deque
from PySide6.QtWidgets import QWidget, QVBoxLayout
import pyqtgraph as pg


class AlphaBetaEmfTab(QWidget):
    def __init__(self):
        super().__init__()

        self.plot = pg.PlotWidget(title="Back‑EMF α / β (Time Domain)")
        self.plot.setYRange(-3000, 3000, padding=0)
        self.plot.setBackground("w")
        self.plot.showGrid(x=True, y=True, alpha=0.3)
        self.plot.addLegend()

        self.curve_alpha = self.plot.plot(
            pen=pg.mkPen((200, 0, 0), width=2),
            name="EMF α"
        )
        self.curve_beta = self.plot.plot(
            pen=pg.mkPen((0, 0, 200), width=2),
            name="EMF β"
        )

        self.alpha = deque(maxlen=200)
        self.beta  = deque(maxlen=200)

        self._scale_div = 0
        #self._min_range = 100   # adjust to your EMF units

        layout = QVBoxLayout()
        layout.addWidget(self.plot)
        self.setLayout(layout)

    def update(self, d):
        if "emf_alpha" not in d or "emf_beta" not in d:
            return

        self.alpha.append(d["emf_alpha"])
        self.beta.append(d["emf_beta"])

        self.curve_alpha.setData(self.alpha)
        self.curve_beta.setData(self.beta)
