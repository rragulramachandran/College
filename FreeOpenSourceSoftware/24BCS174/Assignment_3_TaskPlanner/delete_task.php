<?php
include "db.php";

$id = isset($_GET["id"]) ? (int) $_GET["id"] : 0;

if ($id > 0) {
    $statement = $conn->prepare("DELETE FROM tasks WHERE task_id = ?");
    $statement->bind_param("i", $id);
    $statement->execute();
}

header("Location: index.php?message=Task deleted successfully");
exit;
?>
