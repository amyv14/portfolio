"""Application file for our website that uses cookies and sends users to different webpages"""
import sqlite3
import html
from urllib.parse import unquote
from flask import Flask, render_template, request, make_response, jsonify
import requests
from luxdetails import collect_info
from lux import find_id

app = Flask(__name__)

def query_database(label=None, classifier=None, agent=None, date=None):
    """get db, query it, and return"""
    conn = sqlite3.connect("lux.sqlite")
    conn.row_factory = sqlite3.Row
    cursor = conn.cursor()
    results = find_id(cursor, label=label, classifier=classifier, agent=agent, date=date)
    conn.close()
    return results

@app.route('/', methods=['GET', 'POST'])
def home():
    """creates route to and from home page"""
    search_results = []
    message = ""

    # params = {"l", "a", "c", "d"}

    # retrieves data from cookies if it exists or just sets them to empty strings
    label = unquote(request.cookies.get('label', '')).strip()
    classifier = unquote(request.cookies.get('classifier', '')).strip()
    agent = unquote(request.cookies.get('agent', '')).strip()
    date = unquote(request.cookies.get('date', '')).strip()

    # checks if method is post aka form submission
    if request.method == 'POST':
        # escaping before displaying to prevent XSS attacks
        label = html.escape(request.form.get('label', '').strip())
        classifier = html.escape(request.form.get('classifier', '').strip())
        agent = html.escape(request.form.get('agent', '').strip())
        date = html.escape(request.form.get('date', '').strip())

        # if they didnt type words return no terms provided
        if not (label or classifier or agent or date):
            message = "No search terms provided. Please enter some search terms."
        else:
            # search using what user inputted
            search_results = query_database(label=label,
                                            classifier=classifier,
                                            agent=agent, date=date)

            # handle no results found
            if not search_results:
                message = "No results found with provided information."

        # creates response to render home.html directly with search results
        response = make_response(render_template('home.html',
                                                 search_results=search_results,
                                                 message=message, label=label,
                                                 classifier=classifier,
                                                 agent=agent, date=date))
        response.set_cookie('label', label)
        response.set_cookie('classifier', classifier)
        response.set_cookie('agent', agent)
        response.set_cookie('date', date)
        return response

    # renders home.html
    return render_template('home.html', search_results=search_results, message=message,
                           label=label, classifier=classifier, agent=agent, date=date)

@app.route('/obj/<int:obj_id>')
def details(obj_id):
    """Defines the route to the details page after user clicks on search"""
    try:
        # connect to the database
        conn = sqlite3.connect("lux.sqlite")
        cursor = conn.cursor()

        # retrieve the pickled data
        cursor.execute("SELECT * FROM objects WHERE id = ?", (obj_id,))
        answer = cursor.fetchone()

        if not answer:
            return render_template('error.html', message="Invalid obj_id.")

        # get the pickled response from collect_info in luxdetails.py
        data = collect_info(cursor, obj_id)
        conn.close()

        # unpickle the data
        if 'error' in data:
            return render_template('error.html', message=data['error'])

        # check if an image exists
        image_url = f"https://media.collections.yale.edu/thumbnail/yuag/obj/{obj_id}"
        try:
            # set image_url to None if an image is not found
            response = requests.get(image_url, timeout=100)
            if response.status_code != 200:
                image_url = None
        except requests.RequestException:
            image_url = None

        # render details.html with an image if it exists
        return render_template('details.html', data=data, obj_id=obj_id, image_url=image_url)

    except sqlite3.Error:
        return render_template('error.html', message="Database error.")

@app.route('/search')
def ajax_search():
    """Handles AJAX GET requests made to /search route"""
    label = request.args.get("l", "")
    agent = request.args.get("a", "")
    classifier = request.args.get("c", "")
    date = request.args.get("d", "")

    # query database using info from above
    results = query_database(label=label, agent=agent, classifier=classifier, date=date)

    # make results a list of dicts
    json_results = []
    for r in results:
        json_results.append({
            "id": r["id"],
            "label": r["label"],
            "date": r["date"],
            "agents": r["agents"],
            "classifiers": r["classifiers"]
        })
    # return results to client as JSON
    return jsonify(json_results)

@app.errorhandler(404)
def page_not_found(error):
    """handles page nout found error"""
    return render_template('error.html', message=f"Error: {error}."), 404

@app.errorhandler(500)
def server_error():
    """handle server error"""
    return render_template('error.html', message="Error: A server error occurred."), 500


@app.route('/details/<path:invalid_path>')
def invalid_object_id(invalid_path):
    """invalid object id with 404 status"""
    return render_template('error.html',
                           message=f"Error: No object with id {invalid_path} exists."), 404

if __name__ == '__main__':
    app.run(debug=True)
