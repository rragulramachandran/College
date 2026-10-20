<?php
/**
 * index.php - Main Dashboard (Read Operation & Statistics)
 * Student: Ragul Ramachandran (24BCS174)
 * Course: 19CSP01 - Free Open Source Software
 */
require_once "db.php";

// Handle quick status toggle
if (isset($_GET["toggle"]) && (int)$_GET["toggle"] > 0) {
    $tid = (int)$_GET["toggle"];
    $stmt = $conn->prepare("UPDATE tasks SET task_status = IF(task_status = 'Completed', 'Pending', 'Completed') WHERE task_id = ?");
    $stmt->bind_param("i", $tid);
    $stmt->execute();
    header("Location: index.php?msg=toggled");
    exit;
}

// Fetch filter parameters
$filter_category = isset($_GET["cat"]) ? (int)$_GET["cat"] : 0;
$filter_status   = isset($_GET["status"]) ? trim($_GET["status"]) : "";
$search_query    = isset($_GET["q"]) ? trim($_GET["q"]) : "";

// Build relational query joining 4 tables
$sql = "SELECT 
            t.task_id,
            t.task_title,
            t.description,
            t.due_date,
            t.task_status,
            t.created_at,
            c.category_name,
            c.category_code,
            p.priority_label,
            p.badge_color,
            GROUP_CONCAT(tc.item_title SEPARATOR ' • ') AS checklist_summary,
            COUNT(tc.checklist_id) AS total_subtasks,
            SUM(IF(tc.is_completed = 1, 1, 0)) AS completed_subtasks
        FROM tasks t
        JOIN categories c ON t.category_id = c.category_id
        JOIN priorities p ON t.priority_id = p.priority_id
        LEFT JOIN task_checklists tc ON t.task_id = tc.task_id
        WHERE 1=1 ";

$params = [];
$types  = "";

if ($filter_category > 0) {
    $sql .= "AND t.category_id = ? ";
    $params[] = $filter_category;
    $types   .= "i";
}
if (!empty($filter_status)) {
    $sql .= "AND t.task_status = ? ";
    $params[] = $filter_status;
    $types   .= "s";
}
if (!empty($search_query)) {
    $sql .= "AND (t.task_title LIKE ? OR t.description LIKE ?) ";
    $like = "%" . $search_query . "%";
    $params[] = $like;
    $params[] = $like;
    $types   .= "ss";
}

$sql .= "GROUP BY t.task_id ORDER BY t.due_date ASC, p.priority_level ASC";

$stmt = $conn->prepare($sql);
if (!empty($params)) {
    $stmt->bind_param($types, ...$params);
}
$stmt->execute();
$tasks_result = $stmt->get_result();

// Overall KPI Statistics
$kpi_total     = $conn->query("SELECT COUNT(*) AS c FROM tasks")->fetch_assoc()["c"];
$kpi_pending   = $conn->query("SELECT COUNT(*) AS c FROM tasks WHERE task_status = 'Pending'")->fetch_assoc()["c"];
$kpi_progress  = $conn->query("SELECT COUNT(*) AS c FROM tasks WHERE task_status = 'In Progress'")->fetch_assoc()["c"];
$kpi_completed = $conn->query("SELECT COUNT(*) AS c FROM tasks WHERE task_status = 'Completed'")->fetch_assoc()["c"];
$kpi_high      = $conn->query("SELECT COUNT(*) AS c FROM tasks t JOIN priorities p ON t.priority_id = p.priority_id WHERE p.priority_label = 'High'")->fetch_assoc()["c"];

// Categories for filter dropdown
$categories_res = $conn->query("SELECT * FROM categories ORDER BY category_name ASC");

// Feedback message handler
$feedback = "";
if (isset($_GET["msg"])) {
    $m = $_GET["msg"];
    if ($m === "created") {
        $feedback = "<div class='alert alert-success'><strong>Success!</strong> Task record created successfully with associated category and priority.</div>";
    } elseif ($m === "updated") {
        $feedback = "<div class='alert alert-info'><strong>Updated!</strong> Task details and checklist items updated successfully.</div>";
    } elseif ($m === "deleted") {
        $feedback = "<div class='alert alert-danger'><strong>Deleted!</strong> Task record and associated checklist removed from database.</div>";
    } elseif ($m === "toggled") {
        $feedback = "<div class='alert alert-success'><strong>Status Changed!</strong> Task status was toggled successfully.</div>";
    }
}
?>
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Campus Task Planner - FOSS WebApp (24BCS174)</title>
    <link rel="stylesheet" href="style.css">
</head>
<body>

<header class="app-header">
    <div class="header-container">
        <div class="brand">
            <h1>📋 Campus Task Planner</h1>
            <p class="subtitle">19CSP01 – Free Open Source Software | Relational CRUD WebApp | Register No: 24BCS174</p>
        </div>
        <div class="header-badge">
            <span class="badge badge-tech">MySQL 8.x + PHP 8.2</span>
            <span class="badge badge-rel">4 Relational Tables</span>
        </div>
    </div>
</header>

<main class="container">
    <?= $feedback ?>

    <!-- KPI Metric Cards -->
    <section class="kpi-grid">
        <div class="kpi-card card-blue">
            <div class="kpi-val"><?= $kpi_total ?></div>
            <div class="kpi-lbl">Total Tasks</div>
        </div>
        <div class="kpi-card card-orange">
            <div class="kpi-val"><?= $kpi_pending ?></div>
            <div class="kpi-lbl">Pending</div>
        </div>
        <div class="kpi-card card-cyan">
            <div class="kpi-val"><?= $kpi_progress ?></div>
            <div class="kpi-lbl">In Progress</div>
        </div>
        <div class="kpi-card card-green">
            <div class="kpi-val"><?= $kpi_completed ?></div>
            <div class="kpi-lbl">Completed</div>
        </div>
        <div class="kpi-card card-red">
            <div class="kpi-val"><?= $kpi_high ?></div>
            <div class="kpi-lbl">High Priority</div>
        </div>
    </section>

    <!-- Toolbar: Search, Filter, and Add New Task -->
    <div class="toolbar-card">
        <form method="GET" action="index.php" class="filter-form">
            <input type="text" name="q" placeholder="Search tasks..." value="<?= htmlspecialchars($search_query) ?>" class="input-search">
            
            <select name="cat" class="input-select">
                <option value="0">All Categories</option>
                <?php while ($cat = $categories_res->fetch_assoc()): ?>
                    <option value="<?= $cat['category_id'] ?>" <?= $filter_category == $cat['category_id'] ? 'selected' : '' ?>>
                        <?= htmlspecialchars($cat['category_name']) ?>
                    </option>
                <?php endwhile; ?>
            </select>

            <select name="status" class="input-select">
                <option value="">All Statuses</option>
                <option value="Pending" <?= $filter_status === 'Pending' ? 'selected' : '' ?>>Pending</option>
                <option value="In Progress" <?= $filter_status === 'In Progress' ? 'selected' : '' ?>>In Progress</option>
                <option value="Completed" <?= $filter_status === 'Completed' ? 'selected' : '' ?>>Completed</option>
            </select>

            <button type="submit" class="btn btn-filter">Filter</button>
            <?php if ($filter_category > 0 || !empty($filter_status) || !empty($search_query)): ?>
                <a href="index.php" class="btn btn-clear">Reset</a>
            <?php endif; ?>
        </form>

        <a href="task_form.php" class="btn btn-add">➕ Add New Task</a>
    </div>

    <!-- Tasks Data Table -->
    <div class="table-card">
        <table class="data-table">
            <thead>
                <tr>
                    <th style="width: 5%;">#</th>
                    <th style="width: 25%;">Task Title & Description</th>
                    <th style="width: 15%;">Category</th>
                    <th style="width: 10%;">Priority</th>
                    <th style="width: 12%;">Due Date</th>
                    <th style="width: 11%;">Status</th>
                    <th style="width: 12%;">Subtasks</th>
                    <th style="width: 10%; text-align: center;">Actions</th>
                </tr>
            </thead>
            <tbody>
                <?php if ($tasks_result->num_rows === 0): ?>
                    <tr>
                        <td colspan="8" class="text-center empty-state">No matching tasks found. Click "Add New Task" to create one.</td>
                    </tr>
                <?php else: ?>
                    <?php while ($row = $tasks_result->fetch_assoc()): ?>
                        <tr>
                            <td><span class="task-id-badge"><?= $row["task_id"] ?></span></td>
                            <td>
                                <strong><?= htmlspecialchars($row["task_title"]) ?></strong>
                                <?php if (!empty($row["description"])): ?>
                                    <div class="task-desc"><?= htmlspecialchars($row["description"]) ?></div>
                                <?php endif; ?>
                            </td>
                            <td>
                                <span class="badge badge-cat"><?= htmlspecialchars($row["category_name"]) ?></span>
                            </td>
                            <td>
                                <span class="badge" style="background-color: <?= $row['badge_color'] ?>; color: #fff;">
                                    <?= htmlspecialchars($row["priority_label"]) ?>
                                </span>
                            </td>
                            <td><?= $row["due_date"] ? htmlspecialchars($row["due_date"]) : "<span class='text-muted'>None</span>" ?></td>
                            <td>
                                <a href="index.php?toggle=<?= $row['task_id'] ?>" title="Click to toggle status" class="status-pill status-<?= strtolower(str_replace(' ', '-', $row['task_status'])) ?>">
                                    <?= htmlspecialchars($row["task_status"]) ?>
                                </a>
                            </td>
                            <td>
                                <?php if ($row["total_subtasks"] > 0): ?>
                                    <div class="subtask-progress">
                                        <span class="subtask-ratio"><?= $row["completed_subtasks"] ?>/<?= $row["total_subtasks"] ?> Done</span>
                                    </div>
                                    <small class="checklist-text" title="<?= htmlspecialchars($row['checklist_summary']) ?>">
                                        <?= htmlspecialchars(mb_strimwidth($row["checklist_summary"], 0, 32, "...")) ?>
                                    </small>
                                <?php else: ?>
                                    <span class="text-muted">-</span>
                                <?php endif; ?>
                            </td>
                            <td class="action-cell">
                                <a href="task_form.php?id=<?= $row['task_id'] ?>" class="btn-action btn-edit" title="Edit Task">Edit</a>
                                <a href="delete_task.php?id=<?= $row['task_id'] ?>" class="btn-action btn-delete" onclick="return confirm('Are you sure you want to permanently delete this task?');" title="Delete Task">Delete</a>
                            </td>
                        </tr>
                    <?php endwhile; ?>
                <?php endif; ?>
            </tbody>
        </table>
    </div>
</main>

<footer class="app-footer">
    <p>Campus Task Planner Web Application | Built with PHP, MySQL & CSS3 | 19CSP01 Assignment 3 | Ragul Ramachandran (24BCS174)</p>
</footer>

</body>
</html>
