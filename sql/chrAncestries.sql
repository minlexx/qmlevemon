CREATE TABLE "chrAncestries" ( 
  "ancestryID" INTEGER NOT NULL, 
  "ancestryName" VARCHAR(100), 
  "bloodlineID" INTEGER, 
  "perception" INTEGER, 
  "willpower" INTEGER, 
  "charisma" INTEGER, 
  "memory" INTEGER, 
  "intelligence" INTEGER, 
  "iconID" INTEGER
  PRIMARY KEY ("ancestryID") 
);
INSERT INTO chrAncestries VALUES(1, 'Liberal Holders', 5, 0, 1, 3, 0, 0, 1641);
INSERT INTO chrAncestries VALUES(2, 'Wealthy Commoners', 5, 0, 0, 1, 3, 0, 1642);
INSERT INTO chrAncestries VALUES(3, 'Religious Reclaimers', 5, 0, 4, 0, 0, 0, 1643);
INSERT INTO chrAncestries VALUES(4, 'Free Merchants', 6, 0, 0, 4, 0, 0, 1644);
INSERT INTO chrAncestries VALUES(5, 'Border Runners', 6, 3, 0, 0, 0, 1, 1645);
INSERT INTO chrAncestries VALUES(6, 'Navy Veterans', 6, 0, 4, 0, 0, 0, 1646);
INSERT INTO chrAncestries VALUES(7, 'Entrepreneurs', 2, 0, 0, 0, 4, 0, 1647);
INSERT INTO chrAncestries VALUES(8, 'Mercs', 2, 0, 4, 0, 0, 0, 1648);
INSERT INTO chrAncestries VALUES(9, 'Dissenters', 2, 0, 2, 2, 0, 0, 1649);
INSERT INTO chrAncestries VALUES(10, 'Merchandisers', 1, 0, 0, 0, 4, 0, 1650);
INSERT INTO chrAncestries VALUES(11, 'Scientists', 1, 1, 0, 0, 0, 3, 1651);
INSERT INTO chrAncestries VALUES(12, 'Tube Child', 1, 0, 4, 0, 0, 0, 1652);
INSERT INTO chrAncestries VALUES(13, 'Activists', 7, 0, 0, 4, 0, 0, 1653);
INSERT INTO chrAncestries VALUES(14, 'Miners', 7, 0, 0, 0, 4, 0, 1654);
INSERT INTO chrAncestries VALUES(15, 'Immigrants', 7, 2, 2, 0, 0, 0, 1655);
INSERT INTO chrAncestries VALUES(16, 'Artists', 8, 2, 0, 2, 0, 0, 1656);
INSERT INTO chrAncestries VALUES(17, 'Diplomats', 8, 0, 0, 4, 0, 0, 1657);
INSERT INTO chrAncestries VALUES(18, 'Reborn', 8, 0, 0, 0, 4, 0, 1658);
INSERT INTO chrAncestries VALUES(19, 'Tinkerers', 3, 0, 0, 0, 0, 4, 1659);
INSERT INTO chrAncestries VALUES(20, 'Traders', 3, 0, 0, 4, 0, 0, 1660);
INSERT INTO chrAncestries VALUES(21, 'Rebels', 3, 3, 1, 0, 0, 0, 1661);
INSERT INTO chrAncestries VALUES(22, 'Workers', 4, 0, 0, 0, 4, 0, 1662);
INSERT INTO chrAncestries VALUES(23, 'Tribal Traditionalists', 4, 0, 3, 1, 0, 0, 1663);
INSERT INTO chrAncestries VALUES(24, 'Slave Child', 4, 2, 2, 0, 0, 0, 1664);
INSERT INTO chrAncestries VALUES(25, 'Elders', 9, 0, 3, 0, 1, 0, NULL);
INSERT INTO chrAncestries VALUES(26, 'Unsullied', 9, 0, 0, 4, 0, 0, NULL);
INSERT INTO chrAncestries VALUES(27, 'Stasis People', 9, 4, 0, 0, 0, 0, NULL);
INSERT INTO chrAncestries VALUES(28, 'Existentialists', 10, 1, 0, 0, 0, 3, NULL);
INSERT INTO chrAncestries VALUES(29, 'Puritan', 10, 0, 0, 0, 4, 0, NULL);
INSERT INTO chrAncestries VALUES(30, 'Lab Rat', 10, 0, 4, 0, 0, 0, NULL);
INSERT INTO chrAncestries VALUES(31, 'Inventors', 11, 0, 0, 0, 0, 4, 3025);
INSERT INTO chrAncestries VALUES(32, 'Monks', 11, 2, 2, 0, 0, 0, 3027);
INSERT INTO chrAncestries VALUES(33, 'Stargazers', 11, 1, 0, 0, 3, 0, 3026);
INSERT INTO chrAncestries VALUES(34, 'Sang Do Caste', 12, 0, 3, 1, 0, 0, 3028);
INSERT INTO chrAncestries VALUES(35, 'Saan Go Caste', 12, 0, 0, 0, 2, 2, 3029);
INSERT INTO chrAncestries VALUES(36, 'Jing Ko Caste', 12, 4, 0, 0, 0, 0, 3030);
INSERT INTO chrAncestries VALUES(37, 'Cyber Knights', 13, 3, 0, 0, 0, 1, 3031);
INSERT INTO chrAncestries VALUES(38, 'Unionists', 13, 0, 0, 2, 0, 2, 3032);
INSERT INTO chrAncestries VALUES(39, 'Zealots', 13, 0, 2, 0, 2, 0, 3033);
INSERT INTO chrAncestries VALUES(40, 'Drifters', 14, 2, 0, 0, 0, 2, 3036);
INSERT INTO chrAncestries VALUES(41, 'Mystics', 14, 0, 3, 0, 0, 1, 3034);
INSERT INTO chrAncestries VALUES(42, 'Retailers', 14, 0, 0, 1, 3, 0, 3035);
