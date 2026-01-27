<?php
/**
 * delete_file.php - Delete MP3/WAV or UL files for Announcements Manager
 * Handles deletion of files in /mp3/ (direct) or Asterisk sounds dir (sudo rm)
 * CREATED BY N5AD
 */

header('Content-Type: application/json');

if ($_SERVER['REQUEST_METHOD'] !== 'POST') {
    http_response_code(405);
    echo json_encode(['success' => false, 'message' => 'Method not allowed.']);
    exit;
}

$type = trim($_POST['type'] ?? '');
$filename = trim($_POST['file'] ?? '');

if (empty($type) || empty($filename)) {
    echo json_encode(['success' => false, 'message' => 'Missing type or file.']);
    exit;
}

// Sanitize filename (allow only safe characters)
$filename = basename($filename);
$filename = preg_replace('/[^a-zA-Z0-9_.-]/', '', $filename);

if ($type === 'mp3' || $type === 'wav') {
    // MP3/WAV in /mp3/ - direct unlink (no sudo needed)
    $file_path = "/mp3/" . $filename;
    if ($type === 'wav' && strpos($filename, '.wav') === false) {
        $file_path .= '.wav';
    } elseif ($type === 'mp3' && strpos($filename, '.mp3') === false) {
        $file_path .= '.mp3';
    }

    if (!file_exists($file_path)) {
        echo json_encode(['success' => false, 'message' => "File not found: $filename"]);
        exit;
    }

    if (unlink($file_path)) {
        echo json_encode(['success' => true, 'message' => "Deleted $filename successfully."]);
    } else {
        echo json_encode(['success' => false, 'message' => "Failed to delete $filename (MP3/WAV). Check permissions."]);
    }
} elseif ($type === 'ul') {
    // UL in Asterisk sounds dir - use sudo rm -f
    $file_path = "/usr/local/share/asterisk/sounds/" . $filename;
    if (strpos($filename, '.ul') === false) {
        $file_path .= '.ul';
    }

    if (!file_exists($file_path)) {
        echo json_encode(['success' => false, 'message' => "File not found: $filename.ul"]);
        exit;
    }

    // Use sudo rm -f (requires sudoers rule)
    $cmd = "sudo rm -f " . escapeshellarg($file_path);
    exec($cmd, $output, $retval);

    if ($retval === 0) {
        echo json_encode(['success' => true, 'message' => "Deleted $filename.ul successfully."]);
    } else {
        error_log("Delete failed for $file_path. Cmd: $cmd | Output: " . implode("\n", $output));
        echo json_encode(['success' => false, 'message' => "Failed to delete $filename.ul. Check sudo permissions or logs."]);
    }
} else {
    echo json_encode(['success' => false, 'message' => "Invalid type: $type"]);
}
?>
