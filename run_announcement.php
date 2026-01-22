<?php

// run_announcement.php

// Plays a .ul file immediately on the AllStar node
// Created by N5AD

// Only accept POST requests
if ($_SERVER['REQUEST_METHOD'] !== 'POST') {
    http_response_code(405);
    echo "Method not allowed.";
    exit;
}

if (empty($_POST['file'])) {
    echo "No file specified.";
    exit;
}

// Sanitize input
$base = basename($_POST['file']); // prevents path traversal, gets just the filename part
$sounds_dir = "/usr/local/share/asterisk/sounds";

// Try possible file locations (with .ul first, then without)
$possible_files = [
    $sounds_dir . "/" . $base . ".ul",
    $sounds_dir . "/" . $base,
];

$full_path = null;
foreach ($possible_files as $candidate) {
    if (file_exists($candidate)) {
        $full_path = $candidate;
        break;
    }
}

// Check if we found a valid file
if (!$full_path) {
    echo "UL file not found: $base (tried $base.ul and $base)";
    exit;
}

// Derive the base name **without extension** for playaudio.sh
$base_name = pathinfo($full_path, PATHINFO_FILENAME);

// Path to play script
$play_script = "/etc/asterisk/local/playaudio.sh";

// Verify play script exists and is executable
if (!is_executable($play_script)) {
    echo "playaudio.sh not found or not executable.";
    exit;
}

// Command to run: playaudio.sh expects filename **without extension**
$cmd = escapeshellcmd("sudo $play_script $base_name");

// Run the command
exec($cmd . " 2>&1", $output, $retval);

if ($retval === 0) {
    echo "Playing $base_name now.";
} else {
    echo "Failed to play $base_name. Output: " . implode("\n", $output);
}

?>