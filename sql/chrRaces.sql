CREATE TABLE "chrRaces" ( 
  "raceID" INTEGER NOT NULL, 
  "raceName" VARCHAR(100), 
  "iconID" INTEGER
  PRIMARY KEY ("raceID") 
);
INSERT INTO chrRaces VALUES(1, 'Caldari', 1439);
INSERT INTO chrRaces VALUES(2, 'Minmatar', 1440);
INSERT INTO chrRaces VALUES(4, 'Amarr', 1442);
INSERT INTO chrRaces VALUES(8, 'Gallente', 1441);
INSERT INTO chrRaces VALUES(16, 'Jove', NULL);
INSERT INTO chrRaces VALUES(32, 'Pirate', NULL);
INSERT INTO chrRaces VALUES(64, 'Sleepers', NULL);
INSERT INTO chrRaces VALUES(128, 'ORE', 0);
INSERT INTO chrRaces VALUES(134, 'Rogue Drones', NULL);
INSERT INTO chrRaces VALUES(135, 'Triglavian', NULL);
