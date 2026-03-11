async function update(){

let r=await fetch("/website/status")
let d=await r.json()

document.getElementById("temp").innerText=d.sensor_data.temperature
document.getElementById("hum").innerText=d.sensor_data.humidity
document.getElementById("door").innerText=d.sensor_data.door
document.getElementById("rain").innerText=d.sensor_data.rain

drawMap(d.sensor_data.zone)

}

async function device(){

let r=await fetch("/device_status")
let d=await r.json()

document.getElementById("deviceStatus").innerText=d.device

}

async function sendCommand(){

let c=document.getElementById("commandBox").value

await fetch("/website/command",{

method:"POST",
headers:{"Content-Type":"application/json"},
body:JSON.stringify({command:c})

})

}

function voice(){

let rec=new webkitSpeechRecognition()

rec.onresult=function(e){

let t=e.results[0][0].transcript

document.getElementById("commandBox").value=t

sendCommand()

}

rec.start()

}

function drawMap(zone){

const c=document.getElementById("houseMap")
const ctx=c.getContext("2d")

ctx.clearRect(0,0,300,200)

ctx.strokeRect(20,20,80,60)
ctx.strokeRect(120,20,80,60)
ctx.strokeRect(220,20,60,60)

if(zone=="MOTION"){

ctx.fillStyle="red"
ctx.fillRect(120,20,80,60)

}

}

setInterval(update,3000)
setInterval(device,4000)
