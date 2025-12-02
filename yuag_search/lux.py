"""This file is used to query the db and filter different objects based on what the user enters."""
import argparse
import sqlite3
from table import Table

def find_id(cursor, date=None, agent=None, classifier=None, label=None):
    """Fetch objects from the database using nested queries and building where."""

    # use lower for lowercase and upper to be considered same for classified as
    query = """
    WITH produced_data AS (
        SELECT objects.id,
               COALESCE(GROUP_CONCAT(DISTINCT agents.name || ' (' || productions.part || ')' ORDER BY agents.name ASC, productions.part ASC), 'Unknown') AS produced_by
        FROM objects
        LEFT JOIN productions ON objects.id = productions.obj_id
        LEFT JOIN agents ON productions.agt_id = agents.id
        GROUP BY objects.id
    ),
    classified_data AS (
        SELECT objects.id,
               COALESCE(REPLACE(GROUP_CONCAT(DISTINCT classifiers.name ORDER BY LOWER(classifiers.name) ASC), ',', '\n'), 'Unclassified') AS classified_as
        FROM objects
        LEFT JOIN objects_classifiers ON objects.id = objects_classifiers.obj_id
        LEFT JOIN classifiers ON objects_classifiers.cls_id = classifiers.id
        GROUP BY objects.id
    )
    SELECT objects.id, objects.label, objects.date,
           produced_data.produced_by, classified_data.classified_as
    FROM objects
    LEFT JOIN produced_data ON objects.id = produced_data.id
    LEFT JOIN classified_data ON objects.id = classified_data.id
    LEFT JOIN productions ON objects.id = productions.obj_id
    LEFT JOIN agents ON productions.agt_id = agents.id
    """

    where = []
    cla = []

    if date:
        where.append("objects.date LIKE ?")
        cla.append(f"%{date}%")
    if agent:
        where.append("agents.name LIKE ?")
        cla.append(f"%{agent}%")
    if classifier:
        where.append("classified_data.classified_as LIKE ?")
        cla.append(f"%{classifier}%")
    if label:
        where.append("objects.label LIKE ?")
        cla.append(f"%{label}%")

    if where:
        query += " WHERE " + " AND ".join(where)

    query += " ORDER BY objects.label ASC, objects.date ASC LIMIT 1000"

    if cla:
        cursor.execute(query, cla)
    else:
        cursor.execute(query)

    result_query = cursor.fetchall()

    # convert to list of dicts for later use
    results = []
    for row in result_query:
        obj_id, label, date, raw_agents, raw_classifiers = row
        agent_list = [a.strip() for a in raw_agents.split(',')] if raw_agents else []
        class_l = [c.strip() for c in raw_classifiers.splitlines()] if raw_classifiers else []

        results.append({
            "id": obj_id,
            "label": label,
            "date": date,
            "agents": agent_list,
            "classifiers": class_l})        
    return results



def build_table(result_query):
    """ build our table using the query """
    # use table.py to build our table
    # for debugging to make sure our query ok
    # print(result_query)

    # in the event the query is empty we print no results
    if not result_query:
        print("No results found.")
        return

    # print number of objects by checkung IDs (set to remove repeat vals )
    print(f"Search produced {len(set(row[0] for row in result_query))} objects.\n")

    columns = ["ID", "Label", "Date", "Produced By", "Classified As"]

    # keep the rows currently in table
    in_table = []
    # keep most recent id
    last_obj = None
    # keep curr row
    current_row = None

    # for each row within our query
    for row in result_query:
        obj_id, label, date, produced_by, classified_as = row

        # incase some vals or cols do not exist we say unknown
        # Handle missing values by assigning "Unknown" or "Unclassified"
        obj_id, label, date, produced_by, classified_as = check_missing(
        obj_id, label, date, produced_by, classified_as)


        # if new object add new row (prevents dups)

        ## NEED TO FIX WRAPING OF CLASSIFIED AS ####
        if obj_id != last_obj:
            if current_row:
                in_table.append(current_row)
            current_row = [obj_id, label, date, produced_by, classified_as]
            last_obj = obj_id
            last_produced_by = produced_by
        else:
            # if same object add as new lines in same cell
            if produced_by != last_produced_by:
                current_row = list(current_row)
                current_row[3] += f"\n{produced_by}"
                last_produced_by = produced_by

            if classified_as not in current_row[4]:
                current_row = list(current_row)
                current_row[4] += f"\n{classified_as}"

    # last row
    if current_row:
        in_table.append(current_row)

    # make + print table
    table = Table(columns, in_table, format_str="wwwww")
    print(table)

def check_missing(obj_id, label, date, produced_by, classified_as):
    """If no values we replace with Unknown."""
    if obj_id is None:
        obj_id = "Unknown"
    if label is None:
        label = "Unknown"
    if date is None:
        date = "Unknown"
    if produced_by is None:
        produced_by = "Unknown"
    if classified_as is None:
        classified_as = "Unknown"

    return obj_id, label, date, produced_by, classified_as


def parse_arguments():
    """ Parse our CLA """
    # parse CLA
    parsed = argparse.ArgumentParser(
        usage="lux.py [-h] [-d date] [-a agt] [-c cls] [-l label]",
        allow_abbrev=False
    )

    # add filters , had to google metavar so that we didnt have -data or -ag
    parsed.add_argument(
    "-d", metavar="date", type=str, 
    help="show only those objects whose date contains date")
    parsed.add_argument(
    "-a", metavar="agt", type=str, 
    help="show only those objects produced by an agent with name containing agt")
    parsed.add_argument(
    "-c", metavar="cls", type=str, 
    help="show only those objects classified with a classifier having a name containing cls"
)
    parsed.add_argument(
    "-l", metavar="label", type=str, 
    help="show only those objects whose label contains label")
    args = parsed.parse_args()

    return args

def main():
    """ Main """
    # connect to db
    conn = sqlite3.connect("lux.sqlite")
    cursor = conn.cursor()
     # # parse CLA
    args = parse_arguments()

    # call find id to find the info for our table
    # only pass if there is a value
    info = find_id(
        cursor,
        date=args.d if args.d else None,
        agent=args.a if args.a else None,
        classifier=args.c if args.c else None,
        label=args.l if args.l else None
    )

    # call build table to form table from our info
    build_table(info)

    # close out of our db
    conn.close()


if __name__ == "__main__":
    main()
