from PyQt5.QtWidgets import *
from PyQt5.QtGui     import *
from PyQt5.QtCore    import Qt
import darktheme

#===================================

class Application:

#===================================

    def __init__ (self):
        self.app = QApplication ([])

        self.app.setStyleSheet (darktheme.stylesheet)
        self.app.setPalette    (darktheme.palette   )
        self.app.setStyle      (darktheme.style     )

        self.window = QWidget ()
        self.window.resize (1000, 800)

        self.layout = QHBoxLayout ()

        self.init_interface ()

        self.window.setLayout (self.layout)
        self.window.show ()

#===================================

    def start (self):
        self.app.exec ()

#===================================

    def init_interface (self):
        self.code_editor = QTextEdit ()
        self.code_editor.setFixedSize (800, 800)
        self.layout.addWidget (self.code_editor)

        self.save_button = QPushButton ("Save")
        self.save_button.clicked.connect (self.on_ckick_save)
        self.layout.addWidget (self.save_button)

#===================================

    def on_ckick_save (self):
        QMessageBox.about (None, "Save button pressed", "Sosi chlen")
    
#===================================

app = Application ()
app.start ()

#===================================