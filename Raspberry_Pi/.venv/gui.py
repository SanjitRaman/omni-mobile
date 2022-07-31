import sys, socket
from Ui_mainWindow import Ui_MainWindow as mainWindow
from PyQt5.QtWidgets import QApplication, QMainWindow


client_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)


class GUIMainWindow(QMainWindow, mainWindow):
    def __init__(self, parent=None):
        
        super().__init__()
        self.setupUi(self)
        #self.retranslateUi()

    def setupUi(self, MainWindow):
        super().setupUi(MainWindow)
        self.forwardsButton.pressed.connect(self.forwardsClicked)


    def forwardsClicked(self):
        # method for when forwards button clicked
        print("Forwards Clicked")
        client_socket.sendto("FWD".encode('utf-8'), ('192.168.1.122', 12346)) # send msg


app = QApplication(sys.argv)

window = GUIMainWindow()
window.show()

app.exec()
