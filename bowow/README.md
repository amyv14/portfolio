
# Welcome to the BowWow Builder!

## Description
For the final project, we have created an app that allows users to build their lunch meal at the Bow Wow, keeping track of their total cost, and share it with others.
There are four key pages. The **home page** is where users can interact, posting their daily Bow Wow combos, comments, and rating their meals, along with others. The **food page** where users can browse through items in the BowWow, keeping a running total that shows how much they are over or under the $12 (price equivalent of one meal swipe). The **cart page** allows users to save a meal bundle and post it. And finally, the **profile page** where users can see their account information, a history of their meal bundles, and the average ratings on their meals. 

## Tech Stack
To create this app, we used React Native for compatibility with IOS and Android systems and Expo for easy routing and quick testing. For the database, we used PostgreSQL. For the application, we used Python with Flask for the backend queries and TypeScript for each page. Our database was previously local to the server, but the entire project is hosted on AWS through an EC2 instance. This allows us to maintain a unified IP address allowing for anyone in the EC2 instance to gain access to the database and to run the application without locally storing it.


## Instructions for Running
The following instructions will be for Mac/Linux machines.

Our libraries and databases have all been set up on our EC2 instances. As a result, we will be walking through how to set up the EC2 locally. Please contact us if you run into any issues that we have not outlined.

**Set Up**
- Download the my_key.pem file on your computer with the path saved. 
- Copy the following into the terminal: `ssh -i <path to the_key.pem> ec2-user@13.58.115.85`
	- For example `ssh -i /Users/amy/Desktop/the_key.pem ec2-user@13.58.115.85`
- Also be sure to clone our Github repository with `git clone https://github.com/yale-cpsc-419-25sp/project-project-group-5.git` in a location you will be able to navigate to later


**To run the app**
- Once in the EC2 Instance, run: `cd project-project-group-5/Bowow-Builder/`
- From here, run `python3 appp.py` in the terminal to connect to the server (at port 9000)
	- If you recieve `Address already in use Port 9000 is in use by another program. Either identify and stop that program, or start the server with a different port.` This is not an error and means that the server is already set up and running for you. 
- Then in a second terminal outside of the instance, cd into the `Bowow-Builder folder` from our Github repo
- Finally, type `npx expo start` (if there are error messages, ensure expo is correctly installed. we experimented with our friends' computers, and they had to uninstall some packages until it was compatiable)
- The app can then be displayed in two ways:
	- Download the ExpoGo app on another device and scan the QR code. (RECOMMENDED as it will be simpler)
	- Connect using an Android/IOS emulator.

## Codebase Structure
The top of our project is the Bowow-Builder folder. Bowow-Builder holds our code regarding the app, and is split into multiple sections, listed below:

### App
The app folder holds all our front-end code regarding the actual app such as the login and sign up, before the user has to authenticate themselves. Once the user is authenticated through logging in, the (tabs) folder can be accessed. This folder holds the front end and css for the main pages of the app (home, shop, cart, and profile) which would require a user id token to access correctly.

### Assets
The assets folder holds all the images to the app, and the food item images are separated into the images folder. These images are loaded onto the database and accessed using the read.py in the db folder. The images outside of the assets folder correlate to simple UI/UX design choices like the front display screen, the cart, trash, etc.

### Database
In order to load in our database locally, we created the db folder which takes in different csv files and adds the information into the database depending on the function, allowing our database to be third normal form as each table is connected. Cat.csv contains the category, and is connected to catitems.csv through its category id. All the items in items.csv are sorted by an ID number which connects its categories through catitems.csv. The items.csv contains the ID, item name, price, and image route which are all necessary for the home page to run. To run the database, read.py connects to the postgreSQL user and creates all tables locally.

### appp.py
Appp.py holds the flask API portion of the code, along with encryption and part of the tokenization. The get_current_user_id allows for the tokens to be decoded, and the routes help connect the SQL queries to the frontend.

Here are the following routes to navigate our backend
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

 ### React Native Modularity and Code Structure Explaination
 We opted to use React Native which combines the CSS, HTML-like componenets, and Typescript all in one file because of it's intuitive development experieince. By having the application and presentation layers of each page in one file, it was easy for each team member to work on a single file and reduces potential conflicts. It simplified our repository and allowed us to code for both IOS and Android. 

## Database
![diagram of our database](database_diagram.png)

## Resources
[Click here for our presentation](https://docs.google.com/presentation/d/11VlMHjL7tEJayRi69YNYi9CN7ZkigBryy6pFysgQv3Y/edit?usp=sharing)