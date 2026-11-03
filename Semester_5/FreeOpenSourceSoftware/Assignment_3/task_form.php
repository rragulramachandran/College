<?php
/**
 * task_form.php - Create & Update Operation Form
 * Student: Ragul Ramachandran (24BCS174)
 * Course: 19CSP01 - Free Open Source Software
 */
require_once "db.php";

$id = isset($_GET["id"]) ? (int)$_GET["id"] : 0;
$editing = $id > 0;
$error = "";

$task = [
    "task_title"   => "",
    "description"  => "",
    "due_date"     => "",
    "task_status"  => "Pending",
    "category_id"  => 1,
    "priority_id"  => 2
];
$subtasks = [];

if ($editing) {
    $stmt = $conn->prepare("SELECT * FROM tasks WHERE task_id = ?");
    $stmt->bind_param("i", $id);
    $stmt->execute();
    $task_res = $stmt->get_result()->fetch_assoc();
    if (!$task_res) {
        header("Location: index.php?msg=notfound");
        exit;
    }
    $task = $task_res;

    // Fetch subtasks
    $stmt_st = $conn->prepare("SELECT * FROM task_checklists WHERE task_id = ? ORDER BY checklist_id ASC");
    $stmt_st->bind_param("i", $id);
    $stmt_st->execute();
    $subtasks_res = $stmt_st->get_result();
    while ($st = $subtasks_res->fetch_assoc()) {
        $subtasks[] = $st["item_title"];
    }
}

// Handle Form Submission
if ($_SERVER["REQUEST_METHOD"] === "POST") {
    $title       = trim($_POST["task_title"] ?? "");
    $description = trim($_POST["description"] ?? "");
    $due_date    = !empty($_POST["due_date"]) ? $_POST["due_date"] : null;
    $status      = $_POST["task_status"] ?? "Pending";
    $category_id = (int)($_POST["category_id"] ?? 0);
    $priority_id = (int)($_POST["priority_id"] ?? 0);
    $new_subtask = trim($_POST["new_subtask"] ?? "");

    if ($title === "") {
        $error = "Task Title is mandatory and cannot be empty.";
    } elseif ($category_id <= 0) {
        $error = "Please select a valid Category.";
    } elseif ($priority_id <= 0) {
        $error = "Please select a valid Priority level.";
    } else {
        if ($editing) {
            // Update Existing Task
            $stmt = $conn->prepare("UPDATE tasks SET task_title = ?, description = ?, due_date = ?, task_status = ?, category_id = ?, priority_id = ? WHERE task_id = ?");
            $stmt->bind_param("ssssiii", $title, $description, $due_date, $status, $category_id, $priority_id, $id);
            $stmt->execute();

            // Insert new subtask if provided
            if (!empty($new_subtask)) {
                $stmt_chk = $conn->prepare("INSERT INTO task_checklists (task_id, item_title) VALUES (?, ?)");
                $stmt_chk->bind_param("is", $id, $new_subtask);
                $stmt_chk->execute();
            }

            header("Location: index.php?msg=updated");
            exit;
        } else {
            // Create New Task
            $stmt = $conn->prepare("INSERT INTO tasks (task_title, description, due_date, task_status, category_id, priority_id) VALUES (?, ?, ?, ?, ?, ?)");
            $stmt->bind_param("ssssii", $title, $description, $due_date, $status, $category_id, $priority_id);
            $stmt->execute();
            $new_id = $stmt->insert_id;

            // Insert optional subtask
            if (!empty($new_subtask)) {
                $stmt_chk = $conn->prepare("INSERT INTO task_checklists (task_id, item_title) VALUES (?, ?)");
                $stmt_chk->bind_param("is", $new_id, $new_subtask);
                $stmt_chk->execute();
            }

            header("Location: index.php?msg=created");
            exit;
        }
    }

    $task = [
        "task_title"   => $title,
        "description"  => $description,
        "due_date"     => $due_date,
        "task_status"  => $status,
        "category_id"  => $category_id,
        "priority_id"  => $priority_id
    ];
}

// Fetch categories and priorities for dropdowns
$categories = $conn->query("SELECT * FROM categories ORDER BY category_name ASC");
$priorities = $conn->query("SELECT * FROM priorities ORDER BY priority_level ASC");
?>
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title><?= $editing ? "Edit Task" : "Create New Task" ?> - Campus Task Planner</title>
    <link rel="stylesheet" href="style.css">
</head>
<body>

<header class="app-header">
    <div class="header-container">
        <div class="brand">
            <h1>📋 Campus Task Planner</h1>
            <p class="subtitle"><?= $editing ? "Edit Existing Task Record" : "Create New Relational Task" ?> | 24BCS174</p>
        </div>
        <div>
            <a href="index.php" class="btn btn-clear">⬅ Back to Dashboard</a>
        </div>
    </div>
</header>

<main class="container">
    <div class="form-container">
        <h2><?= $editing ? "✏️ Edit Task Details" : "➕ Add New Task Record" ?></h2>
        <p class="form-lead">Enter task details below. All entries are validated on the server and persisted into relational tables.</p>

        <?php if (!empty($error)): ?>
            <div class="alert alert-danger">
                <strong>Validation Error:</strong> <?= htmlspecialchars($error) ?>
            </div>
        <?php endif; ?>

        <form method="POST" action="task_form.php<?= $editing ? '?id=' . $id : '' ?>" class="crud-form">
            <div class="form-group">
                <label for="task_title">Task Title <span class="required">*</span></label>
                <input type="text" id="task_title" name="task_title" value="<?= htmlspecialchars($task['task_title']) ?>" placeholder="e.g. Complete FOSS Assignment 3 Report" required>
            </div>

            <div class="form-group">
                <label for="description">Task Description / Objective</label>
                <textarea id="description" name="description" rows="3" placeholder="Provide additional task specifications or lab requirements..."><?= htmlspecialchars($task['description'] ?? '') ?></textarea>
            </div>

            <div class="form-row">
                <div class="form-group col-half">
                    <label for="category_id">Category (Relational Table: categories) <span class="required">*</span></label>
                    <select id="category_id" name="category_id" required>
                        <?php while ($cat = $categories->fetch_assoc()): ?>
                            <option value="<?= $cat['category_id'] ?>" <?= $task['category_id'] == $cat['category_id'] ? 'selected' : '' ?>>
                                <?= htmlspecialchars($cat['category_name']) ?> (<?= htmlspecialchars($cat['category_code']) ?>)
                            </option>
                        <?php endwhile; ?>
                    </select>
                </div>

                <div class="form-group col-half">
                    <label for="priority_id">Priority Level (Relational Table: priorities) <span class="required">*</span></label>
                    <select id="priority_id" name="priority_id" required>
                        <?php while ($p = $priorities->fetch_assoc()): ?>
                            <option value="<?= $p['priority_id'] ?>" <?= $task['priority_id'] == $p['priority_id'] ? 'selected' : '' ?>>
                                Level <?= $p['priority_level'] ?>: <?= htmlspecialchars($p['priority_label']) ?>
                            </option>
                        <?php endwhile; ?>
                    </select>
                </div>
            </div>

            <div class="form-row">
                <div class="form-group col-half">
                    <label for="due_date">Due Date</label>
                    <input type="date" id="due_date" name="due_date" value="<?= htmlspecialchars($task['due_date'] ?? '') ?>">
                </div>

                <div class="form-group col-half">
                    <label for="task_status">Task Status</label>
                    <select id="task_status" name="task_status">
                        <option value="Pending" <?= $task['task_status'] === 'Pending' ? 'selected' : '' ?>>Pending</option>
                        <option value="In Progress" <?= $task['task_status'] === 'In Progress' ? 'selected' : '' ?>>In Progress</option>
                        <option value="Completed" <?= $task['task_status'] === 'Completed' ? 'selected' : '' ?>>Completed</option>
                    </select>
                </div>
            </div>

            <div class="form-group">
                <label for="new_subtask">Checklist / Subtask (Relational Table: task_checklists)</label>
                <input type="text" id="new_subtask" name="new_subtask" placeholder="e.g. Verify MySQL foreign key constraints">
                <?php if (!empty($subtasks)): ?>
                    <div class="existing-subtasks">
                        <small><strong>Existing Checklists:</strong> <?= htmlspecialchars(implode(" • ", $subtasks)) ?></small>
                    </div>
                <?php endif; ?>
            </div>

            <div class="form-actions">
                <button type="submit" class="btn btn-save"><?= $editing ? "💾 Update Task Record" : "💾 Save Task" ?></button>
                <a href="index.php" class="btn btn-clear">Cancel</a>
            </div>
        </form>
    </div>
</main>

<footer class="app-footer">
    <p>Campus Task Planner | 19CSP01 Free Open Source Software | Ragul Ramachandran (24BCS174)</p>
</footer>

</body>
</html>
