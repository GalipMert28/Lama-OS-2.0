from PyQt5.QtWidgets import QFileSystemModel, QTreeView, QVBoxLayout, QWidget

class FileManager(QWidget):
    def __init__(self):
        super().__init__()

        self.setWindowTitle('File Manager')
        self.setGeometry(100, 100, 800, 600)

        # Dosya sistemini modelleme
        self.model = QFileSystemModel()
        self.model.setRootPath('')  # Başlangıç dizini

        # Dosya ağacını oluşturma
        self.tree = QTreeView(self)
        self.tree.setModel(self.model)
        self.tree.setRootIndex(self.model.index(''))  # Ana dizin
        self.tree.setColumnWidth(0, 250)  # Kolon genişliği

        layout = QVBoxLayout()
        layout.addWidget(self.tree)
        self.setLayout(layout)
