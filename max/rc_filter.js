inlets = 2;
outlets = 1;

var a = 0.8;
var before = null;

var msg_float = function (r){
	if (before == null) {
		before = r;
		return;
	}
	if (this.inlet == 0) {
		var out = a * before + (1.-a)*r;
		before = out;
		outlet(0, out);
	}
}

var msg_int = function (r) {
	if (this.inlet == 1) a = r;
}

var clear = function() {
	before = null;
}