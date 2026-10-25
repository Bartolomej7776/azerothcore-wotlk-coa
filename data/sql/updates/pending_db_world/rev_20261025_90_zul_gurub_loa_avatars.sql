-- Zul'Gurub: the five Loa Avatars the High Priests and Hakkar drain (Drain Power) for their Aspects.
-- From the client creature cache (captured 2026-09-04): name, subname, model, rank 1, type 1, type_flags 72,
-- HealthModifier 10000, movementId 999. Not in any data: level (63, the priests' level) and faction
-- (35; in the CoA combat log of 2026-08-02 players buff the avatars, so they are not hostile to the raid).
INSERT INTO `creature_template` (`entry`, `name`, `subname`, `minlevel`, `maxlevel`, `faction`, `rank`,
    `BaseAttackTime`, `RangeAttackTime`, `unit_class`, `type`, `type_flags`, `HealthModifier`, `movementId`,
    `VerifiedBuild`) VALUES
(666988, 'Avatar of Bethekk', 'Panther Loa', 63, 63, 35, 1, 2000, 2000, 1, 1, 72, 10000, 999, 0),
(666989, 'Avatar of Shirvallah', 'Tiger Loa', 63, 63, 35, 1, 2000, 2000, 1, 1, 72, 10000, 999, 0),
(666990, 'Avatar of Shadra', 'Spider Loa', 63, 63, 35, 1, 2000, 2000, 1, 1, 72, 10000, 999, 0),
(666991, 'Avatar of Hethiss', 'Cobra Loa', 63, 63, 35, 1, 2000, 2000, 1, 1, 72, 10000, 999, 0),
(666992, 'Avatar of Hir\'eek', 'Bat Loa', 63, 63, 35, 1, 2000, 2000, 1, 1, 72, 10000, 999, 0)
ON DUPLICATE KEY UPDATE `name` = VALUES(`name`), `subname` = VALUES(`subname`), `minlevel` = VALUES(`minlevel`),
    `maxlevel` = VALUES(`maxlevel`), `faction` = VALUES(`faction`), `rank` = VALUES(`rank`),
    `BaseAttackTime` = VALUES(`BaseAttackTime`), `RangeAttackTime` = VALUES(`RangeAttackTime`),
    `unit_class` = VALUES(`unit_class`), `type` = VALUES(`type`), `type_flags` = VALUES(`type_flags`),
    `HealthModifier` = VALUES(`HealthModifier`), `movementId` = VALUES(`movementId`);

DELETE FROM `creature_template_model` WHERE `CreatureID` BETWEEN 666988 AND 666992;
INSERT INTO `creature_template_model` (`CreatureID`, `Idx`, `CreatureDisplayID`, `DisplayScale`, `Probability`,
    `VerifiedBuild`) VALUES
(666988, 0, 633, 1, 1, 0),
(666989, 0, 11031, 1, 1, 0),
(666990, 0, 963, 1, 1, 0),
(666991, 0, 14559, 1, 1, 0),
(666992, 0, 14562, 1, 1, 0);
