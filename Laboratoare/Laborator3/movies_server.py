from flask import Flask, json, Response, request
from itertools import count, filterfalse

server = Flask(__name__)
MOVIES = {}


def _get_first_available_id():
	return next(filterfalse(set(MOVIES.keys()).__contains__, count(1)))


@server.route("/movies", methods=["GET", "POST"])
@server.route("/movie/<id>", methods=["GET", "PUT", "DELETE"])
def handle_request(id=None):
	global MOVIES

	if request.method == "GET":
		if id:
			movie_id = int(id)
			if movie_id not in MOVIES:
				return Response(status=404)

			movies = {"id": movie_id, "nume": MOVIES[movie_id]["nume"]}
		else:
			movies = [{"id": movie_id, "nume": movie["nume"]}
				for movie_id, movie in MOVIES.items()]

		return Response(response=json.dumps(movies), status=200,
			mimetype="application/json")
	elif request.method == "POST":
		json_data = request.json

		if json_data and "nume" in json_data and json_data["nume"] != "":
			movie_id = _get_first_available_id()
			MOVIES[movie_id] = json_data

			return Response(status=201)
		else:
			return Response(status=400)
	elif request.method == "PUT":
		movie_id = int(id)

		if movie_id not in MOVIES:
			return Response(status=404)

		json_data = request.json
		if json_data and "nume" in json_data and json_data["nume"] != "":
			MOVIES[movie_id] = json_data
			return Response(status=200)

		return Response(status=404)
	elif request.method == "DELETE":
		movie_id = int(id)
		if movie_id not in MOVIES:
			return Response(status=404)

		del(MOVIES[movie_id])

		return Response(status=200)
	else:
		return Response(status=400)


@server.route("/reset", methods=["DELETE"])
def delete_all():
	global MOVIES
	MOVIES = {}

	return Response(status=200)

if __name__ == '__main__':
	server.run('0.0.0.0', debug=True)
