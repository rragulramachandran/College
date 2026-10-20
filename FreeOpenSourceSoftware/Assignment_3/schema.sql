-- schema.sql: 4 Relational Tables for Campus Task Planner System
-- Developed for 19CSP01 - Free Open Source Software
-- Student: Ragul Ramachandran (24BCS174)

CREATE DATABASE IF NOT EXISTS foss_task_planner;
USE foss_task_planner;

-- Table 1: Categories Entity
CREATE TABLE IF NOT EXISTS categories (
    category_id     INT AUTO_INCREMENT PRIMARY KEY,
    category_code   VARCHAR(15)  NOT NULL UNIQUE,
    category_name   VARCHAR(60)  NOT NULL,
    description     VARCHAR(150)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- Table 2: Priorities Entity
CREATE TABLE IF NOT EXISTS priorities (
    priority_id     INT AUTO_INCREMENT PRIMARY KEY,
    priority_level  INT          NOT NULL UNIQUE,
    priority_label  VARCHAR(20)  NOT NULL,
    badge_color     VARCHAR(20)  NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- Table 3: Tasks Entity (Foreign Keys -> categories, priorities)
CREATE TABLE IF NOT EXISTS tasks (
    task_id         INT AUTO_INCREMENT PRIMARY KEY,
    task_title      VARCHAR(150) NOT NULL,
    description     TEXT,
    due_date        DATE         NULL,
    task_status     ENUM('Pending', 'In Progress', 'Completed') NOT NULL DEFAULT 'Pending',
    category_id     INT          NOT NULL,
    priority_id     INT          NOT NULL,
    created_at      TIMESTAMP    DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (category_id) REFERENCES categories(category_id) ON DELETE CASCADE,
    FOREIGN KEY (priority_id) REFERENCES priorities(priority_id) ON DELETE RESTRICT
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- Table 4: Task Checklists / Subtasks (Foreign Key -> tasks)
CREATE TABLE IF NOT EXISTS task_checklists (
    checklist_id    INT AUTO_INCREMENT PRIMARY KEY,
    task_id         INT          NOT NULL,
    item_title      VARCHAR(150) NOT NULL,
    is_completed    TINYINT(1)   DEFAULT 0,
    created_at      TIMESTAMP    DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (task_id) REFERENCES tasks(task_id) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- Seed Data Insertion
INSERT INTO categories (category_code, category_name, description) VALUES
('ACAD', 'Academic & Coursework', 'Assignments, lecture notes, and lab preparations'),
('PROJ', 'Mini Project & Capstone', 'Software engineering design, coding, and deliverables'),
('PERS', 'Personal & Skill Development', 'Open-source contributions, webinars, and reading'),
('EXAM', 'Examination Preparation', 'Internal assessments, semester review, and question banks');

INSERT INTO priorities (priority_level, priority_label, badge_color) VALUES
(1, 'High', '#dc3545'),
(2, 'Medium', '#fd7e14'),
(3, 'Low', '#20c997');

INSERT INTO tasks (task_title, description, due_date, task_status, category_id, priority_id) VALUES
('Complete FOSS Assignment 2 & 3', 'Develop Python package and full-stack PHP/MySQL CRUD application with report.', '2026-10-23', 'In Progress', 1, 1),
('Review Operating Systems IPC Lab', 'Study message queues and shared memory implementation for upcoming lab exam.', '2026-10-25', 'Pending', 1, 2),
('FOSS Open Source Contribution', 'Fork active repository, create descriptive feature pull request, and document diff.', '2026-10-28', 'Pending', 3, 2),
('Organise Study Materials for SEM V', 'Consolidate lecture presentations and reference textbooks into cloud drive.', '2026-10-21', 'Completed', 1, 3);

INSERT INTO task_checklists (task_id, item_title, is_completed) VALUES
(1, 'Author Python package with setup.py and test suite', 1),
(1, 'Implement 4-table relational MySQL schema and PHP backend', 1),
(1, 'Generate documentation and commit to GitHub', 0),
(2, 'Test two-way pipe implementation', 1),
(2, 'Verify message queue synchronization', 0),
(3, 'Clone target repository locally', 1),
(3, 'Submit pull request with passing tests', 0),
(4, 'Backup database dumps and configuration files', 1);
