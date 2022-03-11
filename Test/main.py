from PyQt5.QtWidgets import *
from PyQt5.QtGui     import *
from PyQt5.QtCore    import QFile, Qt
import darktheme

#===================================

class AssemblerInstruction ():
    name: str = None
    args: str = None
    desc: str = None
    code: str = None

    def __init__ (self):
        self.name = "untitled"
        self.args = ""
        self.code = ""
        self.desc = ""

        self.list_item = QListWidgetItem (self.name)

    def __init__ (self, name: str, args: str, desc: str, code: str):
        self.name = name
        self.args = args
        self.code = code
        self.desc = desc

        self.list_item = QListWidgetItem (self.name)

#===================================

class Application:

    def __init__ (self):
        self.app = QApplication ([])

        self.app.setStyleSheet (darktheme.stylesheet)
        #self.app.setPalette    (darktheme.palette   )
        self.app.setStyle      (darktheme.style     )

        self.window = QWidget ()
        self.window.setWindowTitle ("Assembler editor")
        self.window.resize (1000, 800)

        self.layout = QGridLayout ()

        self.init_interface ()

        self.instructions = []
        self.selected_instruction = None
        self.on_select_item (None)

        self.window.setLayout (self.layout)
        self.window.show ()

#===================================

    def start (self):
        self.app.exec ()

#===================================

    def init_interface (self):
        self.code_editor = QTextEdit ()
        self.code_editor.setFixedSize (800, 750)
        self.code_editor.setPlaceholderText ("// Processor code")
        self.layout.addWidget (self.code_editor, 0, 0)

        self.tool_group = QGroupBox ("Instructions")
        self.tool_layout = QGridLayout ()

        self.save_button = QPushButton ("Save")
        self.save_button.clicked.connect (self.on_ckick_save)
        self.tool_layout.addWidget (self.save_button, 0, 0)

        self.load_button = QPushButton ("Load")
        self.load_button.clicked.connect (self.on_click_load)
        self.tool_layout.addWidget (self.load_button, 1, 0)

        self.add_button = QPushButton ("Add")
        self.add_button.clicked.connect (self.on_click_new)
        self.tool_layout.addWidget (self.add_button, 3, 0)

        self.remove_button = QPushButton ("Remove")
        self.remove_button.clicked.connect (self.on_click_remove)
        self.tool_layout.addWidget (self.remove_button, 4, 0)

        self.rename_button = QPushButton ("Rename")
        self.rename_button.clicked.connect (self.on_ckick_rename)
        self.tool_layout.addWidget (self.rename_button, 5, 0)

        self.instructions_list = QListWidget ()
        self.instructions_list.itemActivated.connect (self.on_select_item)
        self.tool_layout.addWidget (self.instructions_list, 2, 0)

        self.tool_group.setLayout (self.tool_layout)
        self.layout.addWidget (self.tool_group, 0, 1)
        
        self.description_textbox = QTextEdit ()
        self.description_textbox.setFixedSize (800, 25)
        self.description_textbox.setPlaceholderText ("Instruction description")
        self.layout.addWidget (self.description_textbox, 1, 0)

        self.arguments_textbox = QTextEdit ()
        self.arguments_textbox.setFixedSize (800, 25)
        self.arguments_textbox.setPlaceholderText ("Required arguments")
        self.layout.addWidget (self.arguments_textbox, 2, 0)

        self.file_path_textbox = QTextEdit ()
        self.file_path_textbox.setFixedSize (800, 25)
        self.layout.addWidget (self.file_path_textbox, 3, 0)

        self.browse_button = QPushButton ("Browse...")
        self.browse_button.setFixedSize (200, 25)
        self.browse_button.clicked.connect (self.on_click_browse)
        self.layout.addWidget (self.browse_button, 3, 1)

#===================================

    def on_ckick_save (self):
        QMessageBox.about (self.window, "Sosi chlen", "Save button pressed")
    
    def on_click_load (self):
        QMessageBox.about (self.window, "Sosi govno", "Load button pressed")

    def on_click_new (self):
        text, result = QInputDialog.getText (self.window, "New instruction", "Enter instruction name:")
        if not result:
            return

        self.add_instruction (text)

    def on_click_remove (self):
        if self.selected_instruction:
            self.instructions.remove                (self.selected_instruction)
            self.instructions_list.removeItemWidget (self.selected_instruction.list_item)
            self.on_select_item (None)

    def on_ckick_rename (self):
        if self.selected_instruction:
            text, result = QInputDialog.getText (self.window, "Rename", f"Enter new name for '{self.selected_instruction.name}':")
            if not result:
                return

            self.selected_instruction.name = text
            self.selected_instruction.list_item.setText (text)

    def on_click_browse (self):
        filename, _ = QFileDialog.getOpenFileName (self.window, "Select file", "", "All files (*)", options = QFileDialog.Options ())
        self.file_path_textbox.setText (filename)

#===================================
   
    def on_select_item (self, item):
        if self.selected_instruction != None:
            self.selected_instruction.code = self.code_editor.toPlainText         ()
            self.selected_instruction.desc = self.description_textbox.toPlainText ()
            self.selected_instruction.args = self.arguments_textbox.toPlainText   ()

        if item != None:
            for instruction in self.instructions:
                if instruction.list_item == item:
                    self.selected_instruction = instruction

                    self.code_editor.setText         (instruction.code)
                    self.description_textbox.setText (instruction.desc)
                    self.arguments_textbox.setText   (instruction.args)

                    self.code_editor.setDisabled         (False)
                    self.description_textbox.setDisabled (False)
                    self.arguments_textbox.setDisabled   (False)

        else: 
            self.code_editor.setDisabled         (True)
            self.description_textbox.setDisabled (True)
            self.arguments_textbox.setDisabled   (True)

#===================================

    def load (self):
        pass

#===================================

    def add_instruction (self, name: str):
        def fail (text: str):
            QMessageBox.about (self.window, "Can't add instruction", text)

        if not len (name):
            fail ("Name is empty")
            return

        for item in self.instructions:
            if item.name == name:
                fail (f"Instruction '{name}' already exists")
                return

        instruction = AssemblerInstruction (name, "", "", "")

        self.instructions.append (instruction)
        self.instructions_list.addItem        (instruction.list_item)
        self.instructions_list.setCurrentItem (instruction.list_item)
        self.on_select_item                   (instruction.list_item)

#===================================

app = Application ()
app.start ()

#===================================