<?php
include "db.php";

$sql = "SELECT t.task_id, t.task_title, t.due_date, t.task_status,
               c.category_name
        FROM tasks t
        JOIN categories c ON t.category_id = c.category_id
        ORDER BY t.due_date IS NULL, t.due_date, t.task_id DESC";
$tasks = $conn->query($sql);
?>
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <title>Campus Task Planner</title>
    <link rel="stylesheet" href="style.css">
</head>
<body>
<main class="container">
    <h1>Campus Task Planner</h1>
    <p class="subtitle">PHP and MySQL CRUD Application</p>
    <a class="button primary" href="task_form.php">Add New Task</a>

    <?php if (isset($_GET["message"])): ?>
        <p class="message"><?= htmlspecialchars($_GET["message"]) ?></p>
    <?php endif; ?>

    <table>
        <thead>
            <tr>
                <th>Task</th>
                <th>Category</th>
                <th>Due Date</th>
                <th>Status</th>
                <th>Actions</th>
            </tr>
        </thead>
        <tbody>
        <?php while ($task = $tasks->fetch_assoc()): ?>
            <tr>
                <td><?= htmlspecialchars($task["task_title"]) ?></td>
                <td><?= htmlspecialchars($task["category_name"]) ?></td>
                <td><?= htmlspecialchars($task["due_date"] ?? "-") ?></td>
                <td><?= htmlspecialchars($task["task_status"]) ?></td>
                <td class="actions">
                    <a class="button edit"
                       href="task_form.php?id=<?= $task["task_id"] ?>">Edit</a>
                    <a class="button delete"
                       href="delete_task.php?id=<?= $task["task_id"] ?>"
                       onclick="return confirm('Delete this task?')">Delete</a>
                </td>
            </tr>
        <?php endwhile; ?>
        </tbody>
    </table>
</main>
</body>
</html>
