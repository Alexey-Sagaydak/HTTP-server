ip="0.0.0.0:7777"

user_data="{\"username\": \"admin\", \"password\": \"admin\", \"isAdmin\": true}"
curl -X POST -H "Content-Type: application/json" -d "${user_data}" "http://${ip}/user"

user_data="{\"username\": \"user\", \"password\": \"user\", \"isAdmin\": false}"
curl -X POST -H "Content-Type: application/json" -d "${user_data}" "http://${ip}/user"

