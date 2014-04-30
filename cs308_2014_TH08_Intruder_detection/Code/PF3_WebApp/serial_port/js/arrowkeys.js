$(document).ready(function(){
	$(document).keydown(function(e) {
		switch(e.which) {
			case 37: // left
			alert('Left Press');
			break;

			case 38: // up
			alert('Up Press');
			break;

			case 39: // right
			alert('Right Press');
			break;

			case 40: // down
			alert('Down Press');
			break;

			default: return; // exit this handler for other keys
		}
		e.preventDefault(); // prevent the default action (scroll / move caret)
	});
	
	$(document).keyup(function(e) {
		alert('Key Up');
		e.preventDefault(); // prevent the default action (scroll / move caret)
	});
});