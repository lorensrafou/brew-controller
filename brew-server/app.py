
from flask import Flask, render_template

app = Flask(__name__)
debug = True

@app.route('/')
def index():
  return 'Hello World'

if __name__ == '__main__':
  app.debug = debug
  app.run()