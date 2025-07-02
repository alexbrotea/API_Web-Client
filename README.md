    Justification for using Parson: it is lightweight, portable, and has no external dependencies.
I believe it works quite well for this project, which specifically requires fast JSON object
parsing and simple interaction with primitive values and arrays, without making things overly
complicated.

    For the actual implementation of the project, I used my solution from Lab 9 of PCOM as a
skeleton. I reused the code from requests anhelpers and adapted them to the needs of this assignment.
I significantly developed the client, which, in the lab, only handled a simple GET or POST request.
I extended its logic to also handle cookies and JWTs.

    login_admin
    This function prompts for the administrator’s username and password from the keyboard.
These credentials are packed into a JSON object and sent to the server via an HTTP POST request to
/api/v1/tema/admin/login. If authentication is successful, the response contains a session cookie,
which is extracted from the Set-Cookie header and saved for later use. This cookie essentially confirms
the admin login.

    logout_admin
    This function checks whether an admin cookie is active. If so, it sends a GET request to
/api/v1/tema/admin/logout to end the session. If the logout is accepted by the server, the cookie is
cleared from memory, signaling the end of the session.

    add_user
    The admin can create a new user. The new user’s username and password are read and sent as JSON in
a POST request to /api/v1/tema/admin/users. The request also includes the admin cookie for authorization.
If the response is successful, a confirmation message is displayed.

    get_users
    This function sends a GET request to /api/v1/tema/admin/users to retrieve the list of all existing users.
The response is a JSON object containing an array of users. Each user is displayed with their ID, username,
and password.

    delete_user
    The function receives a username from the keyboard, constructs the URL /api/v1/tema/admin/users/<username>,
and sends a DELETE request. Depending on the server's response, a confirmation or error message is shown.

    login
    The user enters admin_username, username, and password, which are sent in a JSON object to the endpoint
/api/v1/tema/user/login. If login is successful, the session cookie is extracted and saved, as it will be
required for future library-related actions.

    logout
    If the user is logged in, a GET request is sent to /api/v1/tema/user/logout. If it's successful, the
cookie is cleared, and the session ends.

    get_access
    This function uses the user’s cookie to send a GET request to /api/v1/tema/library/access.
The server responds with a token that is extracted from the JSON body and stored. This token is required
for all movie and collection-related actions.

    get_movies
    Using the JWT token, the client sends a GET request to /api/v1/tema/library/movies.
The response contains a list of movies, either as a JSON array or an object with the movies field.
Each movie is displayed with its ID and title.

    get_movie
    After entering an ID, a GET request is sent to /api/v1/tema/library/movies/<id>.
If the movie exists, its title, release year, description, and rating are displayed.
Special cases like rating expressed as a string are handled.

    add_movie
    All movie details are entered: title, year, description, and rating. These are packed into a JSON
object and sent via a POST request to /api/v1/tema/library/movies. If the response indicates success,
the movie is considered added.

    update_movie
    The function receives an ID and allows updating one or more fields: title, description, year, and rating.
A JSON object is built with only the filled-in fields and sent as a PUT request to
/api/v1/tema/library/movies/<id>. The case where no fields are provided is also handled.

    delete_movie
    A movie ID is entered, and a DELETE request is sent to the corresponding endpoint. The validity of the
token and the existence of the movie are checked, and the response message reflects the operation's outcome.

get_collections
A GET request is sent to /api/v1/tema/library/collections. If the JWT token is valid, a list of all collections is returned.
Each collection is displayed with its ID and title.

get_collection
An ID is entered and a GET request is made for its details.
The response includes the collection's title, owner, and list of included movies.

add_collection
The collection title and number of movies are entered.
Multiple POST requests are sent: one for creating the collection and one for each movie added.
The movies are added sequentially, and potential errors are handled.

delete_collection
The collection ID is entered, and a DELETE request is sent to the corresponding endpoint.
All possible server responses are handled: success, nonexistent collection, or lack of permissions.

add_movie_to_collection
This function receives the collection ID and the movie ID, builds a JSON object with the movie ID, and sends a POST request to /collections/<col_id>/movies.
If the JWT token is valid, the movie is added to the collection.

delete_movie_from_collection
The collection_id and movie_id are entered, and a DELETE request is sent to the appropriate endpoint.
The server's response confirms whether the operation was successful or not.
    