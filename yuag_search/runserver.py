"""This file starts a Flask server on the provided port number"""
import argparse
from luxapp import app

def main():
    """Main function that parses CLAs and starts GUI"""
    # using argparse
    parser = argparse.ArgumentParser(
        prog="runserver.py",
        usage="runserver.py [-h] port",
        description="The YUAG search application",
        allow_abbrev=False
    )
    # positional arguments
    parser.add_argument("port", type=int, help="the port at which the server should listen")

    args = parser.parse_args()

    # run with port from command line arg
    app.run(debug=True, port=args.port)

if __name__ == '__main__':
    main()
