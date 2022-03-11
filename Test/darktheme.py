from PyQt5.QtWidgets import *
from PyQt5.QtGui     import *
from PyQt5.QtCore    import Qt

#===================================

palette = QPalette ()
palette.setColor (QPalette.Link,            QColor (42, 130, 218))
palette.setColor (QPalette.Highlight,       QColor (42, 130, 218))
palette.setColor (QPalette.Window,          QColor (53, 53,  53 ))
palette.setColor (QPalette.AlternateBase,   QColor (53, 53,  53 ))
palette.setColor (QPalette.Base,            QColor (25, 25,  25 ))
palette.setColor (QPalette.Button,          QColor (53, 53,  53 ))
palette.setColor (QPalette.WindowText,      Qt.white             )
palette.setColor (QPalette.ToolTipBase,     Qt.white             )
palette.setColor (QPalette.ToolTipText,     Qt.white             )
palette.setColor (QPalette.Text,            Qt.white             )
palette.setColor (QPalette.ButtonText,      Qt.white             )
palette.setColor (QPalette.BrightText,      Qt.red               )
palette.setColor (QPalette.HighlightedText, Qt.black             )

palette.setColor (QPalette.ColorGroup.Disabled, QPalette.Link,            QColor (84,  250, 218))
palette.setColor (QPalette.ColorGroup.Disabled, QPalette.Highlight,       QColor (84,  250, 218))
palette.setColor (QPalette.ColorGroup.Disabled, QPalette.Window,          QColor (106, 106, 106))
palette.setColor (QPalette.ColorGroup.Disabled, QPalette.AlternateBase,   QColor (106, 106, 106))
palette.setColor (QPalette.ColorGroup.Disabled, QPalette.Base,            QColor (70,  70,  70 ))
palette.setColor (QPalette.ColorGroup.Disabled, QPalette.Button,          QColor (106, 106, 106))
palette.setColor (QPalette.ColorGroup.Disabled, QPalette.WindowText,      QColor (200, 200, 200))
palette.setColor (QPalette.ColorGroup.Disabled, QPalette.ToolTipBase,     QColor (200, 200, 200))
palette.setColor (QPalette.ColorGroup.Disabled, QPalette.ToolTipText,     QColor (200, 200, 200))
palette.setColor (QPalette.ColorGroup.Disabled, QPalette.Text,            QColor (200, 200, 200))
palette.setColor (QPalette.ColorGroup.Disabled, QPalette.ButtonText,      QColor (200, 200, 200))
palette.setColor (QPalette.ColorGroup.Disabled, QPalette.BrightText,      QColor (255, 100, 100))
palette.setColor (QPalette.ColorGroup.Disabled, QPalette.HighlightedText, QColor (128, 128, 128))

stylesheet = "QToolTip { color: #ffffff; background-color: #2a82da; border: 1px solid white; }"
style      = "Fusion"

#===================================