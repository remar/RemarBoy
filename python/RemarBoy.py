from Tkinter import *
import MainWindow
import sys

if len(sys.argv) != 2:
    print "Usage: %s <rom file>" % sys.argv[0]
    exit()

root = Tk()
root.title("RemarBoy")

main = MainWindow.MainWindow(root)
main.insert_cart(sys.argv[1])

root.grid_columnconfigure(0, weight=1)
root.grid_rowconfigure(0, weight=1)
root.geometry("800x500")
root.mainloop()
