from PyQt5.QtWebEngineWidgets import QWebEngineView, QWebEnginePage
from PyQt5.QtWidgets import QVBoxLayout, QLineEdit, QPushButton, QWidget

class Galipinium(QWebEngineView):
    def __init__(self):
        super().__init__()

        self.setUrl("http://www.google.com")  # Varsayılan URL

        self.url_bar = QLineEdit(self)
        self.url_bar.returnPressed.connect(self.navigate_to_url)
        
        self.layout = QVBoxLayout()
        self.layout.addWidget(self.url_bar)
        self.layout.addWidget(self)

        self.setLayout(self.layout)

    def navigate_to_url(self):
        url = self.url_bar.text()
        self.setUrl(url)  # URL'yi tarayıcıya yönlendir
