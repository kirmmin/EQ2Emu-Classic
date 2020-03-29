/*
SQLyog Ultimate v9.20 
MySQL - 5.1.54-1ubuntu4 : Database - eq2_updates
*********************************************************************
*/


/*!40101 SET NAMES utf8 */;

/*!40101 SET SQL_MODE=''*/;

/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;
/*Table structure for table `opcodes` */

DROP TABLE IF EXISTS `opcodes`;

CREATE TABLE `opcodes` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `version_range1` smallint(5) unsigned NOT NULL DEFAULT '0',
  `version_range2` smallint(5) unsigned NOT NULL DEFAULT '0',
  `name` varchar(255) NOT NULL DEFAULT '',
  `opcode` smallint(5) unsigned NOT NULL DEFAULT '0',
  `table_data_version` smallint(5) unsigned NOT NULL DEFAULT '1',
  PRIMARY KEY (`id`),
  UNIQUE KEY `newindex` (`version_range1`,`name`,`version_range2`)
) ENGINE=InnoDB AUTO_INCREMENT=11250 DEFAULT CHARSET=latin1;

/*Data for the table `opcodes` */

INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_LoginRequestMsg',0);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_LoginByNumRequestMsg',1);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_WSLoginRequestMsg',2);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_ESLoginRequestMsg',3);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_LoginReplyMsg',4);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_WSStatusReplyMsg',5);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_WorldStatusChangeMsg',6);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_AllWSDescRequestMsg',7);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_WorldListMsg',8);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_AllCharactersDescRequestMsg',9);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_AllCharactersDescReplyMsg',10);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_CreateCharacterRequestMsg',11);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_CreateCharacterReplyMsg',12);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_WSCreateCharacterRequestMsg',13);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_WSCreateCharacterReplyMsg',14);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_DeleteCharacterRequestMsg',15);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_DeleteCharacterReplyMsg',16);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_PlayCharacterRequestMsg',17);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_PlayCharacterReplyMsg',18);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_ServerPlayCharacterRequestMsg',19);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_ServerPlayCharacterReplyMsg',20);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_ESInitMsg',21);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_ESReadyForClientsMsg',22);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_CreateZoneInstanceMsg',23);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_ZoneInstanceCreateReplyMsg',24);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_ZoneInstanceDestroyedMsg',25);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_ExpectClientAsCharacterRequestMsg',26);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_ExpectClientAsCharacterReplyMsg',27);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_ZoneInfoMsg',28);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_DoneLoadingZoneResourcesMsg',29);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_DoneSendingInitialEntitiesMsg',30);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_DoneLoadingEntityResourcesMsg',31);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_PredictionUpdateMsg',32);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_SetRemoteCmdsMsg',33);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_RemoteCmdMsg',34);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_GameWorldTimeMsg',35);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_MOTDMsg',36);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_ZoneMOTDMsg',37);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_AvatarCreatedMsg',38);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_AvatarDestroyedMsg',39);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_RequestCampMsg',40);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_CampStartedMsg',41);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_CampAbortedMsg',42);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_WhoQueryRequestMsg',43);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_WhoQueryReplyMsg',44);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_MonitorReplyMsg',45);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_MonitorCharacterListMsg',46);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_MonitorCharacterListRequestMsg',47);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_ClientCmdMsg',48);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_DispatchClientCmdMsg',49);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_DispatchESMsg',50);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_UpdateTargetMsg',51);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_UpdateTargetLocMsg',52);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_UpdateCharacterSheetMsg',53);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_UpdateSpellBookMsg',54);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_UpdateInventoryMsg',56);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_UpdateRecipeBookMsg',57);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_UpdateSkillBookMsg',58);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_UpdateSkillsMsg',59);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_UpdateOpportunityMsg',60);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_ChangeZoneMsg',62);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_ClientTeleportRequestMsg',63);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_TeleportWithinZoneMsg',64);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_TeleportWithinZoneNoReloadMsg',65);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_MigrateClientToZoneRequestMsg',66);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_MigrateClientToZoneReplyMsg',67);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_ReadyToZoneMsg',68);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_AddClientToGroupMsg',69);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_AddGroupToGroupMsg',70);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_RemoveClientFromGroupMsg',71);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_RemoveGroupFromGroupMsg',72);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_MakeGroupLeaderMsg',73);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_GroupCreatedMsg',74);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_GroupDestroyedMsg',75);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_GroupMemberAddedMsg',76);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_GroupMemberRemovedMsg',77);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_GroupRemovedFromGroupMsg',78);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_GroupLeaderChangedMsg',79);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_GroupSettingsChangedMsg',80);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_SendLatestRequestMsg',81);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_ClearDataMsg',82);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_SetSocialMsg',83);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_ESStatusMsg',84);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_ESZoneInstanceStatusMsg',85);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_ZonesStatusRequestMsg',86);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_ZonesStatusMsg',87);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_ESWeatherRequestMsg',88);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_ESWeatherRequestEndMsg',89);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_WSWeatherUpdateMsg',90);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_DialogSelectMsg',91);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_DialogCloseMsg',92);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_RemoveSpellEffectMsg',93);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_RemoveConcentrationMsg',94);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_QuestJournalOpenMsg',95);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_QuestJournalInspectMsg',96);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_QuestJournalSetVisibleMsg',97);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_QuestJournalWaypointMsg',98);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_CreateGuildRequestMsg',99);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_CreateGuildReplyMsg',100);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_GuildsayMsg',101);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_GuildKickMsg',102);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_GuildUpdateMsg',103);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_FellowshipExpMsg',104);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_ConsignmentUpdateMsg',105);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_ConsignItemRequestMsg',106);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_ConsignItemResponseMsg',107);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_PurchaseConsignmentRequestMsg',108);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_PurchaseConsignmentResponseMsg',109);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_ProcessScriptMsg',110);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_ProcessWorkspaceMsg',111);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_HouseDeletedRemotelyMsg',112);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_UpdateHouseDataMsg',113);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_UpdateHouseAccessDataMsg',114);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_PlayerHouseBaseScreenMsg',115);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_PlayerHousePurchaseScreenMsg',116);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_PlayerHouseAccessUpdateMsg',117);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_PlayerHouseDisplayStatusMsg',118);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_PlayerHouseCloseUIMsg',119);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_BuyPlayerHouseMsg',120);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_BuyPlayerHouseTintMsg',121);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_CollectAllHouseItemsMsg',122);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_RelinquishHouseMsg',123);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_EnterHouseMsg',124);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_ExitHouseMsg',125);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_HouseDefaultAccessSetMsg',126);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_HouseAccessSetMsg',127);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_HouseAccessRemoveMsg',128);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_PayHouseUpkeepMsg',129);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_EnterMoveObjectModeMsg',130);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_PositionMoveableObject',131);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_ShaderCustomizationMsg',132);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_ReplaceableSubMeshesMsg',133);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_HouseCustomizationScreenMsg',134);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_CustomizationPurchaseRequestMsg',135);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_CustomizationSetRequestMsg',136);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_CustomizationReplyMsg',137);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_TintWidgetsMsg',138);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_ExamineConsignmentRequestMsg',139);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_ExamineConsignmentResponseMsg',140);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_UISettingsResponseMsg',141);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_KeymapLoadMsg',142);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_KeymapNoneMsg',143);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_KeymapDataMsg',144);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_KeymapSaveMsg',145);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_DispatchSpellCmdMsg',146);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_EntityVerbsRequestMsg',148);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_EntityVerbsReplyMsg',149);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_EntityVerbsVerbMsg',150);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_ChatRelationshipUpdateMsg',151);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_ChatFriendLoginStatusMsg',152);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_LootItemsRequestMsg',153);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_StoppedLootingMsg',154);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_SitMsg',155);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_StandMsg',156);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_SatMsg',157);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_StoodMsg',158);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_ExamineItemRequestMsg',159);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_DefaultGroupOptionsRequestMsg',160);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_DefaultGroupOptionsMsg',161);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_GroupOptionsMsg',162);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_DisplayGroupOptionsScreenMsg',163);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_DisplayInnVisitScreenMsg',164);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_DumpSchedulerMsg',165);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_LSRequestPlayerDescMsg',166);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_LSCheckAcctLockMsg',167);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_WSAcctLockStatusMsg',168);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_RequestHelpRepathMsg',169);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_RequestTargetLocMsg',170);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_UpdateMotdMsg',171);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_PerformPlayerKnockbackMsg',172);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_PerformCameraShakeMsg',173);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_PopulateSkillMapsMsg',174);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_CancelledFeignMsg',175);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_SignalMsg',176);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_ShowCreateFromRecipeUIMsg',177);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_CancelCreateFromRecipeMsg',178);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_BeginItemCreationMsg',179);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_StopItemCreationMsg',180);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_ShowItemCreationProcessUIMsg',181);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_UpdateItemCreationProcessUIMsg',182);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_DisplayTSEventReactionMsg',183);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_ShowRecipeBookMsg',184);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_KnowledgebaseRequestMsg',185);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_KnowledgebaseResponseMsg',186);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_CSTicketHeaderRequestMsg',187);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_CSTicketInfoMsg',188);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_CSTicketCommentRequestMsg',189);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_CSTicketCommentResponseMsg',190);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_CSTicketCreateMsg',191);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_CSTicketAddCommentMsg',192);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_CSTicketDeleteMsg',193);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_CSTicketChangeNotificationMsg',194);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_WorldDataUpdateMsg',195);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_KnownLanguagesMsg',196);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_LsRequestClientCrashLogMsg',197);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_LsClientBaselogReplyMsg',198);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_LsClientCrashlogReplyMsg',199);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_LsClientAlertlogReplyMsg',200);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_LsClientVerifylogReplyMsg',201);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_ClientTeleportToLocationMsg',202);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_UpdateClientPredFlagsMsg',203);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_ChangeServerControlFlagMsg',204);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_CSToolsRequestMsg',205);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_CSToolsResponseMsg',206);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_AddSocialStructureStandingMsg',207);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_CreateBoatTransportsMsg',208);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_PositionBoatTransportMsg',209);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_MigrateBoatTransportMsg',210);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_MigrateBoatTransportReplyMsg',211);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_DisplayDebugNLLPointsMsg',212);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_ExamineInfoRequestMsg',213);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_QuickbarInitMsg',214);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_QuickbarUpdateMsg',215);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_MacroInitMsg',216);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_MacroUpdateMsg',217);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_QuestionnaireMsg',218);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_LevelChangedMsg',219);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_SpellGainedMsg',220);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_EncounterBrokenMsg',221);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_OnscreenMsgMsg',222);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_ModifyGuildMsg',223);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_RewardPackMsg',224);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_RenameGuildMsg',225);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_ZoneToFriendRequestMsg',226);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_ZoneToFriendReplyMsg',227);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_ChatCreateChannelMsg',228);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_ChatJoinChannelMsg',229);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_ChatWhoChannelMsg',230);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_ChatLeaveChannelMsg',231);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_ChatTellChannelMsg',232);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_ChatTellUserMsg',233);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_ChatToggleFriendMsg',234);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_ChatToggleIgnoreMsg',235);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_ChatSendFriendsMsg',236);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_ChatSendIgnoresMsg',237);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_ChatFiltersMsg',238);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_WaypointRequestMsg',239);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_WaypointReplyMsg',240);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_WaypointSelectMsg',241);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_WaypointUpdateMsg',242);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_CharNameChangedMsg',243);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_ShowZoneTeleporterDestinationsMsg',244);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_SelectZoneTeleporterDestinationMsg',245);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_ReloadLocalizedTxtMsg',246);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_RequestGuildMembershipMsg',247);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_GuildMembershipResponseMsg',248);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_LeaveGuildNotifyMsg',249);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_JoinGuildNotifyMsg',250);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_AvatarUpdateMsg',251);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_BioUpdateMsg',252);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_InspectPlayerMsg',253);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_WSServerLockMsg',254);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_LSServerLockMsg',255);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_CsCategoryRequestMsg',256);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_CsCategoryResponseMsg',257);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_KnowledgeWindowSlotMappingMsg',258);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_LFGUpdateMsg',259);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_AFKUpdateMsg',260);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_UpdateActivePublicZonesMsg',261);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_UnknownNpcMsg',262);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_ConsumableItemsDetailsMsg',263);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_ConsignViewCreateMsg',264);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_ConsignViewGetPageMsg',265);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_ConsignViewReleaseMsg',266);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_ConsignRemoveItemsMsg',267);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_UpdateDebugRadiiMsg',268);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_SnoopMsg',269);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_ReportMsg',270);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_UpdateRaidMsg',271);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_ConsignViewSortMsg',272);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_EqHearChatCmd',273);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_EqDisplayTextCmd',274);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_EqCreateGhostCmd',275);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_EqCreateWidgetCmd',276);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_EqCreateSignWidgetCmd',277);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_EqDestroyGhostCmd',278);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_EqUpdateGhostCmd',279);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_EqSetControlGhostCmd',280);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_EqSetPOVGhostCmd',281);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_EqHearCombatCmd',282);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_EqHearSpellCastCmd',283);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_EqHearSpellInterruptCmd',284);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_EqHearSpellFizzleCmd',285);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_EqHearConsiderCmd',286);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_EqUpdateSubClassesCmd',287);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_EqCreateListBoxCmd',288);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_EqSetDebugPathPointsCmd',289);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_EqCannedEmoteCmd',290);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_EqStateCmd',291);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_EqPlaySoundCmd',292);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_EqPlaySound3DCmd',293);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_EqPlayVoiceCmd',294);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_EqHearDrowningCmd',295);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_EqHearDeathCmd',296);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_EqGroupMemberRemovedCmd',297);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_EqHearChainEffectCmd',298);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_EqReceiveOfferCmd',299);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_EqInspectPCResultsCmd',300);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_EqDrawablePathGraphCmd',301);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_EqDialogOpenCmd',302);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_EqDialogCloseCmd',303);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_EqQuestJournalUpdateCmd',304);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_EqQuestJournalReplyCmd',305);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_EqQuestGroupCmd',306);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_EqUpdateMerchantCmd',307);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_EqUpdateStoreCmd',308);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_EqUpdatePlayerTradeCmd',309);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_EqHelpPathCmd',310);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_EqHelpPathClearCmd',311);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_EqUpdateBankCmd',312);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_EqExamineInfoCmd',313);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_EqUpdateLootCmd',314);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_EqJunctionListCmd',315);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_EqShowDeathWindowCmd',316);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_EqDisplaySpellFailCmd',317);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_EqSpellCastStartCmd',318);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_EqSpellCastEndCmd',319);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_EqResurrectedCmd',320);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_EqChoiceWinCmd',321);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_EqSetDefaultVerbCmd',322);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_EqInstructionWindowCmd',323);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_EqInstructionWindowCloseCmd',324);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_EqInstructionWindowGoalCmd',325);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_EqInstructionWindowTaskCmd',326);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_EqEnableGameEventCmd',327);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_EqShowWindowCmd',328);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_EqEnableWindowCmd',329);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_EqFlashWindowCmd',330);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_EqHearPlayFlavorCmd',331);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_EqUpdateSignWidgetCmd',332);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_EqDebugPVDCmd',333);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_EqShowBookCmd',334);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_EqQuestionnaireCmd',335);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_EqGetProbsCmd',336);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_EqHearHealCmd',337);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_EqChatChannelUpdateCmd',338);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_EqWhoChannelQueryReplyCmd',339);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_EqAvailWorldChannelsCmd',340);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_EqUpdateTargetCmd',341);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_EqConsignmentItemsCmd',342);
INSERT INTO `opcodes` (`version_range1`,`version_range2`,`name`,`opcode`) VALUES
(283,283,'OP_EqStartBrokerCmd',343);

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;
