<?
	session_start();

	if($_GET['logout'] == 'true') {
		session_destroy();
		header("Location: index.php");
		die();
        }

	if($_SESSION['login'] == 'true') {
		header('Location: index.php');
		exit();
	}

	if($_POST['username'] == 'sanket' && $_POST['password'] == 'totala') {
		$_SESSION['login']="true";
		header('Location: index.php');
		exit();
	} else session_destroy();
?>

<?php require_once("prologue.php"); ?>

    <title>IIT-B Smart Greenhouse | Admin Login</title>

</head>

<body>

    <div class="container">
        <div class="row">
            <div class="col-md-6 col-md-offset-3 col-sm-8 col-sm-offset-2 col-xs-10 col-xs-offset-1">
                <div class="login-panel panel panel-default">
                    <div class="panel-heading">
                        <b class="panel-title">IIT Bombay Smart Greenhouse Admin Login</b>
                    </div>
                    <div class="panel-body">
                        <form class="form-horizontal" role="form" action="login.php" method="post">
                            <fieldset>
                                <div class="form-group">
				    <label for="username" class="col-xs-1 control-label"><i class="fa fa-user fa-lg text-info"></i></label>
                                    <div class="col-xs-11"><input class="form-control" placeholder="f005ba11" name="username" type="username" autofocus></div>
                                </div>
                                <div class="form-group">
				    <label for="username" class="col-xs-1 control-label"><i class="fa fa-key fa-lg text-info"></i></label>
                                    <div class="col-xs-11"><input class="form-control" placeholder="b3a7#m3" name="password" type="password" value=""></div>
                                </div>
                                <div class="form-group">
				    <div class="col-xs-11 col-xs-offset-1"
				    	<div class="checkbox">
					    <label>
					        <input name="remember" type="checkbox" value="Remember Me"> Remember me on this browser
					    </label>
					</div>
				    </div>
                                </div>

                                <!-- Change this to a button or input when using this as a form -->
                                <center><button type="submit" value="submit" class="btn btn-default">Log in</button></center>
                                <br/>
                            </fieldset>
                        </form>
                    </div>
                </div>
            </div>
        </div>
    </div>

    <?php require_once("epilogue.php"); ?>

</body>

</html>
