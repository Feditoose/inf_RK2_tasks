-- Таблица студентов
CREATE TABLE students (
    student_id SERIAL PRIMARY KEY,  
    full_name VARCHAR(100) NOT NULL,  
    group_number VARCHAR(10) NOT NULL  
);

-- Таблица предметов
CREATE TABLE subjects (
    subject_id SERIAL PRIMARY KEY, 
    subject_name VARCHAR(100) UNIQUE NOT NULL 
);

-- Таблица оценок
CREATE TABLE grades (
    grade_id SERIAL PRIMARY KEY, 
    student_id INT REFERENCES students(student_id), 
    subject_id INT REFERENCES subjects(subject_id),  
    grade INT CHECK (grade BETWEEN 2 AND 5) 
);

-- Таблица посещаемости
CREATE TABLE attendance (
    attendance_id SERIAL PRIMARY KEY, 
    student_id INT REFERENCES students(student_id), 
    date_attended DATE NOT NULL,  
    status VARCHAR(20) CHECK (status IN ('присутствовал', 'отсутствовал'))
);

-- Таблица заметок преподавателей
CREATE TABLE notes (
    note_id SERIAL PRIMARY KEY, 
    student_id INT REFERENCES students(student_id),  
    note_text TEXT NOT NULL  
);

-- Индекс для быстрого поиска одногруппников
CREATE INDEX idx_students_group ON students(group_number);

-- Индекс для ускорения агрегированных запросов по оценкам
CREATE INDEX idx_grades_student ON grades(student_id);

-- GIN-индекс для полнотекстового поиска в заметках
CREATE INDEX idx_notes_text ON notes USING GIN(to_tsvector('russian', note_text));

CREATE VIEW student_avg_grades AS
SELECT s.student_id, s.full_name, ROUND(AVG(g.grade), 2) AS avg_grade
FROM students s
JOIN grades g ON s.student_id = g.student_id
GROUP BY s.student_id;

-- Добавляем 6 студентов одной группы
INSERT INTO students (full_name, group_number) VALUES
('Литичевский Федор', 'ИУ1-12Б'),
('Перцовский Петр', 'ИУ1-12Б'),
('Любимова Алиса', 'ИУ1-12Б'),
('Шевцов Федор', 'ИУ1-12Б'),
('Казаченок Артем', 'ИУ1-12Б'),
('Анисимов Арсений', 'ИУ1-12Б');

-- Добавляем 3 предмета
INSERT INTO subjects (subject_name) VALUES
('Математический анализ'),
('Аналитическая геометрия'),
('Информатика');

-- Добавляем оценки для всех студентов по каждому предмету
INSERT INTO grades (student_id, subject_id, grade) VALUES
(1, 1, 5), (1, 2, 4), (1, 3, 5),
(2, 1, 4), (2, 2, 3), (2, 3, 4),
(3, 1, 5), (3, 2, 5), (3, 3, 5),
(4, 1, 3), (4, 2, 4), (4, 3, 3),
(5, 1, 4), (5, 2, 4), (5, 3, 5),
(6, 1, 3), (6, 2, 3), (6, 3, 4);

-- Добавляем посещаемость на два дня
INSERT INTO attendance (student_id, date_attended, status) VALUES
(1, '2024-03-01', 'присутствовал'), (1, '2024-03-02', 'присутствовал'),
(2, '2024-03-01', 'присутствовал'), (2, '2024-03-02', 'отсутствовал'),
(3, '2024-03-01', 'присутствовал'), (3, '2024-03-02', 'присутствовал'),
(4, '2024-03-01', 'отсутствовал'), (4, '2024-03-02', 'присутствовал'),
(5, '2024-03-01', 'присутствовал'), (5, '2024-03-02', 'присутствовал'),
(6, '2024-03-01', 'присутствовал'), (6, '2024-03-02', 'отсутствовал');

-- Добавляем заметки по каждому студенту
INSERT INTO notes (student_id, note_text) VALUES
(1, 'Любит информатику'),
(2, 'Нужна помощь по информатике'),
(3, 'Редко посещает занятия по информатике'),
(4, 'Отличник по всем предметам'),
(5, 'Хорошо работает в команде'),
(6, 'Прогресс по информатике');

BEGIN;  -- Начало транзакции

INSERT INTO students (full_name, group_number) VALUES ('Рожков Стефан', 'ИУ1-12Б') RETURNING student_id;

-- Предположим, что новый студент получил id = 7
INSERT INTO grades (student_id, subject_id, grade) VALUES
(7, 1, 4), (7, 2, 5), (7, 3, 4);

COMMIT;  -- Фиксация транзакции

-- 1. Найти 5 одногруппников студента (2 до и 3 после по student_id)
WITH target AS (SELECT student_id FROM students WHERE full_name = 'Любимова Алиса')
SELECT * FROM students 
WHERE group_number = (SELECT group_number FROM students WHERE student_id = (SELECT student_id FROM target))
AND student_id BETWEEN (SELECT student_id FROM target) - 2 AND (SELECT student_id FROM target) + 3
ORDER BY student_id;

-- 2. Посмотреть средний балл конкретного студента через представление
SELECT * FROM student_avg_grades WHERE full_name = 'Литичевский Федор';

-- 3. Агрегировать средний балл по предмету «Информатика»
SELECT ROUND(AVG(grade), 2) AS avg_informatics 
FROM grades 
WHERE subject_id = (SELECT subject_id FROM subjects WHERE subject_name = 'Информатика');

-- 4. Полнотекстовый поиск по заметкам (слово «Информатика»)
SELECT * FROM notes 
WHERE to_tsvector('russian', note_text) @@ to_tsquery('russian', 'Информатика');

-- 5. Обновить посещаемость студента на конкретную дату через транзакцию
BEGIN;
UPDATE attendance 
SET status = 'отсутствовал' 
WHERE student_id = (SELECT student_id FROM students WHERE full_name = 'Литичевский Федор') 
AND date_attended = '2024-03-01';
COMMIT;
