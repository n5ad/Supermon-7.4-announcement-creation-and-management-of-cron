<?php

if(!isset($_POST['raw_line'])) { echo "Error: Missing cron line"; exit; }

$raw = trim($_POST['raw_line']);


// Remove from root's crontab

$tempfile = tempnam(sys_get_temp_dir(), 'cron');

exec('sudo crontab -l', $crons);

file_put_contents($tempfile, '');

foreach($crons as $line){

    if(trim($line) !== $raw){

        file_put_contents($tempfile, $line.PHP_EOL, FILE_APPEND);

    }

}

exec("sudo crontab $tempfile");

unlink($tempfile);


echo "Deleted cron for $raw";

?>

