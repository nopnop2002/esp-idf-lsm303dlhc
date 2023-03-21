//document.getElementById("datetime").innerHTML = "WebSocket is not connected";

var websocket = new WebSocket('ws://'+location.hostname+'/');
var xmax = -9999;
var xmin = 9999;
var xcenter = 0;
var xshift = 0;
var ymax = -9999;
var ymin = 9999;
var ycenter = 0;
var yshift = 0;
var zmax = -9999;
var zmin = 9999;
var zcenter = 0;
var zshift = 0;

function ClearValuePush() {
	console.log('ClearChartPush');
	xmax = -9999;
	xmin = 9999;
	xcenter = 0;
	xshift = 0;
	ymax = -9999;
	ymin = 9999;
	ycenter = 0;
	yshift = 0;
	zmax = -9999;
	zmin = 9999;
	zcenter = 0;
	zshift = 0;
}

function sendText(name) {
	console.log('sendText');
	var data = {};
	data["id"] = name;
	console.log('data=', data);
	json_data = JSON.stringify(data);
	console.log('json_data=' + json_data);
	websocket.send(json_data);
}

websocket.onopen = function(evt) {
	console.log('WebSocket connection opened');
	var data = {};
	data["id"] = "init";
	console.log('data=', data);
	json_data = JSON.stringify(data);
	console.log('json_data=' + json_data);
	websocket.send(json_data);
	//document.getElementById("datetime").innerHTML = "WebSocket is connected!";
}

websocket.onmessage = function(evt) {
	var msg = evt.data;
	console.log("msg=" + msg);
	var values = msg.split('\4'); // \4 is EOT
	//console.log("values=" + values);
	switch(values[0]) {
		case 'HEAD':
			console.log("HEAD values[1]=" + values[1]);
			var h1 = document.getElementById( 'header' );
			h1.textContent = values[1];
			break;

		case 'DATA':
			//console.log("DATA values[1]=" + values[1]);
			//console.log("DATA values[2]=" + values[2]);
			//console.log("DATA values[3]=" + values[3]);
			var xval = parseInt(values[1], 10);
			var yval = parseInt(values[2], 10);
			var zval = parseInt(values[3], 10);
			if (xval > 1000 || xval < -1000) return;
			if (yval > 1000 || yval < -1000) return;
			if (zval > 1000 || zval < -1000) return;

			Plotly.extendTraces('myDiv1', {
			    x: [[xval]],
			    y: [[yval]]
			}, [0])
			Plotly.extendTraces('myDiv2', {
			    x: [[xval]],
			    y: [[zval]]
			}, [0])

			if (xval < xmin) xmin = xval;
			if (xval > xmax) xmax = xval;
			//console.log("xmin=" + xmin);
			//console.log("xmax=" + xmax);
			xcenter = (xmax - xmin) / 2.0;
			//console.log("xcenter=" + xcenter);
			xshift = xcenter - xmax;
			//console.log("xshift=" + xshift);
			document.getElementById('xmin_val').innerText = xmin.toString();
			document.getElementById('xmax_val').innerText = xmax.toString();
			document.getElementById('xcenter_val').innerText = xcenter.toString();
			document.getElementById('xshift_val').innerText = xshift.toString();

			if (yval < ymin) ymin = yval;
			if (yval > ymax) ymax = yval;
			//console.log("ymin=" + ymin);
			//console.log("ymax=" + ymax);
			ycenter = (ymax - ymin) / 2.0;
			//console.log("ycenter=" + ycenter);
			yshift = ycenter - ymax;
			//console.log("yshift=" + yshift);
			document.getElementById('ymin_val').innerText = ymin.toString();
			document.getElementById('ymax_val').innerText = ymax.toString();
			document.getElementById('ycenter_val').innerText = ycenter.toString();
			document.getElementById('yshift_val').innerText = yshift.toString();

			if (zval < zmin) zmin = zval;
			if (zval > zmax) zmax = zval;
			//console.log("zmin=" + zmin);
			//console.log("zmax=" + zmax);
			zcenter = (zmax - zmin) / 2.0;
			//console.log("zcenter=" + zcenter);
			zshift = zcenter - zmax;
			//console.log("zshift=" + zshift);
			document.getElementById('zmin_val').innerText = zmin.toString();
			document.getElementById('zmax_val').innerText = zmax.toString();
			document.getElementById('zcenter_val').innerText = zcenter.toString();
			document.getElementById('zshift_val').innerText = zshift.toString();

		default:
			break;
	}
}

websocket.onclose = function(evt) {
	console.log('Websocket connection closed');
	//document.getElementById("datetime").innerHTML = "WebSocket closed";
}

websocket.onerror = function(evt) {
	console.log('Websocket error: ' + evt);
	//document.getElementById("datetime").innerHTML = "WebSocket error????!!!1!!";
}
