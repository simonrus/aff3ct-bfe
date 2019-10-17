from Aff3ctShell import *
import sys

if __name__ == "__main__":
    app = Aff3ctShell()
    app.debug = True
    sys.exit(app.cmdloop())
