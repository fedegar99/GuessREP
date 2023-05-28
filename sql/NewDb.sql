CREATE TABLE utente(
	username varchar(32) NOT NULL PRIMARY KEY,
	passw varchar(16) NOT NULL,
	email varchar(64) NOT NULL,
	partiteVinte integer DEFAULT 0,
	imgId integer DEFAULT 0
);
