var inlets = 2;
var outlets = 1;

var multiplier = 0.5;

var msg_float = function(r){
    switch(this.inlet){
        case 0:
        post("inlet_0\n");
        outlet(0,r * multiplier);
        break;

        case 1:
        post("inlet_1\n");
        multiplier = r;
        break;

        default:
        break;
    }
}