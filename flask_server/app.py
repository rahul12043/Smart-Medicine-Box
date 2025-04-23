from flask import Flask, request, jsonify, render_template
import datetime

app = Flask(__name__)

latest_data = {
    "time": None,
    "temperature": None,
    "humidity": None,
    "ldr": None,
    "ir": None,
    "distance": None,
    "timestamp": None,
    "alarm": None,       # Now in HH:MM:SS format
    "alarm_triggered": False
}

pending_commands = []

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/set_alarm', methods=['POST'])
def set_alarm():
    data = request.get_json()
    
    if data.get('clear'):
        latest_data['alarm'] = None
        pending_commands.append("CLEAR_ALARM")
        return jsonify({"status": "cleared"})
    
    try:
        hour = int(data['hour'])
        minute = int(data['minute'])
        second = int(data.get('second', 0))  # Default to 0 if not provided
        
        if all([0 <= hour < 24, 0 <= minute < 60, 0 <= second < 60]):
            alarm_time = f"{hour:02d}:{minute:02d}:{second:02d}"  # HH:MM:SS format
            latest_data['alarm'] = alarm_time
            pending_commands.append(f"SET_ALARM:{hour}:{minute}:{second}")
            return jsonify({"status": "set", "alarm": alarm_time})
        
        return jsonify({"error": "Invalid time"}), 400
        
    except Exception as e:
        return jsonify({"error": str(e)}), 400

@app.route('/get_command', methods=['GET'])
def get_command():
    if pending_commands:
        return pending_commands.pop(0)
    return ""

@app.route('/log_data', methods=['POST'])
def log_data():
    data = request.get_json()
    print("Received raw JSON:", data)

    required_fields = ['time', 'temperature', 'humidity', 'ldr', 'ir', 'distance']
    if not data or not all(field in data for field in required_fields):
        return jsonify({"error": "Missing one or more fields"}), 400

    latest_data.update({
        "time": data['time'],
        "temperature": data['temperature'],
        "humidity": data['humidity'],
        "ldr": data['ldr'],
        "ir": data['ir'],
        "distance": data['distance'],
        "timestamp": datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S')
    })

    print(f"Data received at {latest_data['timestamp']}")
    return jsonify({"message": "Data logged successfully"}), 200

@app.route('/latest')
def get_latest():
    return jsonify(latest_data)

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)