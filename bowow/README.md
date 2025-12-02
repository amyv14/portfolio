# BowWow Builder

## Overview

BowWow Builder is a full-stack mobile application that allows users to create custom meal combinations from Yale’s Bow Wow dining hall, track their total cost, and share their creations with others. I designed and implemented this project as a final course project, building both the frontend and backend components and deploying the system on AWS.

The app offers four main pages:

- **Home:** A social feed where users post meal bundles, browse others’ meals, and interact through comments and ratings.
- **Food:** A browsable list of all Bow Wow items. Users can view prices, track how close they are to the $12 meal swipe value, and add items to their cart.
- **Cart:** A bundle-building interface where users assemble a meal, save it, and optionally publish it publicly.
- **Profile:** Displays the user’s account information, history of posted meals, and average rating from the community.

The entire system supports authentication, bundle creation, real-time ratings and comments, and dynamic item retrieval from a PostgreSQL database.

---

## Brief Video 

https://github.com/user-attachments/assets/027c32af-a1d3-40a4-8fe5-04b49d2ec0e4

Demo 

## Tech Stack

I built BowWow Builder as a fully integrated full-stack system using:

### **Frontend**
- **React Native (TypeScript)** for cross-platform development (iOS + Android)
- **Expo Router** for navigation and live testing
- **Custom state management** (CartContext) to manage bundle building
- **Dynamic image loading** from server asset routes

### **Backend**
- **Flask (Python)** for API routing and business logic
- **Flask-Bcrypt** for password hashing
- **JWT Authentication** for stateless login and protected routes
- **PostgreSQL** for persistent storage
- **Psycopg2** as the database connector
- **Flask-CORS** for secure frontend communication

### **Deployment**
- Full deployment on **AWS EC2**
- Persistent backend IP accessible to the React Native client
- PostgreSQL database hosted on EC2 for shared access across users

This setup ensures that the entire application can be run from a single cloud environment without local database requirements.

---

## Codebase Structure

### `App/` (Frontend)

Contains all React Native screens, components, styles, and routing logic:

- **Login/Signup** (unauthenticated screens)
- **Tabs** (authenticated app structure):
  - **Home**
  - **Shop** (food list)
  - **Cart**
  - **Profile**

After a successful login, a JWT token is stored and required for accessing protected backend routes.

---

### `assets/`

Holds all images and icons used by the app:

- Food item images (referenced using `img_route` in the database)
- UI assets such as:
  - App logo
  - Icons (cart, profile, etc.)

Images are served from Flask via:

```bash
/assets/<filename>
```

### `db/`

Contains CSV files and initialization scripts for the PostgreSQL database:

- `items.csv` — list of all BowWow food items, prices, and image file references  
- `cat.csv` — category definitions  
- `catitems.csv` — many-to-many mappings between items and categories  
- `read.py` — Python script that loads all CSV data into PostgreSQL using `psycopg2`

The database schema is fully normalized to **3NF** and supports:

- Users  
- Meals  
- Items  
- Meal–item linking table  
- Ratings (one rating per user per meal)  
- Timestamped comments  
- Food categories  

---

### `appp.py` (Backend)

The main Flask backend file, responsible for:

- **JWT Authentication**
  - `/login`
  - `/signup`

- **Item Retrieval**
  - `GET /items`

- **Image Serving**
  - `GET /assets/<filename>`

- **Meal Bundle Operations**
  - `POST /api/meals`
  - `DELETE /api/meals/<meal_id>`
  - `GET /api/meals` (includes meals, items, ratings, comments)

- **Ratings + Comments**
  - `POST /api/ratings`
  - `POST /api/comments`
  - `GET /api/meals/<meal_id>/comments`
  - `DELETE /api/comments/<comment_id>`

- **User Profile**
  - `GET /api/profile`  
    Returns user details, posted meals, and rating statistics

Other features include:

- SQL query execution via `psycopg2`  
- Token decoding utilities  
- CORS configuration  
- Static asset routing  
- Data serialization for client consumption

### appp.py
Appp.py holds the flask API portion of the code, along with encryption and part of the tokenization. The get_current_user_id allows for the tokens to be decoded, and the routes help connect the SQL queries to the frontend.

Here are the following routes to navigate the backend
 - `@app.route('/assets/<path:filename>')` - for image routes
 - `@app.route("/items", methods=["GET"])` - access to item database
 - `@app.route("/api/meals", methods=["GET"])` - get meals from itemsdb
 - `@app.route("/api/ratings", methods=["POST"])` - get ratings
 - `@app.route("/api/comments", methods=["POST"])` - get comments
 - `@app.route("/api/meals/<int:meal_id>/comments", methods=["GET"])` - get comments
 - `@app.route("/api/comments/<int:comment_id>", methods=["DELETE"])` - delete comments
 - `@app.route("/api/meals", methods=["POST"])` - get meals
 - `@app.route("/api/meals/<int:meal_id>", methods=["DELETE"]` - delete meals
 - `@app.route("/api/profile", methods=["GET"])`- get profile details
 - `@app.route('/login', methods=['POST'])` - verify login credentials again
 - `@app.route("/signup", methods=["POST"])`- add sign up credentials to the database

## Database
![diagram of the database](database_diagram.png)


## Summary

BowWow Builder is a full-stack mobile application that demonstrates:
- Cross-platform mobile development
- Backend API engineering
- Database design and normalization
- Authentication and security
- AWS cloud deployment
- Integration of frontend + backend systems

This project represents the culmination of a full semester of development, engineering, testing, and deployment.
