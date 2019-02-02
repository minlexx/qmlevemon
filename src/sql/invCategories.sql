CREATE TABLE "invCategories" ( 
  "categoryID" INTEGER NOT NULL, 
  "categoryName" VARCHAR(100), 
  "iconID" INTEGER, 
  PRIMARY KEY ("categoryID") 
);
INSERT INTO invCategories VALUES(0, '#System', NULL);
INSERT INTO invCategories VALUES(1, 'Owner', NULL);
INSERT INTO invCategories VALUES(2, 'Celestial', NULL);
INSERT INTO invCategories VALUES(3, 'Station', NULL);
INSERT INTO invCategories VALUES(4, 'Material', 22);
INSERT INTO invCategories VALUES(5, 'Accessories', 33);
INSERT INTO invCategories VALUES(6, 'Ship', NULL);
INSERT INTO invCategories VALUES(7, 'Module', 67);
INSERT INTO invCategories VALUES(8, 'Charge', NULL);
INSERT INTO invCategories VALUES(9, 'Blueprint', 21);
INSERT INTO invCategories VALUES(10, 'Trading', NULL);
INSERT INTO invCategories VALUES(11, 'Entity', NULL);
INSERT INTO invCategories VALUES(14, 'Bonus', 0);
INSERT INTO invCategories VALUES(16, 'Skill', 33);
INSERT INTO invCategories VALUES(17, 'Commodity', 0);
INSERT INTO invCategories VALUES(18, 'Drone', 0);
INSERT INTO invCategories VALUES(20, 'Implant', 0);
INSERT INTO invCategories VALUES(22, 'Deployable', 0);
INSERT INTO invCategories VALUES(23, 'Starbase', 0);
INSERT INTO invCategories VALUES(24, 'Reaction', 0);
INSERT INTO invCategories VALUES(25, 'Asteroid', NULL);
INSERT INTO invCategories VALUES(26, 'WorldSpace', NULL);
INSERT INTO invCategories VALUES(29, 'Abstract', NULL);
INSERT INTO invCategories VALUES(30, 'Apparel', NULL);
INSERT INTO invCategories VALUES(32, 'Subsystem', NULL);
INSERT INTO invCategories VALUES(34, 'Ancient Relics', NULL);
INSERT INTO invCategories VALUES(35, 'Decryptors', NULL);
INSERT INTO invCategories VALUES(39, 'Infrastructure Upgrades', NULL);
INSERT INTO invCategories VALUES(40, 'Sovereignty Structures', NULL);
INSERT INTO invCategories VALUES(41, 'Planetary Interaction', NULL);
INSERT INTO invCategories VALUES(42, 'Planetary Resources', NULL);
INSERT INTO invCategories VALUES(43, 'Planetary Commodities', NULL);
INSERT INTO invCategories VALUES(46, 'Orbitals', NULL);
INSERT INTO invCategories VALUES(49, 'Placeables', NULL);
INSERT INTO invCategories VALUES(53, 'Effects', NULL);
INSERT INTO invCategories VALUES(54, 'Lights', NULL);
INSERT INTO invCategories VALUES(59, 'Cells', NULL);
INSERT INTO invCategories VALUES(63, 'Special Edition Assets', NULL);
INSERT INTO invCategories VALUES(65, 'Structure', NULL);
INSERT INTO invCategories VALUES(66, 'Structure Module', NULL);
INSERT INTO invCategories VALUES(87, 'Fighter', NULL);
INSERT INTO invCategories VALUES(91, 'SKINs', NULL);
INSERT INTO invCategories VALUES(350001, 'Infantry', NULL);
