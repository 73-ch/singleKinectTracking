inlets = 3;
outlets = 1;

var array = [];
var max_array = 5;
var calc = 0;

function getSum (g) {
	var s = 0;
	g.forEach(function(e) {
		s += e;
	});
	
	return s;
}

function getMedian (g) {
	var half = (g.length/2)|0;
	var temp = g.sort();
	
	if (temp.length%2) {
		return temp[half];
	}
	
	return (temp[half-1] + temp[half])/2;
}

var msg_float = function (r){
	if (this.inlet == 0) {
		array.push(r);
	
		while (array.length > max_array) {
			array.shift();
		}
		
		if (calc == 0) {
			outlet(0, getSum(array) / array.length);
		} else if (calc == 1) {
			outlet(0, getMedian(array));
		}
	}
}

var msg_int = function (r) {
	if (this.inlet == 1) max_array = r;
	if (this.inlet == 2) calc = r;
}
	