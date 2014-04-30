<html>
<head>
	<title>Robot Navigation</title>
	<style>
		body {background-color:black;}
		h1.tableHeader {color:white; font-family:Arial, Helvetica, sans-serif; font-size:34px}
		img.direction {padding-left:150px; display:none;}

	</style>
</head>
<body>
<table>
<tr>
	<th><h1 class="tableHeader">Robot Live Feed</h1></th>
	<th></th>
</tr>
<tr>
	<td>
		<img src="http://192.168.173.223:8080/video" alt="Video feed"/>
	</td>
	<td>
		<img class="direction" src="images/up_front_white.png" alt="Direction" height="180" width="180"/>
	</td>
</tr>


<script type="text/javascript" language="javascript" src="js/jquery.js"></script>
	<script type="text/javascript" language="javascript">
		
		$(document).keydown(function(e) {
			switch(e.which) {
				case 32: // stop
				$('#keyName').append('Stop\n');
				$("img.direction").fadeIn(100);
				$("img.direction").attr("src", "images/stop_white.png");
				$.ajax({
					url: 'stop.php',
					success: function (response) {
						$('#keyName').append('Success : Stop\n');
					}
				});
				break;
				
				case 37: // left
				$('#keyName').append('Left\n');
				$("img.direction").fadeIn(100);
				$("img.direction").attr("src", "images/left_front_white.png");
				$.ajax({
					url: 'left.php',
					success: function (response) {
						$('#keyName').append('Success : Left\n');
					}
				});
				break;

				case 38: // up
				$('#keyName').append('Up\n');
				$("img.direction").fadeIn(100);
				$("img.direction").attr("src", "images/up_front_white.png");
				$.ajax({
					url: 'up.php',
					success: function (response) {
						$('#keyName').append('Success : Up\n');
					}
				});
				break;

				case 39: // right
				$('#keyName').append('Right\n');
				$("img.direction").fadeIn(100);
				$("img.direction").attr("src", "images/right_front_white.png");
				$.ajax({
					url: 'right.php',
					success: function (response) {
						$('#keyName').append('Success : Right\n');
					}
				});
				break;

				case 40: // down
				$('#keyName').append('Down\n');
				$("img.direction").fadeIn(100);
				$("img.direction").attr("src", "images/down_front_white.png");
				$.ajax({
					url: 'down.php',
					success: function (response) {
						$('#keyName').append('Success : Down\n');
					}
				});
				break;

				default: return; // exit this handler for other keys
			}
			e.preventDefault(); // prevent the default action (scroll / move caret)
		});
		
		$(document).keyup(function(e) {
			$('#keyName').append('Key Lift\n');
			$("img.direction").fadeOut(200);
			$.ajax({
				url: 'stop.php',
				success: function (response) {
					$('#keyName').append('Success : Stop\n');
				}
			});
			e.preventDefault(); // prevent the default action (scroll / move caret)
		});
	</script>
</body>
</html>
<!--
<html>
<head>
	<style>
		td { padding:0px; position:relative; }
		span.overlay_img {
			visibility:visible;
			position:absolute;
			left:0%;
			top:0%;
		}
		img.back {
			filter:alpha(opacity=30);
			-moz-opacity:0.3;
			-khtml-opacity: 0.3;
			opacity: 0.3;
		}

		
	</style>
</head>
<body>
<table>
	<tr>
		<td></td>
		<td><img src="images/up_back.jpg" height="50" width="50"></td>
		<td></td>
	</tr>
	<tr>
		<td><img src="images/left_back.jpg" height="50" width="50"></td>
		<td><img src="images/down_back.jpg" height="50" width="50"></td>
		<td><img class="back" src="images/right_back.jpg" height="50" width="50"><span class="overlay_img"><img src="images/right_front_red.png" height="50" width="50"></span></td>
	</tr>
</table>
</div>
</html>

-->