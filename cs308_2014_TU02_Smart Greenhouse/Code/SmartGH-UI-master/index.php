<?php require_once("prologue.php"); ?>

    <title>IIT-B Smart Greenhouse | Dashboard</title>
</head>

<body>

    <div id="wrapper">

        <nav class="navbar navbar-default navbar-static-top" role="navigation" style="margin-bottom: 0">
            <div class="navbar-header">
                <a class="navbar-brand" href="/"><b>IIT Bombay Smart Greenhouse</b></a>
		<div class="pull-right">
		    <?
			    session_start();
			    if (isset($_SESSION['login']) && $_SESSION['login'] == "true")
				echo '<a class="btn navbar-btn btn-warning" href="login.php?logout=true"><i class="fa fa-sign-out"></i> &nbsp; Log out</a>';
			    else
				echo '<a class="btn navbar-btn btn-success" href="login.php"><i class="fa fa-sign-in"></i> &nbsp; Log in</a>';
		    ?>
		</div>
            <!-- /.navbar-header -->
            </div>
        </nav>
        <!-- /.navbar-static-top -->

        <div id="page-wrapper">
            <div class="row">
                <div class="col-lg-12">
                    <h2 class="page-header">Dashboard</h2>
                </div>
                <!-- /.col-lg-12 -->
            </div>
            <!-- /.row -->
            <div class="row">
                <div class="col-lg-5 col-md-6 col-sm-5">
                    <div class="panel panel-default">
                        <div class="panel-heading">
                            <i class="fa fa-caret-square-o-right fa-fw"></i> Camera Feed
                            <div class="pull-right">
                                <div class="btn-toolbar">
                                    <div class="btn-group btn-group-xs">
                                        <button type="button" class="btn btn-default"><a id="pause-video-menu-item"><i class="fa fa-pause fa-fw"></i></a></button>
                                        <button type="button" class="btn btn-default"><a id="capture-video-menu-item"><i class="fa fa-camera fa-fw"></i></a></button>
                                    </div>
                                </div>
                            </div>
                        </div>
                        <!-- /.panel-heading -->
                        <div class="panel-body">
                            <img id="video-feed-area"></img>
                        </div>
                        <!-- /.panel-body -->
                    </div>
                    <!-- /.panel -->
                </div>
                <!-- /.col-lg-4 .col-md-6 -->
                <div class="col-lg-4 col-md-6 col-sm-7">
                    <div class="panel panel-default">
                        <div class="panel-heading">
                            <i class="fa fa-bar-chart-o fa-fw"></i> Sensor Readings
                            <div class="pull-right">
                                <div class="btn-group btn-group-xs">
                                    <button type="button" class="btn btn-default"><a id="pause-sensors-menu-item"><i class="fa fa-pause fa-fw"></i></a></button>
                                </div>
                            </div>
                        </div>
                        <!-- /.panel-heading -->
                        <div class="panel-body">
                            <div class="row">
                                <div class="col-xs-6">
                                    <div class="row">
                                        <div class="col-sm-2 col-xs-3"><strong>T<sub>0</sub></strong></div>
                                        <div class="col-sm-10 col-xs-9">
                                            <div id="current-t0-progress" class="progress progress-striped active">
                                                <div class="progress-bar" role="progressbar" aria-valuemin="5" aria-valuemax="50">
                                                    <span></span>
                                                </div>
                                            </div>
                                        </div>
                                    </div>
                                    <div class="row">
                                        <div class="col-sm-2 col-xs-3"><strong>L<sub>0</sub></strong></div>
                                        <div class="col-sm-10 col-xs-9">
                                            <div id="current-l0-progress" class="progress progress-striped active">
                                                <div class="progress-bar" role="progressbar" aria-valuemin="0" aria-valuemax="100">
                                                    <span></span>
                                                </div>
                                            </div>
                                        </div>
                                    </div>
                                    <div class="row">
                                        <div class="col-sm-2 col-xs-3"><strong>RH<sub>0</sub></strong></div>
                                        <div class="col-sm-10 col-xs-9">
                                            <div id="current-h0-progress" class="progress progress-striped active">
                                                <div class="progress-bar" role="progressbar" aria-valuemin="0" aria-valuemax="100">
                                                    <span></span>
                                                </div>
                                            </div>
                                        </div>
                                    </div>
                                </div>
                                <div class="col-xs-6">
                                    <div class="row">
                                        <div class="col-sm-2 col-xs-3"><strong>T<sub>1</sub></strong></div>
                                        <div class="col-sm-10 col-xs-9">
                                            <div id="current-t1-progress" class="progress progress-striped active">
                                                <div class="progress-bar" role="progressbar" aria-valuemin="5" aria-valuemax="50">
                                                    <span></span>
                                                </div>
                                            </div>
                                        </div>
                                    </div>
                                    <div class="row">
                                        <div class="col-sm-2 col-xs-3"><strong>L<sub>1</sub></strong></div>
                                        <div class="col-sm-10 col-xs-9">
                                            <div id="current-l1-progress" class="progress progress-striped active">
                                                <div class="progress-bar" role="progressbar" aria-valuemin="0" aria-valuemax="100">
                                                    <span></span>
                                                </div>
                                            </div>
                                        </div>
                                    </div>
                                    <div class="row">
                                        <div class="col-sm-2 col-xs-3"><strong>RH<sub>1</sub></strong></div>
                                        <div class="col-sm-10 col-xs-9">
                                            <div id="current-h1-progress" class="progress progress-striped active">
                                                <div class="progress-bar" role="progressbar" aria-valuemin="0" aria-valuemax="100">
                                                    <span></span>
                                                </div>
                                            </div>
                                        </div>
                                    </div>
                                </div>
                            </div>
			    <div class="table-responsive">
				<table id="sensor-readings-table" class="table table-bordered table-hover table-striped table-condensed">
				    <thead>
					<tr>
					    <th rowspan="2">Reading<br>Timestamp</th>
					    <th colspan="3">Sensors Module 0</th>
					    <th colspan="3">Sensors Module 1</th>
					</tr>
                                        <tr>
                                            <th>T &deg;C</th>
                                            <th>L %</th>
                                            <th>RH %</th>
                                            <th>T &deg;C</th>
                                            <th>L %</th>
                                            <th>RH %</th>
                                        </tr>
				    </thead>
				    <tbody>
				    </tbody>
				</table>
			    </div>
			    <!-- /.table-responsive -->
			</div>
                        <!-- /.panel-body -->
                    </div>
                    <!-- /.panel -->
                </div>
                <!-- /.col-lg-5 .col-md-7 -->
                <div class="col-lg-3 col-sm-12">
                    <div class="panel panel-default">
                        <div class="panel-heading">
                            <i class="fa fa-gears fa-fw"></i> Control Panel
                        </div>
                        <!-- /.panel-heading -->
                        <div class="panel-body">
                            <div id="thresholds-update-alerts"></div>
				<!-- Slider for temp, humi, etc... -->
				<label for="temperature">Maximum Temperature:</label>
				<input type="text" id="temperature" class="slide-box" readonly>
				&deg;C
				<div id="tempSlider" class="temp"></div>
				<br/>
			
				<label for="light">Minimum Light Intensity:</label>
				<input type="text" id="light" class="slide-box" readonly>
				%
				<div id="lighSlider" class="ligh"></div>
				<br/>
			
				<label for="humidity">Minimum Relative Humidity:</label>
				<input type="text" id="humidity" class="slide-box" readonly>
				%
				<div id="humiSlider" class="humi"></div>
				<br/>
				
				<button class="btn btn-large btn-primary" id="submit-thresholds-button"><i class="fa fa-save fa-lg"></i> &nbsp; Save</button>
				<div class="pull-right">
				    <button class="btn btn-large" id="reset-thresholds-button"><i class="fa fa-times-circle-o fa-lg"></i> &nbsp; Reset</button>
				</div>
						
                        	<div class="list-group" id="notification-panel"></div>
                            <!-- /.list-group -->
                        </div>
                        <!-- /.panel-body -->
                    </div>
                    <!-- /.panel -->
                </div>
                <!-- /.col-lg-3 .col-md-12 -->
            </div>
            <!-- /.row -->
        </div>
        <!-- /#page-wrapper -->

    </div>
    <!-- /#wrapper -->

    <?php require_once("epilogue.php"); ?>
    <script src="js/sb-admin.js"></script>
    
</body>

</html>
