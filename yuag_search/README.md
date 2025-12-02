# YUAG Search – Yale University Art Gallery Explorer

A lightweight search engine for the Yale University Art Gallery (YUAG) collection. The app lets visitors query a large SQLite dataset of artworks and explore detailed object records through an interactive Flask web interface.

---

## Project Overview

YUAG Search exposes the `lux.sqlite` collection database through:

- A **primary search page** with live results that update as you type.
- A **secondary object detail page** with rich metadata, production information, and images.
- A clean HTML/CSS front end driven by **AJAX** requests so only the results table refreshes, not the whole page.

The goal was to practice full-stack web development with server-side Python and client-side JavaScript, while working with a non-trivial relational schema.

---

## Tech Stack

**Backend**

- Python 3
- Flask
- SQLite (`lux.sqlite`)
- Standard library `sqlite3` module and prepared statements for all queries

**Frontend**

- HTML + Jinja2 templates
- CSS (`static/styles.css`)
- JavaScript + AJAX (with optional jQuery in the course version)

---

## Features

- **Live search interface**
  - Four text inputs: **Label**, **Classifier**, **Agent**, and **Date**.
  - Results table updates automatically on every keystroke via AJAX.
  - Search parameters are preserved in the inputs after each query.

- **Search results table**
  - Displays up to the first **1000** matching objects.
  - Columns: `Label`, `Date`, `Agents`, `Classified As`.
  - Agents and classifiers are rendered as multi-line lists inside each cell.
  - Table rows are sorted using a deterministic server-side ordering consistent with the course spec.
  - Clicking a label opens a new tab with the full object record.

- **Object detail view** (`/obj/<int:obj_id>`)
  - **Summary** table (accession number, date, place).
  - **Label** section with the object title.
  - **Produced By** table:
    - Part, agent name, nationalities, and timespan (begin–end year, or open-ended for living artists).
    - Table sorted by agent name, then part, timespan, and nationality.
  - **Image** section (if an image exists), using:
    `https://media.collections.yale.edu/thumbnail/yuag/obj/{obj_id}`
  - **Classified As** section listing all classifiers as a sorted unordered list.
  - **Information** section listing references (type + rendered HTML content), sorted by type then content.
  - Robust handling of invalid or missing IDs with user-friendly 404 error pages.

- **Error handling**
  - Graceful messages for:
    - Invalid or missing `obj_id` request paths.
    - Invalid search parameters (ignores unknown query keys).
    - Missing `lux.sqlite` database or invalid port at server start.
  - Error pages are returned at the requested URL (no redirect to a dedicated `/404` route).

- **Styling**
  - Shared `styles.css` for both primary and detail pages.
  - Improved table readability: cell padding, row borders, and hover highlighting.
  - Additional light layout styling while keeping dependencies minimal.

---

## Project Structure

```text
yuag_search/
├── runserver.py        # Parses port argument and starts the Flask dev server
├── luxapp.py           # Primary Flask application (routes, search endpoint)
├── luxdetails.py       # Helper logic for object detail queries (if split out)
├── lux.py              # Database access helpers / query functions
├── templates/
│   ├── home.html       # Primary search page
│   ├── details.html    # Object detail page
│   ├── error.html      # Error page template
├── static/
│   └── styles.css      # Shared styling for tables and layout
└── results.html        # Partial template or legacy file for search results (if used)
(Names may differ slightly depending on the final course submission; this is the intended layout.)

Running the App Locally
Install dependencies

From inside yuag_search/:

bash
Copy code
pip install flask
If you are using a virtual environment, activate it first.

Place the database

Copy lux.sqlite into the yuag_search/ directory (it is not tracked in git because of its size).

Start the server

bash
Copy code
python runserver.py 5000
Open the app

Visit:

Primary search page: http://localhost:5000/

Example detail page: http://localhost:5000/obj/<some_obj_id>

Notes & Possible Extensions
The application currently reads from a static SQLite file and does not modify the collection.

It could be extended with:

Pagination on the search results.

Client-side column sorting.

Deployment to a production server with Gunicorn and Nginx.

Migration to PostgreSQL with an ORM such as SQLAlchemy.

This project shows my ability to build a full Flask-based search interface, design SQL queries over a non-trivial schema, and connect server-side logic with an interactive, AJAX-driven frontend.