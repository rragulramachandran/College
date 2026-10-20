<?php
include "db.php";

$id = isset($_GET["id"]) ? (int) $_GET["id"] : 0;
$editing = $id > 0;
$error = "";
$task = [
    "task_title" => "",
    "due_date" => "",
    "task_status" => "Pending",
    "category_id" => ""
];

if ($editing) {
    $statement = $conn->prepare("SELECT * FROM tasks WHERE task_id = ?");
    $statement->bind_param("i", $id);
    $statement->execute();
    $task = $statement->get_result()->fetch_assoc();

    if (!$task) {
        header("Location: index.php?message=Task not found");
        exit;
    }
}

if ($_SERVER["REQUEST_METHOD"] === "POST") {
    $title = trim($_POST["task_title"] ?? "");
    $date = $_POST["due_date"] ?: null;
    $status = $_POST["task_status"] ?? "Pending";
    $category = (int) ($_POST["category_id"] ?? 0);

    if ($title === "" || $category <= 0) {
        $error = "Task title and category are required.";
    } elseif ($editing) {
        $statement = $conn->prepare(
            "UPDATE tasks
             SET task_title = ?, due_date = ?, task_status = ?, category_id = ?
             WHERE task_id = ?"
        );
        $statement->bind_param("sssii", $title, $date, $status, $category, $id);
        $statement->execute();
        header("Location: index.php?message=Task updated successfully");
        exit;
    } else {
        $statement = $conn->prepare(
            "INSERT INTO tasks (task_title, due_date, task_status, category_id)
             VALUES (?, ?, ?, ?)"
        );
        $statement->bind_param("sssi", $title, $date, $status, $category);
        $statement->execute();
        header("Location: index.php?message=Task added successfully");
        exit;
    }

    $task = [
        "task_title" => $title,
        "due_date" => $date,
        "task_status" => $status,
        "category_id" => $category
    ];
}

$categories = $conn->query(
    "SELECT category_id, category_name FROM categories ORDER BY category_name"
);
?>
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <title><?= $editing ? "Edit Task" : "Add Task" ?></title>
    <link rel="stylesheet" href="style.css">
</head>
<body>
<main class="container form-card">
    <h1><?= $editing ? "Edit Task" : "Add New Task" ?></h1>

    <?php if ($error): ?>
        <p class="error"><?= htmlspecialchars($error) ?></p>
    <?php endif; ?>

    <form method="post">
        <label for="task_title">Task Title</label>
        <input id="task_title" type="text" name="task_title"
               value="<?= htmlspecialchars($task["task_title"]) ?>" required>

        <label for="category_id">Category</label>
        <select id="category_id" name="category_id" required>
            <option value="">Select a category</option>
            <?php while ($category = $categories->fetch_assoc()): ?>
                <option value="<?= $category["category_id"] ?>"
                    <?= $task["category_id"] == $category["category_id"] ? "selected" : "" ?>>
                    <?= htmlspecialchars($category["category_name"]) ?>
                </option>
            <?php endwhile; ?>
        </select>

        <label for="due_date">Due Date</label>
        <input id="due_date" type="date" name="due_date"
               value="<?= htmlspecialchars($task["due_date"] ?? "") ?>">

        <label for="task_status">Status</label>
        <select id="task_status" name="task_status">
            <option value="Pending"
                <?= $task["task_status"] === "Pending" ? "selected" : "" ?>>Pending</option>
            <option value="Completed"
                <?= $task["task_status"] === "Completed" ? "selected" : "" ?>>Completed</option>
        </select>

        <button class="button primary" type="submit">Save Task</button>
        <a class="button edit" href="index.php">Cancel</a>
    </form>
</main>
</body>
</html>
