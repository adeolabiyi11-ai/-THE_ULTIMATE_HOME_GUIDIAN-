from flask import Flask, request, jsonify, render_template
import time

app = Flask(__name__)

latest_data={}
pending_command=None
last_heartbeat=0

users={
"0632adebaaqi":{
"password":"987654321",
"role":"admin",
"online":False,
"last_seen":None
}
}

intrusion_log=[]

@app.route("/")
def dashboard():
    return render_template("index.html")

@app.route("/login_page")
def login_page():
    return render_template("login.html")

def predict_rain(h):

    if h>85:
        return "High Chance Of Rain"

    elif h>70:
        return "Possible Rain"

    else:
        return "Low Chance Of Rain"

@app.route("/heartbeat")
def heartbeat():

    global last_heartbeat

    last_heartbeat=time.time()

    return jsonify({"status":"ok"})

@app.route("/device_status")
def device_status():

    if time.time()-last_heartbeat<20:
        s="ONLINE"
    else:
        s="OFFLINE"

    return jsonify({"device":s})

@app.route("/arduino/data")
def arduino_data():

    zone=request.args.get("zone")
    temp=float(request.args.get("temp"))
    hum=float(request.args.get("hum"))
    door=request.args.get("door")

    rain=predict_rain(hum)

    data={
    "temperature":temp,
    "humidity":hum,
    "door":door,
    "zone":zone,
    "rain":rain,
    "time":time.time()
    }

    latest_data.update(data)

    intrusion_log.append(data)

    return jsonify({"received":True})

@app.route("/website/status")
def status():

    return jsonify({"sensor_data":latest_data})

@app.route("/website/command",methods=["POST"])
def command():

    global pending_command

    data=request.json

    pending_command=data["command"]

    return jsonify({"stored":True})

@app.route("/arduino/get_command")
def get_command():

    global pending_command

    cmd=pending_command

    pending_command=None

    return jsonify({"command":cmd})

@app.route("/login",methods=["POST"])
def login():

    data=request.json

    u=data["username"]
    p=data["password"]

    if u in users and users[u]["password"]==p:

        users[u]["online"]=True
        users[u]["last_seen"]=time.time()

        return jsonify({"status":"success"})

    return jsonify({"status":"denied"})

@app.route("/active_users")
def active_users():

    return jsonify(users)

if __name__=="__main__":

    app.run(host="0.0.0.0",port=10000)
