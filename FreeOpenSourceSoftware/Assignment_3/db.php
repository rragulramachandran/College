<?php
/**
 * db.php - Database Configuration and Connection Handler
 * Student: Ragul Ramachandran (24BCS174)
 * Course: 19CSP01 - Free Open Source Software
 */

$host     = "127.0.0.1";
$user     = "root";
$password = "";
$database = "foss_task_planner";
$port     = (int) (getenv("TASK_DB_PORT") ?: 3306);

// Establish database connection using mysqli
$conn = @new mysqli($host, $user, $password, $database, $port);

if ($conn->connect_error) {
    die("<div style='font-family:sans-serif;padding:20px;color:#721c24;background:#f8d7da;border:1px solid #f5c6cb;border-radius:6px;max-width:600px;margin:40px auto;'>"
        . "<strong>Database Connection Error:</strong> " . htmlspecialchars($conn->connect_error)
        . "<br><small>Please verify that MySQL service is running and credentials are accurate.</small></div>");
}

$conn->set_charset("utf8mb4");
?>
