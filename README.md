# Web Client API – PCOM Assignment

## Justification for Using Parson

Parson is lightweight, portable, and has no external dependencies.  
It works very well for this project, which specifically requires fast JSON object parsing and simple interaction with primitive values and arrays, without unnecessary complexity.

## Implementation Overview

For the actual implementation of the project, I used my solution from Lab 9 of PCOM as a skeleton.  
I reused the code from `requests` and `helpers`, adapting them to this assignment's needs.  
The client logic was significantly expanded: in the lab, it only handled simple GET or POST requests, but here it also handles cookies and JWTs.

## Admin Functions

### `login_admin`
Prompts for the administrator’s username and password. Sends a POST request to `/api/v1/tema/admin/login` with this data as JSON.  
If authentication is successful, a session cookie is extracted from the `Set-Cookie` header and saved for future use.

### `logout_admin`
Checks whether an admin session cookie exists. If so, sends a GET request to `/api/v1/tema/admin/logout`.  
If the logout is accepted, the cookie is cleared from memory.

### `add_user`
Allows the admin to create a new user. The new user's username and password are read and sent as JSON via POST to `/api/v1/tema/admin/users`.  
The request includes the admin cookie for authorization. If successful, a confirmation message is shown.

### `get_users`
Sends a GET request to `/api/v1/tema/admin/users` to retrieve all users.  
The response is a JSON object containing an array of users, each shown with ID, username, and password.

### `delete_user`
Receives a username, constructs the URL `/api/v1/tema/admin/users/<username>`, and sends a DELETE request.  
Displays a confirmation or error message based on the server's response.

## User Functions

### `login`
Receives `admin_username`, `username`, and `password`. Sends them in a JSON object via POST to `/api/v1/tema/user/login`.  
On success, extracts and stores the session cookie.

### `logout`
If the user is authenticated, sends a GET request to `/api/v1/tema/user/logout`.  
Clears the session cookie on success.

## Authorization and Library Access

### `get_access`
Uses the user’s cookie to send a GET request to `/api/v1/tema/library/access`.  
The server responds with a JWT token, which is extracted from the JSON body and stored.  
This token is required for all movie and collection-related actions.

## Movie Endpoints

### `get_movies`
Uses the JWT token to send a GET request to `/api/v1/tema/library/movies`.  
The response is a JSON array or an object with a `movies` field.  
Each movie is displayed with its ID and title.

### `get_movie`
After entering a movie ID, sends a GET request to `/api/v1/tema/library/movies/<id>`.  
If found, displays the title, release year, description, and rating. Handles cases where the rating is a string.

### `add_movie`
Prompts for the title, year, description, and rating. Sends them as JSON in a POST request to `/api/v1/tema/library/movies`.  
If the response indicates success, the movie is added.

### `update_movie`
Receives a movie ID and allows updating one or more fields: title, description, year, and rating.  
Builds a JSON object with only the provided fields and sends a PUT request to `/api/v1/tema/library/movies/<id>`.  
Handles the case where no fields are entered.

### `delete_movie`
Receives a movie ID and sends a DELETE request to the corresponding endpoint.  
Checks for token validity and movie existence. The response message reflects the result.

## Collection Endpoints

### `get_collections`
Sends a GET request to `/api/v1/tema/library/collections`.  
If the JWT token is valid, returns a list of collections, each with its ID and title.

### `get_collection`
Receives a collection ID and sends a GET request for its details.  
The response includes the collection's title, owner, and the list of movies it contains.

### `add_collection`
Prompts for the collection title and number of movies.  
Sends multiple POST requests: one to create the collection and one for each movie added.  
Ensures sequential addition and handles possible errors.

### `delete_collection`
Receives the collection ID and sends a DELETE request to the appropriate endpoint.  
Handles possible responses: success, collection not found, or lack of permissions.

### `add_movie_to_collection`
Receives the collection ID and movie ID, builds a JSON object with the movie ID, and sends a POST request to `/collections/<col_id>/movies`.  
If the JWT token is valid, the movie is added to the collection.

### `delete_movie_from_collection`
Receives `collection_id` and `movie_id` and sends a DELETE request to the appropriate endpoint.  
The server's response confirms success or failure.
