from flask import Flask, request, jsonify, send_from_directory
import psycopg2
from flask_cors import CORS
from flask_bcrypt import Bcrypt
import jwt
import os
import traceback
from datetime import timedelta, datetime, timezone
from typing import Dict, List
from constants import DB_NAME, DB_USER, DB_PASSWORD, DB_HOST, DB_PORT, INVALID_SALT

app = Flask(__name__)
app.config['SECRET_KEY'] = os.getenv("FLASK_SECRET_KEY", "dev-only-secret")
CORS(app)
bcrypt = Bcrypt(app)


# =============================== DATABASE ===============================

# Database connection
def get_db_connection():
   return psycopg2.connect(
       dbname=os.getenv("DB_NAME"),
       user=os.getenv("DB_USER"),
       password=os.getenv("DB_PASSWORD"),
       host=os.getenv("DB_HOST"),
       port=os.getenv("DB_PORT"),
   )

def get_current_user_id():
    auth = request.headers.get("Authorization", "")
    print("Authorization header received:", repr(auth))  # new debugging line
    if not auth.startswith("Bearer "):
        print("Token not found or malformed")
        return None
    token = auth.replace("Bearer ", "")
    try:
        decoded = jwt.decode(token, app.config["SECRET_KEY"], algorithms=["HS256"])
        print("Decoded token:", decoded)  # debugging line lol
        return decoded.get("user_id")
    except jwt.InvalidTokenError as e:
        print("Invalid token:", e)
        return None

# =============================== IMAGES ===============================


# Serve images from /assets
@app.route('/assets/<path:filename>')
def serve_assets(filename):
    return send_from_directory('assets', filename)

# =============================== ITEMS ===============================

# Get items (now includes img_route!)
@app.route("/items", methods=["GET"])
def get_items():
    conn = get_db_connection()
    cur = conn.cursor()

# Order alphabetically (putting numbers last instead of first!)
    cur.execute("""
        SELECT i.id, i.name, i.price, c.name AS category, i.img_route
        FROM items i
        LEFT JOIN item_categories ic ON i.id = ic.item_id
        LEFT JOIN categories c ON ic.category_id = c.id
        ORDER BY 
        (LEFT(i.name, 1) ~ '^\d') ASC, 
        i.name ASC;
    """)
    rows = cur.fetchall()
    cur.close()
    conn.close()

    return jsonify([
        {
            "id": row[0],
            "name": row[1],
            "price": float(row[2]),
            "category": row[3],
            "img_route": row[4]
        }
        for row in rows
    ])

# =============================== HOMEPAGE MEALS, COMMENTS, RATINGS ===============================

@app.route("/api/meals", methods=["GET"])
def get_meals():
    print("here in get meals")
    conn = get_db_connection()
    cur = conn.cursor()

    # Get meal + item info INCLUDING poster's username
    cur.execute("""
        SELECT m.id, m.name, m.created_at, u.username, i.name, i.price, i.img_route
        FROM meals m
        JOIN users u ON m.user_id = u.id
        JOIN meal_items mi ON m.id = mi.meal_id
        JOIN items i ON i.id = mi.item_id
        ORDER BY m.id;
    """)
    rows = cur.fetchall()

    # Get average ratings
    cur.execute("""
        SELECT meal_id, ROUND(AVG(rating), 1) as avg_rating
        FROM ratings
        GROUP BY meal_id;
    """)
    rating_rows = cur.fetchall()
    ratings_dict = {row[0]: row[1] for row in rating_rows}

    # Get comments
    cur.execute("""
        SELECT c.id, c.meal_id, users.username, c.text, c.created_at
        FROM comments c
        JOIN users ON c.user_id = users.id
        ORDER BY c.created_at DESC;
    """)
    comment_rows = cur.fetchall()
    cur.close()
    conn.close()

    # Organize comments
    comments_dict = {}
    for comment_id, meal_id, username, text, created_at in comment_rows:
        if meal_id not in comments_dict:
            comments_dict[meal_id] = []
        comments_dict[meal_id].append({
            "id": comment_id,
            "user": username,
            "text": text,
            "created_at": created_at.isoformat()
        })

    # Build final meal response
    meals = {}
    for meal_id, meal_name, created_at, poster_username, item_name, item_price, item_img in rows:
        if meal_id not in meals:
            meals[meal_id] = {
                "id": meal_id,
                "name": meal_name,
                "poster": poster_username,
                "created_at": int(created_at.timestamp() * 1000),
                "avg_rating": ratings_dict.get(meal_id),
                "comments": comments_dict.get(meal_id, []),
                "items": []
            }
        meals[meal_id]["items"].append({
            "name": item_name,
            "price": float(item_price),
            "img_route": item_img
        })

    return jsonify(list(meals.values()))

# get the rating of the meal
@app.route("/api/ratings", methods=["POST"])
def post_rating():
    data = request.get_json()
    user_id = get_current_user_id()
    if not user_id:
        return jsonify({"error": "Unauthorized"}), 401

    meal_id = data["meal_id"]
    rating = data["rating"]

    if rating < 1 or rating > 5:
        return jsonify({"error": "Rating must be between 1 and 5"}), 400

    conn = get_db_connection()
    cur = conn.cursor()

    #  Check if the user already rated this meal
    cur.execute(
        "SELECT id FROM ratings WHERE user_id = %s AND meal_id = %s;",
        (user_id, meal_id)
    )
    existing = cur.fetchone()

    if existing:
        # If they already rated, block them
        cur.close()
        conn.close()
        return jsonify({"error": "You have already rated this meal!"}), 400

    #  Otherwise, insert the new rating
    cur.execute(
        "INSERT INTO ratings (user_id, meal_id, rating) VALUES (%s, %s, %s);",
        (user_id, meal_id, rating)
    )
    conn.commit()
    cur.close()
    conn.close()

    return jsonify({"message": "Rating saved!"}), 201


# Post a comment
@app.route("/api/comments", methods=["POST"])
def post_comment():
    data = request.get_json()
    user_id = get_current_user_id()
    if not user_id:
        return jsonify({"error": "Unauthorized"}), 401

    meal_id = data["meal_id"]
    text = data["text"]

    if not text.strip():
        return jsonify({"error": "Comment cannot be empty"}), 400

    conn = get_db_connection()
    cur = conn.cursor()
    cur.execute(
        "INSERT INTO comments (user_id, meal_id, text) VALUES (%s, %s, %s);",
        (user_id, meal_id, text)
    )
    conn.commit()
    cur.close()
    conn.close()

    # debugging (can prob remove later )
    return jsonify({"message": "Comment added!"}), 201

# Get comments for a meal
@app.route("/api/meals/<int:meal_id>/comments", methods=["GET"])
def get_comments(meal_id):
    conn = get_db_connection()
    cur = conn.cursor()
    cur.execute("""
        SELECT c.id, users.username, c.text, c.created_at
        FROM comments c
        JOIN users ON c.user_id = users.id
        WHERE c.meal_id = %s
        ORDER BY c.created_at DESC;
    """, (meal_id,))
    rows = cur.fetchall()
    cur.close()
    conn.close()

    return jsonify([
        { "id": row[0], "user": row[1], "text": row[2], "created_at": row[3].isoformat()}
        for row in rows
    ])

# Delete a comment by ID
@app.route("/api/comments/<int:comment_id>", methods=["DELETE"])
def delete_comment(comment_id):
    conn = get_db_connection()
    cur = conn.cursor()

    try:
        cur.execute("DELETE FROM comments WHERE id = %s;", (comment_id,))
        conn.commit()
        return jsonify({"message": "Comment deleted successfully"}), 200
    except Exception as e:
        print("Error deleting comment:", e)
        conn.rollback()
        return jsonify({"error": "Failed to delete comment"}), 500
    finally:
        cur.close()
        conn.close()

# =============================== CART ===============================

# Create a new meal
@app.route("/api/meals", methods=["POST"])
def create_meal():
    print("here in POST meals")
    user_id = get_current_user_id()
    if not user_id:
        return jsonify({"error": "Unauthorized"}), 401

    data = request.get_json()
    meal_name = data.get("name")
    item_ids = data.get("items", [])

    meal_name = data.get("name")
    item_ids = data.get("items", [])

    if not meal_name or not item_ids:
        return jsonify({"error": "Missing required fields"}), 400

    conn = get_db_connection()
    cur = conn.cursor()

    try:
        # add our total price 
        cur.execute("SELECT SUM(price) FROM items WHERE id = ANY(%s);", (item_ids,))
        total_price = cur.fetchone()[0] or 0

        cur.execute(
            "INSERT INTO meals (name, user_id, total_price) VALUES (%s, %s, %s) RETURNING id, created_at;",
            (meal_name, user_id, total_price)
        )
        meal_id, created_at = cur.fetchone()  

        for item_id in item_ids:
            cur.execute(
                "INSERT INTO meal_items (meal_id, item_id) VALUES (%s, %s);",
                (meal_id, item_id)
            )

        conn.commit()
        return jsonify({"message": "Meal created", "meal_id": meal_id, "created_at": created_at.isoformat(timespec='minutes')}), 201

    except Exception as e:
        conn.rollback()
        print("Error:", e)
        return jsonify({"error": "Internal server error"}), 500

    finally:
        cur.close()
        conn.close()

@app.route("/api/meals/<int:meal_id>", methods=["DELETE"])
def delete_meal(meal_id):
    user_id = get_current_user_id()
    if not user_id:
        return jsonify({"error": "Unauthorized"}), 401

    conn = get_db_connection()
    cur = conn.cursor()

    # Check if this user owns the meal
    cur.execute("SELECT user_id FROM meals WHERE id = %s;", (meal_id,))
    result = cur.fetchone()

    if not result:
        cur.close()
        conn.close()
        return jsonify({"error": "Meal not found"}), 404

    owner_id = result[0]
    if owner_id != user_id:
        cur.close()
        conn.close()
        return jsonify({"error": "Forbidden"}), 403

    try:
        cur.execute("DELETE FROM meals WHERE id = %s;", (meal_id,))
        conn.commit()
        return jsonify({"message": "Meal deleted"}), 200
    except Exception as e:
        print("Error deleting meal:", e)
        traceback.print_exc()  
        conn.rollback()
        return jsonify({"error": "Server error"}), 500
    finally:
        cur.close()
        conn.close()


# =============================== PROFILE ===============================
@app.route("/api/profile", methods=["GET"])
def get_profile():
    user_id = get_current_user_id()
    if not user_id:
        return jsonify({"error": "Unauthorized"}), 401

    conn = get_db_connection()
    cur = conn.cursor()

    # Fetch basic user info + stats
    cur.execute("""
        SELECT u.username,
               u.email,
               COUNT(DISTINCT m.id)           AS bundle_count,
               COALESCE(AVG(r.rating), 0) AS avg_rating
        FROM users u
        LEFT JOIN meals m   ON m.user_id = u.id
        LEFT JOIN ratings r ON r.meal_id = m.id
        WHERE u.id = %s
        GROUP BY u.username, u.email;
    """, (user_id,))
    row = cur.fetchone()
    if not row:
        cur.close()
        conn.close()
        return jsonify({"error": "User not found"}), 404

    username, email, bundle_count, avg_rating = row

    # Fetch each meal’s header info
    cur.execute("""
        SELECT id,
               name,
               EXTRACT(EPOCH FROM date_trunc('minute', created_at)) * 1000 AS ts_ms
        FROM meals
        WHERE user_id = %s
        ORDER BY created_at DESC;
    """, (user_id,))
    meals_meta = cur.fetchall()   # list of (meal_id, name, ts_ms)

    # Fetch all items, keyed by meal_id
    cur.execute("""
        SELECT mi.meal_id,
               i.name
        FROM meal_items mi
        JOIN items i ON i.id = mi.item_id
        WHERE mi.meal_id = ANY(%s)
        ORDER BY i.name;
    """, ([m[0] for m in meals_meta],))
    items_rows = cur.fetchall()   # list of (meal_id, item_name)

    cur.close()
    conn.close()

    # Assemble bundles in Python
    bundles = []
    # build a dict of meal_id and list of item names
    items_map: Dict[int, List[str]] = {}
    for meal_id, item_name in items_rows:
        items_map.setdefault(meal_id, []).append(item_name)

    for meal_id, name, ts_ms in meals_meta:
        bundles.append({
            "id":          meal_id,
            "name":        name,
            "created_at":  int(ts_ms),
            "items":       items_map.get(meal_id, [])
        })

    return jsonify({
        "username":    username,
        "email":       email,
        "bundleCount": bundle_count,
        "avgRating":   round(avg_rating, 1),
        "bundles":     bundles
    })

# =============================== LOGIN ===============================

@app.route('/login', methods=['POST'])
def login():
    global token
    print("log in ")
    data = request.get_json()
    username = data.get('username')
    password = data.get('password')
    conn = get_db_connection()
    if not conn:
        return jsonify({"error": "Failed to connect to the database"}), 500
    curr = conn.cursor()
    try:
        curr.execute("SELECT * FROM users WHERE username = %s;", (username,))
        exists_user = curr.fetchone()        
        # MAKE SURE TO CHECK THIS W UR LOCAL DB !!!!!
        if exists_user and bcrypt.check_password_hash(exists_user[3], password):
            token = jwt.encode({
                'user_id': exists_user[0],
                'username': exists_user[1],
                'exp': datetime.now(timezone.utc)  + timedelta(hours=3) 
            }, app.config['SECRET_KEY'], algorithm='HS256')
            return jsonify({"message": "Login successful!", "token": token}), 200
        else:
            return jsonify({"error": "Invalid username or password"}), 401
    except Exception as error:
        print(error)
        return jsonify({"error": "An error occurred during login"}), 500
    finally:
        curr.close()
        conn.close()

# =============================== SIGNUP ===============================

@app.route("/signup", methods=["POST"])
def signup():
    print("sign up")
    data = request.get_json()
    if not request.is_json:
        return jsonify({"error": "Content type must be JSON"}), 400
    email = data.get('email')
    username = data.get('username')
    password = data.get('password')
    pass_hash = bcrypt.generate_password_hash(password).decode('utf-8')
    conn = get_db_connection()
    if not conn:
        return jsonify({"error": "Failed to connect to the database"}), 500
    curr = conn.cursor()
    try:
        curr.execute("SELECT * FROM users WHERE username = %s OR email = %s;", (username, email))
        exists_user = curr.fetchone()
        if exists_user:
            return jsonify({"error": "Username or Email already exists. Please try and log in."}), 400
        curr.execute("INSERT INTO users (email, username, password_hash) VALUES (%s, %s, %s);", (email, username, pass_hash))
        conn.commit()
        return jsonify({"message": "User registered successfully!"}), 201
    except Exception as e:
        print("Error:", e) 
        return jsonify({"error": "Database error occurred"}), 500

    finally:
        curr.close()
        
        ## CHANGE PORT IF UR ON DIF ONE

if __name__ == "__main__":
    app.run(host="0.0.0.0", port=9000)
