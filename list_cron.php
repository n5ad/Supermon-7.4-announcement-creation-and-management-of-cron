<?php
header('Content-Type: application/json');

$lines = [];
exec('sudo crontab -l', $lines);

$result = [];
$last_desc = '';

foreach ($lines as $line) {
    $line = trim($line);

    if ($line === '') continue;

    // Capture description comment
    if (strpos($line, '# DESC:') === 0) {
        $last_desc = trim(substr($line, 7));
        continue;
    }

    // Skip other comments
    if ($line[0] === '#') continue;

    // Only match playaudio.sh entries
    if (!preg_match(
        '/^(\S+\s+\S+\s+\S+\s+\S+\s+\S+)\s+.*playaudio\.sh\s+(\S+)/',
        $line,
        $m
    )) {
        continue;
    }

    $result[] = [
        'time' => $m[1],
        'file' => basename($m[2]),
        'desc' => $last_desc,
        'raw'  => $line
    ];

    $last_desc = '';
}

echo json_encode($result, JSON_PRETTY_PRINT);
