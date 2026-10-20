CREATE DATABASE IF NOT EXISTS foss_task_planner;
USE foss_task_planner;

CREATE TABLE categories (
    category_id INT AUTO_INCREMENT PRIMARY KEY,
    category_name VARCHAR(50) NOT NULL UNIQUE
);

CREATE TABLE tasks (
    task_id INT AUTO_INCREMENT PRIMARY KEY,
    task_title VARCHAR(120) NOT NULL,
    due_date DATE NULL,
    task_status ENUM('Pending', 'Completed') NOT NULL DEFAULT 'Pending',
    category_id INT NOT NULL,
    FOREIGN KEY (category_id) REFERENCES categories(category_id)
);

INSERT INTO categories (category_name) VALUES
('Academic'), ('Personal'), ('Project');

INSERT INTO tasks (task_title, due_date, task_status, category_id) VALUES
('Complete FOSS assignment', '2026-10-23', 'Pending', 1),
('Prepare mini project notes', '2026-10-25', 'Pending', 3),
('Organise study materials', '2026-10-21', 'Completed', 2);
