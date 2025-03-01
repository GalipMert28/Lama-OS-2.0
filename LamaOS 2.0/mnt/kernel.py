import sys
from PyQt5.QtWidgets import QApplication, QMainWindow, QVBoxLayout, QPushButton, QHBoxLayout, QWidget, QStackedWidget, QLabel
from PyQt5.QtCore import Qt
from PyQt5.QtGui import QColor
from galipinium import Galipinium
from file import FileManager

class DesktopWindow(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle('Galipinium Desktop')
        self.setGeometry(100, 100, 800, 600)
        self.setStyleSheet("background-color: cobalt blue;")  # Arka plan rengi

        # Ana Layout
        layout = QVBoxLayout()
        self.icon_layout = QHBoxLayout()

        # Galipinium Web Browser'ı ekleyin
        self.browser_button = QPushButton('Galipinium Web Browser')
        self.browser_button.clicked.connect(self.open_browser)
        self.browser_button.setIcon(self.style().standardIcon(self.style().SP_FileDialogContentsView))  # Fare ile tıklama için
        self.icon_layout.addWidget(self.browser_button)

        # File Manager (Dosya Yöneticisi) ekleyin
        self.file_button = QPushButton('File Manager')
        self.file_button.clicked.connect(self.open_file_manager)
        self.file_button.setIcon(self.style().standardIcon(self.style().SP_FileDialogFolderIcon))
        self.icon_layout.addWidget(self.file_button)

        layout.addLayout(self.icon_layout)

        # Stacked Widget, hangi pencerenin gösterileceğini kontrol eder
        self.stacked_widget = QStackedWidget(self)
        layout.addWidget(self.stacked_widget)

        # Galipinium ve File Manager pencereleri
        self.browser = Galipinium()
        self.file_manager = FileManager()

        self.stacked_widget.addWidget(self.browser)
        self.stacked_widget.addWidget(self.file_manager)

        # Container ve Layout ayarları
        container = QWidget()
        container.setLayout(layout)
        self.setCentralWidget(container)

    def open_browser(self):
        self.stacked_widget.setCurrentWidget(self.browser)

    def open_file_manager(self):
        self.stacked_widget.setCurrentWidget(self.file_manager)

if __name__ == '__main__':
    app = QApplication(sys.argv)
    window = DesktopWindow()
    window.show()
    sys.exit(app.exec_())
