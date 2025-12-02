"""Luxdetails.py queries the database to give users info about the art collection"""
import argparse
import sqlite3
import sys
import pickle
from table import Table

def validate_id(cursor, obj_id):
    """Checks if the provided ID exists in the database and sends pickled response."""
    try:
         # use SQL query to search for the id given by user
        cursor.execute("SELECT * FROM objects WHERE id = ?", (obj_id,))
        answer = cursor.fetchone()

        # call print function only on valid id
        if answer:
            response = collect_info(cursor, obj_id)
            # we now need to pickle data before we send i
            sys.stdout.buffer.write(pickle.dumps(response))
        else:
            sys.stdout.buffer.write(pickle.dumps({"error": f"No object found with ID {obj_id}"}))
    # print if there is a database error
    except sqlite3.Error as e:
        sys.stdout.buffer.write(pickle.dumps({"error": f"Database error: {e}"}))
        sys.exit(1)

# needed to add to remove dup headers and keep lines in tble.
def fix_doubles(table_str):
    """Get rid of double headers from table as well as collect info conflict."""
    lines = table_str.split("\n")

    # make sure there more than one line left to check , if we see all text we can assume title
    if len(lines) > 1 and all(c.isalnum() or c.isspace() for c in lines[0]):
        #if second line has dashes we need to keep them
        if set(lines[1]) == {"-"}:
            return "\n".join(lines[1:])

    #return to tble w lines still present and dups gone
    return "\n".join(lines)

def collect_info(cursor, obj_id):
    """Collects all necessary info and returns it as a dictionary without duplicate headers."""
    return {
        "Summary": fix_doubles(summary(cursor, obj_id)),
        "Label": fix_doubles(label(cursor, obj_id)),
        "Produced By": fix_doubles(produced_by(cursor, obj_id)),
        "Classified As": fix_doubles(classified_as(cursor, obj_id)),
        "Information": fix_doubles(information(cursor, obj_id))
    }

def summary(cursor, obj_id):
    """Fetch summary details and return as structured data."""
      # fetch the summary; accession no, date, place, and department with using left joins
    cursor.execute("""
        SELECT o.accession_no, o.date, p.label, d.name
        FROM objects o
        LEFT JOIN objects_places op ON o.id=op.obj_id
        LEFT JOIN places p ON op.pl_id=p.id
        LEFT JOIN objects_departments od ON o.id=od.obj_id
        LEFT JOIN departments d ON od.dep_id=d.id
        WHERE o.id = ?
    """, (obj_id,))
    # fetch and store the resul
    result = cursor.fetchone()

    # print summary using the Table function
    # make sure empty string is printed when there is no information
    if result:
        accession_no, date, place, department = result
        return Table(
            ["Accession Number", "Date", "Places", "Department"],
            [[accession_no or "N/A", date or "N/A", place or "N/A", department or "N/A"]],
            format_str="tttt"
        ).__str__()

    return "No summary available."

# prints the label section of outpu
def label(cursor, obj_id):
    """Fetch label details and return as structured data."""
    # select the label by using id
    cursor.execute("SELECT label FROM objects WHERE id = ?", (obj_id,))
    labels = cursor.fetchone()

    return Table(
        ["Label"],
        [[labels[0]]] if labels else [["No label available"]],
        format_str="w"
    ).__str__()


def produced_by(cursor, obj_id):
    """Fetch production details and return as structured data."""
    cursor.execute("""
        SELECT pr.part, ag.name,
            GROUP_CONCAT(n.descriptor, ', ' ORDER BY n.descriptor ASC) AS nationalities,
            strftime('%Y', ag.begin_date), strftime('%Y', ag.end_date), ag.begin_bce, ag.end_bce
        FROM productions pr
        JOIN agents ag ON pr.agt_id = ag.id
        LEFT JOIN agents_nationalities an ON ag.id = an.agt_id
        LEFT JOIN nationalities n ON an.nat_id = n.id
        WHERE pr.obj_id = ?
        GROUP BY pr.agt_id
        ORDER BY ag.name ASC, pr.part ASC;
    """, (obj_id,))

    # fetch and store the productions

    production = cursor.fetchall()

    # print with proper format using the Table function
    if production:
        return Table(
            ["Part", "Name", "Timespan", "Nationalities"],
            [[row[0] or "N/A", row[1] or "N/A",
              f"{row[3] or ''}-{row[4] or ''}" if row[3] or row[4] else "Unknown",
              row[2] or "N/A"] for row in production],
            format_str="wwww"
        ).__str__()

    return "No production data available."


def classified_as(cursor, obj_id):
    """Fetch classification details and return as structured data."""
    # select statement for getting all of the classifications
    cursor.execute("""
        SELECT c.name
        FROM objects o
        LEFT JOIN objects_classifiers oc ON o.id=oc.obj_id
        LEFT JOIN classifiers c ON oc.cls_id=c.id
        WHERE o.id = ?
        ORDER BY LOWER(c.name) ASC
    """, (obj_id,))

     # fetch and store the classifiers
    classifier = cursor.fetchall()

     # print with proper format using the Table function
    if classifier:
        return Table(
            ["Classified As"],
            [[row[0]] for row in classifier],
            format_str="w"
        ).__str__()

    return "No classification available."


def information(cursor, obj_id):
    """Fetch reference details and return as structured data."""
    cursor.execute("""
        SELECT type, content
        FROM "references"
        WHERE obj_id = ?;
    """, (obj_id,))

    # fetch and store the references
    references = cursor.fetchall()

    if references:
        return Table(
            ["Type", "Content"],
            [[row[0], row[1]] for row in references],
            format_str="ww"
        ).__str__()

    return "No additional information available."


def main():
    """Parses CLA and opens connection to the database."""
    parsed = argparse.ArgumentParser(usage="luxdetails.py [-h] id",
                                     allow_abbrev=False)
    # add our CLA:
    parsed.add_argument(
        "id",
        type=int,
        help="The ID of the object whose details should be shown"
    )
    args = parsed.parse_args()

     # open connection to the database & create cursor for parsing
    connection = sqlite3.connect("lux.sqlite")
    cursor = connection.cursor()

    # call helper to display the info
    validate_id(cursor, args.id)

    #close connection
    connection.close()


if __name__ == "__main__":
    main()
