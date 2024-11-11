from flask import Flask, jsonify
import requests

app = Flask(__name__)

@app.route('/weather', methods=['GET'])
def get_weather():
    api_key = "a94921e0493df026422be9ae30edaf9c"
    city = "Novosibirsk"
    url = f"http://api.openweathermap.org/data/2.5/weather?q={city}&appid={api_key}&units=metric"
    
    response = requests.get(url)
    data = response.json()
    
    weather_info = {
        "city": city,
        "temperature": data["main"]["temp"],
        "description": data["weather"][0]["description"]
    }
    
    return jsonify(weather_info)

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)
