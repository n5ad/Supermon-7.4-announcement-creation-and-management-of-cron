<?php
/**
*created by n5ad
*date 1-6-2026
*/

$files = glob("/mp3/*.mp3");


$out = [];


foreach ($files as $f) {


    $out[] = basename($f);


}



header('Content-Type: application/json');


echo json_encode($out);


