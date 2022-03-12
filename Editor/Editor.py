
from PyQt5.QtWidgets import *
from PyQt5.QtGui     import *
from PyQt5.QtCore    import QFile, Qt, QTextStream
import qdarktheme, traceback, os.path, json, io, re

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

class MainWindow (QWidget):

    def __init__ (self, parent = None):
        self.app = QApplication ([])
        self.init_theme ()

        super ().__init__ (parent)
        self.setWindowTitle ("Assembler editor")
        self.resize         (1000, 500)

        self.layout = QGridLayout ()

        self.init_interface ()

        self.instructions = []
        self.selected_instruction = None
        self.on_select_item (None)

        self.setLayout (self.layout)
        self.show ()

        if self.set_path ("../ByteCode/Commands.h"):
            self.load (self.file_path_textbox.text ())

#===================================

    def start (self):
        self.app.exec ()

#===================================

    def init_theme (self):
        self.app.setStyleSheet (qdarktheme.load_stylesheet ())

#===================================

    def init_interface (self):
        font = QFont ('cascadia mono')
        font.setPointSize (12)

        self.code_editor = QTextEdit ()
        self.code_editor.setMinimumSize (800, 450)
        self.code_editor.setPlaceholderText ("// Processor code")
        self.code_editor.setFont (font)
        self.code_editor.setTabStopDistance (QFontMetricsF (font).horizontalAdvance (' ') * 4)
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

        self.clear_button = QPushButton ("Clear")
        self.clear_button.clicked.connect (self.on_click_clear)
        self.tool_layout.addWidget (self.clear_button, 6, 0)

        self.instructions_list = QListWidget ()
        self.instructions_list.itemActivated.connect (self.on_select_item)
        self.tool_layout.addWidget (self.instructions_list, 2, 0)

        self.tool_group.setLayout (self.tool_layout)
        self.layout.addWidget (self.tool_group, 0, 1)
        
        self.description_textbox = QTextEdit ()
        self.description_textbox.setMinimumSize (800, 40)
        self.description_textbox.setVerticalScrollBarPolicy (Qt.ScrollBarPolicy.ScrollBarAlwaysOff)
        self.description_textbox.setPlaceholderText ("Instruction description")
        self.description_textbox.setFont (font)
        self.layout.addWidget (self.description_textbox, 1, 0)

        self.arguments_textbox = QTextEdit ()
        self.arguments_textbox.setMinimumSize (800, 40)
        self.arguments_textbox.setVerticalScrollBarPolicy (Qt.ScrollBarPolicy.ScrollBarAlwaysOff)
        self.arguments_textbox.setPlaceholderText ("Required arguments")
        self.arguments_textbox.setFont (font)
        self.layout.addWidget (self.arguments_textbox, 2, 0)

        self.file_path_textbox = QLineEdit ()
        self.file_path_textbox.setMinimumSize (800, 40)
        self.file_path_textbox.setPlaceholderText ("Filename")
        self.file_path_textbox.setDisabled (True)
        self.file_path_textbox.setFont (font)
        self.layout.addWidget (self.file_path_textbox, 3, 0)

        self.browse_button = QPushButton ("Browse...")
        self.browse_button.setMinimumSize (200, 40)
        self.browse_button.clicked.connect (self.on_click_browse)
        self.layout.addWidget (self.browse_button, 3, 1)

#===================================

    def on_ckick_save (self):
        if not len (self.file_path_textbox.text ()):
            self.on_click_browse ()

        self.save (self.file_path_textbox.text ())
    
    def on_click_load (self):
        if not len (self.file_path_textbox.text ()):
            self.on_click_browse ()

        self.load (self.file_path_textbox.text ())

    def on_click_new (self):
        text, result = QInputDialog.getText (self, "New instruction", "Enter instruction name:")
        if not result:
            return

        self.create_instruction (text)

    def on_click_remove (self):
        if self.selected_instruction:
            self.instructions.remove        (self.selected_instruction)
            self.instructions_list.takeItem (self.instructions_list.row (self.selected_instruction.list_item))
            self.on_select_item             (None)

    def on_ckick_rename (self):
        if self.selected_instruction:
            text, result = QInputDialog.getText (self, "Rename", f"Enter new name for '{self.selected_instruction.name}':")
            if not result:
                return

            def fail (text: str):
                QMessageBox.about (self, "Can't rename instruction", text)

            if not len (text):
                fail ("Name is empty")
                return

            for item in self.instructions:
                if item.name == text:
                    fail (f"Instruction '{text}' already exists")
                    return

            self.selected_instruction.name = text
            self.selected_instruction.list_item.setText (text)

    def on_click_browse (self):
        path, result = QFileDialog.getOpenFileName (self, "Select file", "", "All files (*)", options = QFileDialog.Options ())
        if result:
            self.set_path (path)

    def on_click_clear (self):
        self.instructions.clear      ()
        self.instructions_list.clear ()
        self.on_select_item (None)

#===================================
   
    def on_select_item (self, item):
        self.save_changes ()

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
            self.selected_instruction = None

            self.code_editor.setDisabled         (True)
            self.description_textbox.setDisabled (True)
            self.arguments_textbox.setDisabled   (True)

#===================================

    def save_changes (self):
        if self.selected_instruction != None:
            self.selected_instruction.code = self.code_editor.toPlainText         ()
            self.selected_instruction.desc = self.description_textbox.toPlainText ()
            self.selected_instruction.args = self.arguments_textbox.toPlainText   ()

#===================================

    def save (self, filename):
        self.save_changes ()

        data = {}

        data['header_filename'] = filename
        data['instructions'] = []

        for instruction in self.instructions:
            data['instructions'].append ({'name': instruction.name, 'desc': instruction.desc, 'args': instruction.args, 'code': instruction.code})

        with open (filename + ".json", 'w') as file:
            json.dump (data, file, indent = 4)
            file.close ()

        with open (filename, 'w') as file:
            file.write (self.generate_source ())
            file.close ()

        print (f"Saved to {filename}")

#===================================

    def load (self, filename):
        with open (filename + ".json", 'r') as file:
            data = json.load (file)
            file.close ()

        self.instructions_list.clear ()
        self.instructions.clear      ()

        self.on_select_item (None)
        for instruction in data['instructions']:
            self.add_instruction (AssemblerInstruction (instruction['name'], instruction['args'], instruction['desc'], instruction['code']))

        print (f"Loaded from {filename}")

#===================================

    def set_path (self, path):
        if not os.path.isfile (path):
            QMessageBox.critical (self, "File not found", f"File {path} does not exist or is directory")
            return False

        self.file_path_textbox.setText (path)
        return True

#===================================

    def create_instruction (self, name: str):
        def fail (text: str):
            QMessageBox.about (self, "Can't add instruction", text)

        if not len (name):
            fail ("Name is empty")
            return

        for item in self.instructions:
            if item.name == name:
                fail (f"Instruction '{name}' already exists")
                return

        self.add_instruction (AssemblerInstruction (name, "", "", ""))

#===================================

    def add_instruction (self, instruction: AssemblerInstruction):
        self.instructions.append (instruction)
        self.instructions_list.addItem        (instruction.list_item)
        self.instructions_list.setCurrentItem (instruction.list_item)
        self.on_select_item                   (instruction.list_item)

#===================================

    def generate_source (self) -> str:
        source = io.StringIO ()

        delimiter = '//------------------------------\n'

        source.write ("//-----------------------------------------//\n")
        source.write ("// This header was generated automatically //\n")
        source.write ("//-----------------------------------------//\n")
        source.write ("\n")
        source.write ("#pragma once\n")
        source.write ("\n");
        source.write (delimiter)
        source.write ("\n")
        source.write ("#define TOKENS_(args) std::initializer_list <TokenType> (args)\n")
        source.write ("\n")
        source.write ("#define COMMANDS_DEFINES_ \\\n")

        max_name_len = 0
        max_args_len = 0
        max_desc_len = 0
        max_code_len = 0

        for instruction in self.instructions:
            args = ', '.join (["TokenType::" + token for token in instruction.args.split (';')]) if len (instruction.args) else ""
            code = instruction.code.replace ('\n', ' ').replace ('\t', ' ')

            name_len = len (instruction.name)
            args_len = len (args)
            desc_len = len (instruction.desc)
            code_len = len (code)

            if name_len > max_name_len: max_name_len = name_len
            if args_len > max_args_len: max_args_len = args_len
            if desc_len > max_desc_len: max_desc_len = desc_len
            if code_len > max_code_len: max_code_len = code_len

        for instruction in self.instructions:
            args = ', '.join (["TokenType::" + token for token in instruction.args.split (';')]) if len (instruction.args) else ""
            code = instruction.code.replace ('\n', ' ').replace ('\t', ' ')

            source.write ("ACD_ ( {0: <{1}}, TOKENS_ ({{{2: <{3}}}}), \"{4: <{5}}\", {{ {6: <{7}} }}) \\\n".format (instruction.name, max_name_len, args, max_args_len, instruction.desc, max_desc_len, code, max_code_len))

        source.write ("\n")
        source.write (delimiter)

        string = source.getvalue ()
        source.close ()

        return string
        
#===================================

app = MainWindow ()
app.start ()

#===================================