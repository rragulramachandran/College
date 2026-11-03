# Assignment 3: Campus Task Planner Web Application

**Course:** 19CSP01 – Free Open Source Software  
**Institution:** MEPCO SCHLENK ENGINEERING COLLEGE (Autonomous), Sivakasi  
---

## Project Overview

Campus Task Planner is a database-driven web application developed in PHP and MySQL as part of Assignment 3. The system is designed with a **4-table relational architecture** connecting categories, priorities, tasks, and task checklists. It supports complete **Create, Read, Update, and Delete (CRUD)** operations, status toggling, search & filtering, live KPI counters, and server-side validation.

## 4-Table Relational Schema

1. **`categories`** (Entity Table): Departmental/academic task categorizations (`category_id`, `category_code`, `category_name`, `description`).
2. **`priorities`** (Entity Table): Task urgency levels with color coding (`priority_id`, `priority_level`, `priority_label`, `badge_color`).
3. **`tasks`** (Core Entity Table): Task details linked via Foreign Keys (`task_id`, `task_title`, `description`, `due_date`, `task_status`, `category_id`, `priority_id`, `created_at`).
4. **`task_checklists`** (Associative Table): Subtasks / checklist items linked to parent tasks (`checklist_id`, `task_id`, `item_title`, `is_completed`, `created_at`).

## Features

- **Relational JOIN Queries**: Cross-table queries connecting all four tables with `GROUP_CONCAT` of subtasks.
- **KPI Metrics Dashboard**: Live counters for total tasks, pending, in-progress, completed, and high priority tasks.
- **Dynamic Search & Filters**: Filter by category, status, or keyword.
- **Status Toggling**: Direct one-click toggle of task completion status.
- **Strict Input Validation**: Server-side checks preventing empty titles and validating relational keys.
- **Prepared Statements**: Protection against SQL Injection using `mysqli` prepared statements.
- **Responsive Feedback Alerts**: Visual notification banners confirming create, update, and delete actions.

## Installation & Deployment

1. Start Apache HTTP Server and MySQL / MariaDB via XAMPP.
2. Open phpMyAdmin (`http://localhost/phpmyadmin`) and import `schema.sql`.
3. Copy the `Assignment_3_TaskPlanner` folder into the XAMPP `htdocs` directory.
4. Access the application in any browser:
   ```text
   http://localhost/Assignment_3_TaskPlanner/index.php
   ```

---
Hosted on GitHub: [rragulramachandran/College](https://github.com/rragulramachandran/College)
