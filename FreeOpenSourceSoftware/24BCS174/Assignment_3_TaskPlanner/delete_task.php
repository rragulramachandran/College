<?php
/**
 * delete_task.php - Delete Operation Handler
 * Student: Ragul Ramachandran (24BCS174)
 * Course: 19CSP01 - Free Open Source Software
 */
require_once "db.php";

$id = isset($_GET["id"]) ? (int)$_GET["id"] : 0;

if ($id > 0) {
    // Prepared statement to securely delete task (cascading removes checklists)
    $stmt = $conn->prepare("DELETE FROM tasks WHERE task_id = ?");
    $stmt->bind_param("i", $id);
    $stmt->execute();
}

header("Location: index.php?msg=deleted");
exit;
?>
