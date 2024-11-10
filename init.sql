CREATE TABLE IF NOT EXISTS users (
    userId UUID PRIMARY KEY,
    username VARCHAR(50) NOT NULL,
    password VARCHAR(64) NOT NULL,
    isAdmin BOOLEAN NOT NULL
);