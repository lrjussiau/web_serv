#!/usr/bin/php

<?php
date_default_timezone_set('Europe/Zurich');
$current_time = date('Y-m-d H:i:s');
?>

<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Current Time in Switzerland</title>
    <style>
        body {
            display: flex;
            justify-content: center;
            align-items: center;
            height: 100vh;
            margin: 0;
            font-family: Arial, sans-serif;
            background-color: #f0f0f0;
        }
        h1 {
            color: #333;
        }
    </style>
</head>
<body>
    <h1>The current time in Switzerland is: <?php echo $current_time; ?></h1>
	<button onclick="location.href='index.html'">Go Back to Home</button>
</body>
</html>