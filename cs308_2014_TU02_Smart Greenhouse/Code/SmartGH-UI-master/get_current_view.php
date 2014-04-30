<?php

        $filename = "/var/log/cameramon/camera.dat";
        $filehandle = fopen($filename, 'rb');

        header("Content-Type: image/jpeg");
        header("Content-Length: " . filesize($filename));

        fpassthru($filehandle);
