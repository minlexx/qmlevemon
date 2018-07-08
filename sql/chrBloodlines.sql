CREATE TABLE "chrBloodlines" ( 
  "bloodlineID" INTEGER NOT NULL, 
  "bloodlineName" VARCHAR(100), 
  "raceID" INTEGER, 
  "shipTypeID" INTEGER, 
  "corporationID" INTEGER, 
  "perception" INTEGER, 
  "willpower" INTEGER, 
  "charisma" INTEGER, 
  "memory" INTEGER, 
  "intelligence" INTEGER, 
  "iconID" INTEGER, 
  PRIMARY KEY ("bloodlineID") 
);
INSERT INTO chrBloodlines VALUES(1, 'Deteis', 1, 601, 1000006, 5, 5, 6, 7, 7, 1633);
INSERT INTO chrBloodlines VALUES(2, 'Civire', 1, 601, 1000009, 9, 6, 6, 4, 5, 1631);
INSERT INTO chrBloodlines VALUES(3, 'Sebiestor', 2, 588, 1000046, 5, 6, 6, 6, 7, 1634);
INSERT INTO chrBloodlines VALUES(4, 'Brutor', 2, 588, 1000049, 9, 7, 6, 4, 4, 1635);
INSERT INTO chrBloodlines VALUES(5, 'Amarr', 4, 596, 1000066, 4, 10, 3, 6, 7, 1628);
INSERT INTO chrBloodlines VALUES(6, 'Ni-Kunni', 4, 596, 1000072, 7, 4, 8, 6, 5, 1632);
INSERT INTO chrBloodlines VALUES(7, 'Gallente', 8, 606, 1000107, 8, 4, 8, 4, 6, 1629);
INSERT INTO chrBloodlines VALUES(8, 'Intaki', 8, 606, 1000111, 3, 6, 6, 7, 8, 1630);
INSERT INTO chrBloodlines VALUES(9, 'Static', 16, 670, 1000177, 7, 8, 5, 10, 10, 0);
INSERT INTO chrBloodlines VALUES(10, 'Modifier', 16, 670, 1000178, 10, 9, 7, 7, 7, 0);
INSERT INTO chrBloodlines VALUES(11, 'Achura', 1, 601, 1000014, 7, 6, 3, 6, 8, 3022);
INSERT INTO chrBloodlines VALUES(12, 'Jin-Mei', 8, 606, 1000114, 6, 7, 7, 5, 5, 3024);
INSERT INTO chrBloodlines VALUES(13, 'Khanid', 4, 596, 1000080, 8, 8, 5, 4, 5, 3023);
INSERT INTO chrBloodlines VALUES(14, 'Vherokior', 2, 588, 1000060, 4, 3, 8, 8, 7, 3021);
INSERT INTO chrBloodlines VALUES(19, 'Drifter', 16, 24690, 1000274, 0, 0, 0, 0, 0, 21404);
INSERT INTO chrBloodlines VALUES(25, 'Narodnya', 135, 47269, 1000293, 9, 9, 9, 9, 9, NULL);
INSERT INTO chrBloodlines VALUES(26, 'Koschoi', 135, 47269, 1000293, 9, 9, 9, 9, 9, NULL);
INSERT INTO chrBloodlines VALUES(27, 'Navka', 135, 47269, 1000293, 9, 9, 9, 9, 9, NULL);
