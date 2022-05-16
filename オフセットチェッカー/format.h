#define FORMAT_MAPPU    \
    _T(";---Mappu---\r\n")\
    _T("[OFFSET]\r\n")\
    _T("NPCMAP=%X\r\n")\
    _T("MAP=%X\r\n")\
    _T("NPCLIST=%X\r\n")\
    _T("NOWSTA=%X\r\n")\
    _T("TARGET=%X\r\n")\
    _T("WEATHER=%X\r\n")\
    _T("\r\n")\
    ,g_Offset.Npcmap,g_Offset.Map,g_Offset.Npclist,g_Offset.NowSta,g_Offset.Target,g_Offset.Weather

#define FORMAT_HEALER   \
    _T(";---���Y����---\r\n")\
    _T("[OFFSET]\r\n")\
    _T("NPCMAP=%X\r\n")\
    _T("MAP=%X\r\n")\
    _T("NOWSTA=%X\r\n")\
    _T("RECAST=%X\r\n")\
    _T("CAST=%X\r\n")\
    _T("VIEWSTYLE=%X\r\n")\
    _T("TARGET=%X\r\n")\
    _T("NPCLIST=%X\r\n")\
    _T("ACTIVITY_INFO=%X\r\n")\
    _T("CAMERA=%X\r\n")\
    _T("\r\n")\
    ,g_Offset.Npcmap,g_Offset.Map,g_Offset.NowSta,g_Offset.Recast,g_Offset.Cast,g_Offset.Viewstyle,g_Offset.Target,g_Offset.Npclist,g_Offset.Activityinfo,g_Offset.Camera

#define FORMAT_BAZAAR   \
    _T(";---Bazaar---\r\n")\
    _T("[OFFSET]\r\n")\
    _T("NPCMAP=%X\r\n")\
    _T("NOWSTA=%X\r\n")\
    _T("BAZAAR=%X\r\n")\
    _T("\r\n")\
    ,g_Offset.Npcmap,g_Offset.NowSta,g_Offset.Bazaar

#define FORMAT_GRAPHICSID2JOBS  \
    _T(";---GraphicsID2Jobs---\r\n")\
    _T("[OFFSET]\r\n")\
    _T("NPCMAP=%X\r\n")\
    _T("NOWSTA=%X\r\n")\
    _T("\r\n")\
    ,g_Offset.Npcmap,g_Offset.NowSta

#define FORMAT_NPCMEM   \
    _T(";---NPC_MEM---\r\n")\
    _T("[NPC_MEM]\r\n")\
    _T("POS_X=04\r\n")\
    _T("POS_Y=08\r\n")\
    _T("POS_Z=0C\r\n")\
    _T("RADIAN=%X\r\n")\
    _T("CHANGEDID=%X\r\n")\
    _T("FIXEDID=%X\r\n")\
    _T("NAME=%X\r\n")\
    _T("VIEW_ADDRESS=%X\r\n")\
    _T("DISTANCE=%X\r\n")\
    _T("HP=%X\r\n")\
    _T("GRAPHICSID=%X\r\n")\
    _T("BAZAAR=%X\r\n")\
    _T("NPCTYPE=%X\r\n")\
    _T("VIEW=%X\r\n")\
    _T("INRANGE=%X\r\n")\
    _T("STATE=%X\r\n")\
    _T("LASTHATEID=%X\r\n")\
    _T("LOCKON = %02X\r\n")\
    _T("SUBTAGE = %02X\r\n")\
    _T("\r\n")\
    ,g_Offset.NpcMemRadian,g_Offset.NpcMemChangedId,g_Offset.NpcMemFixedId,g_Offset.NpcMemName,g_Offset.NpcMemViewAddress,g_Offset.NpcMemDistance,g_Offset.NpcMemHP,g_Offset.NpcMemGraphicsId,g_Offset.NpcMemBazaar,g_Offset.NpcMemNpcType,g_Offset.NpcMemView,g_Offset.NpcMemInRange,g_Offset.NpcMemState,g_Offset.NpcMemLastHateId,g_Offset.NpcMemLockOn,g_Offset.NpcMemSubTarget

#define FORMAT_SOUKOBAN \
    _T(";---�q�ɔ�---\r\n")\
    _T("[OFFSET]\r\n")\
    _T("ItemInfo=%X\r\n")\
    _T("MenuHelp=%X\r\n")\
    _T("MenuName=%X\r\n")\
    _T("Inventory=%X\r\n")\
    _T("ItemNameList=%X\r\n")\
    _T("\r\n")\
    ,g_Offset.ItemInfo,g_Offset.MenuHelp,g_Offset.MenuName,g_Offset.Inventory,g_Offset.ItemNameList

#define FORMAT_REALTIMELOGGETS  \
    _T(";---RealTimeLogGets---\r\n")\
    _T("���O�C�����:0x%X\r\n")\
    _T("���O���:0x%X\r\n")\
    _T("���O�ʔ�:0x%X\r\n")\
    _T("PT���:0x%X\r\n")\
    _T("�X�e�[�^�X:0x%X\r\n")\
    _T("�T�[�o�[:0x%X\r\n")\
    _T("�ʒu:0x%X\r\n")\
    _T("�^�[�Q�b�g:0x%X\r\n")\
    _T("PC���X�g:0x%X\r\n")\
    _T("NPC���X�g1:0x%X\r\n")\
    _T("NPC���X�g2:0x%X\r\n")\
    _T("\r\n")\
    ,g_Offset.Login,g_Offset.Loginfo,g_Offset.Logserialno,g_Offset.PartyInfo,g_Offset.Status,g_Offset.Server,g_Offset.Pos,g_Offset.Target,g_Offset.Pcmap,g_Offset.Npcmap,g_Offset.Npcmap2

#define FORMAT_XIACE    \
    _T(";---XIACE---\r\n")\
    _T("FISH_INFO = 0x%X,\r\n")\
    _T("TARGET_INFO = 0x%X,\r\n")\
    _T("PLAYER_INFO = 0x%X,\r\n")\
    _T("JOB_INFO = 0x%X,\r\n")\
    _T("MAXHPMP_INFO = 0x%X,\r\n")\
    _T("ACTIVITY_INFO = 0x%X,\r\n")\
    _T("ITEM_INFO = 0x%X,\r\n")\
    _T("INVENTORY_COUNT = 0x%X,\r\n")\
    _T("LOGGED_IN = 0x%X,\r\n")\
    _T("BUFFS_INFO = 0x%X,\r\n")\
    _T("EQUIP_INFO = 0x%X,\r\n")\
    _T("INVENTORY_INFO = 0x%X,\r\n")\
    _T("INVENTORY_MAX = 0x%X,\r\n")\
    _T("SAFEBOX_INFO = INVENTORY_INFO + (81 * 0x2c),\r\n")\
    _T("STORAGE_INFO = SAFEBOX_INFO + (81 * 0x2c),\r\n")\
    _T("TEMPITEM_INFO = STORAGE_INFO + (81 * 0x2c),\r\n")\
    _T("LOCKER_INFO = TEMPITEM_INFO + (81 * 0x2c),\r\n")\
    _T("SATCHEL_INFO = LOCKER_INFO + (81 * 0x2c),\r\n")\
    _T("SACK_INFO = SATCHEL_INFO + (81 * 0x2c),\r\n")\
    _T("CASE_INFO = SACK_INFO + (81 * 0x2c),\r\n")\
    _T("WORDROBE_INFO = CASE_INFO + (81 * 0x2c),\r\n")\
    _T("SAFEBOX2_INFO = WORDROBE_INFO + (81 * 0x2c),\r\n")\
    _T("WORDROBE2_INFO = SAFEBOX2_INFO + (81 * 0x2c),\r\n")\
    _T("WORDROBE3_INFO = WORDROBE2_INFO + (81 * 0x2c),\r\n")\
    _T("WORDROBE4_INFO = WORDROBE3_INFO + (81 * 0x2c),\r\n")\
    _T("WORDROBE5_INFO = WORDROBE4_INFO + (81 * 0x2c),\r\n")\
    _T("WORDROBE6_INFO = WORDROBE5_INFO + (81 * 0x2c),\r\n")\
    _T("WORDROBE7_INFO = WORDROBE6_INFO + (81 * 0x2c),\r\n")\
    _T("WORDROBE8_INFO = WORDROBE7_INFO + (81 * 0x2c),\r\n")\
    _T("\r\n")\
    ,g_Offset.Fish,g_Offset.Target,g_Offset.NowSta,g_Offset.Jobinfo,g_Offset.Maxhpmpinfo,g_Offset.Activityinfo,g_Offset.XIACE_Iteminfo,g_Offset.Inventorycount,g_Offset.Loggedin,g_Offset.Status,g_Offset.Equipinfo,g_Offset.Inventory,g_Offset.Inventorymax

#define FORMAT_KODOUGU  \
    _T(";---FFXI������---\r\n")\
    _T(";�|�X�g�͖��Ή�\r\n")\
    _T("�{�b�N�X���g = %X\r\n")\
    _T("����� = %X\r\n")\
    _T("�A�C�e���� = %X\r\n")\
    _T("�J�[�\���ʒu = %X\r\n")\
    _T("�w���v = %X\r\n")\
    _T("���ɓ��e = %X\r\n")\
    _T("�J�o�����e = %X\r\n")\
    _T("���[���e = %X\r\n")\
    _T("���b�J�[���e = %X\r\n")\
    _T("�T�b�`�F�����e = %X\r\n")\
    _T("�J�o���ő� = %X\r\n")\
    _T("�J�o������ = %X\r\n")\
    _T("�A�C�R�� = %X\r\n")\
    _T("�I�[�_�[ = %X\r\n")\
    _T("�A�C�e��ID = %X\r\n")\
    _T("����� = %X\r\n")\
    _T("�������z = %X\r\n")\
    _T("���͋��z = %X\r\n")\
    _T("\r\n")\
    ,g_Offset.SendBox,g_Offset.Sender,g_Offset.ItemNameList,g_Offset.CursorPos,g_Offset.MenuHelp,g_Offset.Inventory?g_Offset.Inventory + 81 * 0x2C:0,g_Offset.Inventory,g_Offset.Inventory?g_Offset.Inventory + 81 * 0x2C * 2:0,g_Offset.Inventory?g_Offset.Inventory + 81 * 0x2C * 4:0,g_Offset.Inventory?g_Offset.Inventory + 81 * 0x2C * 5:0,g_Offset.Inventorymax,g_Offset.Inventorycount,g_Offset.Status,g_Offset.Order,g_Offset.ItemInfo,g_Offset.SendTos,g_Offset.Money,g_Offset.InputMoney

#define FORMAT_FISHKAI  \
    _T(";---Fish��---\r\n")\
    _T("FishAddr = %X\r\n")\
    _T("LogAddr = %X\r\n")\
    _T("\r\n")\
    ,g_Offset.Fish,g_Offset.LogAddr

#define FORMAT_HOOK_INFO    \
    _T(";---hook address---\r\n")\
    _T(";---ffhook.dll---\r\n")\
    _T("pattern_log = 0x%08X\r\n")\
    _T("pattern_cmd = 0x%08X\r\n")\
    _T(";---ffxihook_log.dll---\r\n")\
    _T("\"bin\" = hex:%s\r\n")\
    _T("\"mask\" = hex:FF,FF,00,00,00,00,FF,FF,FF,FF,FF,FF,00,00,00,00,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF,FF\r\n")\
    _T(";---ffxihook_cmd.dll---\r\n")\
    _T("\"bin\" = hex:%s\r\n")\
    _T("\"mask\" = hex:00,00,00,00,00,00,FF,FF,FF,00,00,00,00,FF,FF,FF,FF,FF,FF,00,00,00,00,FF,00,00,00,00,FF,FF,FF,FF\r\n")\
    ,g_Offset.Ffhook_log,g_Offset.Ffhook_cmd,szHexLog,szHexCmd

#define FORMAT_MADMAN   \
    _T("�����������D�j����������\r\n")\
    _T("[OFFSET]\r\n")\
    _T("INITCODE = %X\r\n")\
    _T("NPCMAP = %X\r\n")\
    _T("OWNPOSITION = %X\r\n")\
    _T("�f���o���[ = 0%X\r\n")\
    _T("��z = 0%X\r\n")\
    _T("���O�|�X�g = 0%X\r\n")\
    _T("�a���m�F = 0%X\r\n")\
    _T("���D = %X\r\n")\
    _T("�����P = %X\r\n")\
    _T("�����Q = %X\r\n")\
    _T("�l���� = 0%X\r\n")\
    _T("�ǔ��� = 0%X\r\n")\
    _T("�����G���A = %X\r\n")\
    _T("�L�扻 = 0%X\r\n")\
    _T("�����| = %X\r\n")\
    _T("���O�n�m�F = %X\r\n")\
    _T("�������j�� = %X\r\n")\
    _T("���m���j�� = %X\r\n")\
    _T("�s���ύX = %X\r\n")\
    _T("�J���p�j�G = %X\r\n")\
    _T("FLEE = 0%X\r\n")\
    _T("OWNAREAID = %X\r\n")\
    _T("\r\n")\
    _T("[CODE]\r\n")\
    _T("FLEE�f�t�H���g = %02X%02X%02X%02X%02X%02X\r\n")\
    _T("�l�����f�t�H���g = %02X%02X%02X%02X%02X\r\n")\
    _T("�o�U�[�˃����| = %X\r\n")\
    _T("�����|�˃o�U�[ = %X\r\n")\
    _T("\r\n")\
    _T("�v���X�l = %X\r\n")\
    _T("�Ԋu = 1000\r\n")\
    _T("\r\n")\
    , g_Offset.ArgusInitCode, g_Offset.Npcmap, g_Offset.OwnPosition, g_Offset.Delivery, g_Offset.Wait0DeliveryBox, g_Offset.Wait0MogDeliveryBox, g_Offset.Wait0SalesStatus, g_Offset.Wait0Bid, g_Offset.Wait0Synthesis1, g_Offset.Wait0Synthesis2, g_Offset.CharaThrough, g_Offset.WallThrough, g_Offset.AuctionHouseArea, g_Offset.WideCamera, g_Offset.MeripoMenu, g_Offset.ViewMogHouse, g_Offset.AuctionHouseMenu, g_Offset.ViewMogHouseMenu, g_Offset.ChatLogColumn, g_Offset.CampaignInfo, g_Offset.Flee, g_Offset.OwnAreaID, g_Offset.FleeDefault1, g_Offset.FleeDefault2, g_Offset.FleeDefault3, g_Offset.FleeDefault4, g_Offset.FleeDefault5, g_Offset.FleeDefault6, g_Offset.CharaThroughDefault1, g_Offset.CharaThroughDefault2, g_Offset.CharaThroughDefault3, g_Offset.CharaThroughDefault4, g_Offset.CharaThroughDefault5, g_Offset.Bazaar2Meripo, g_Offset.Meripo2Bazaar, g_Offset.PlusValue

#define FORMAT_MADMOVE  \
    _T("�����������D������������\r\n")\
    _T("[OFFSET]\r\n")\
    _T("NPCMAP = %X\r\n")\
    _T("OWNPOSITION = %X\r\n")\
    _T("VIEWSTYLE = %X\r\n")\
    _T("OWNAREAID = %X\r\n")\
    _T("BGF = %X\r\n")\
    _T("\r\n")\
    , g_Offset.Npcmap, g_Offset.OwnPosition, g_Offset.Viewstyle, g_Offset.OwnAreaID, g_Offset.BackGroundFlag

#define FORMAT_PUPSTATUS    \
    _T("���������ς؂��ā�������\r\n")\
    _T("[OFFSET]\r\n")\
    _T("MAINPUP = %X\r\n")\
    _T("PUPTP = %X\r\n")\
    _T("JOB = %X\r\n")\
    _T("���ð��� = %X\r\n")\
    _T("NPCMAP = %X\r\n")\
    _T("\r\n")\
    , g_Offset.MainPuppet, g_Offset.PuppetTP, g_Offset.Jobinfo, g_Offset.AbilityTable, g_Offset.Npcmap

#define FORMAT_ARGUS    \
    _T("���������A�[�K�X��������\r\n")\
    _T("[OFFSET]\r\n")\
    _T("INITCODE = %X\r\n")\
    _T("PCMAP = %X\r\n")\
    _T("NPCMAP = %X\r\n")\
    _T("TARGETINFO = %X\r\n")\
    _T("OWNPOSITION = %X\r\n")\
    _T("\r\n")\
    , g_Offset.ArgusInitCode, g_Offset.Pcmap, g_Offset.Npcmap, g_Offset.Target, g_Offset.OwnPosition

#define FORMAT_CAST \
    _T("��������CAST��������\r\n")\
    _T("[OFFSET]\r\n")\
    _T("�W���u��� = %X\r\n")\
    _T("�����| = %X\r\n")\
    _T("�p�[�e�B��� = %X\r\n")\
    _T("�A�r�e�[�u�� = %X\r\n")\
    _T("���@���L���X�g = %X\r\n")\
    _T("���S�� = %X\r\n")\
    _T("PCMAP = %X\r\n")\
    _T("TARGETINFO = %X\r\n")\
    _T("�M����� = %X\r\n")\
    _T("�V���� = %X\r\n")\
    _T("�������� = %X\r\n")\
    _T("���΂� = %X\r\n")\
    _T("�A�N�V���� = %X\r\n")\
    _T("�A�C�e�� = %X\r\n")\
    _T("�A�C�e����� = %X\r\n")\
    _T("�����N�V�F���� = %X\r\n")\
    _T("\r\n")\
    , g_Offset.Jobinfo, g_Offset.MeripoValue, g_Offset.NowSta, g_Offset.AbilityTable, g_Offset.Recast, g_Offset.BlueMagicHold, g_Offset.Pcmap, g_Offset.Target, g_Offset.Money, g_Offset.Weather, g_Offset.Inventorycount, g_Offset.Inventorymax, g_Offset.Activityinfo, g_Offset.ItemInfo, g_Offset.Inventory, g_Offset.LinkShellName

#define FORMAT_DORO_DLL \
    _T("//----Cast�pdoro.dll��p----\r\n")\
    _T("CHOICE = %X\r\n")\
    _T("BGF = %X\r\n")\
    _T("MYPOS = %X\r\n")\
    _T("\r\n")\
    , g_Offset.CursorPos, g_Offset.BackGroundFlag, g_Offset.Pos

#define FORMAT_OTHER    \
    _T("�����������̂ق���������\r\n")\
    _T("[OFFSET]\r\n")\
    _T("���ð��� = %X\r\n")\
    _T("���ط��� = %X\r\n")\
    _T("���@ط��� = %X\r\n")\
    _T("���ط��� = %X\r\n")\
    _T("���S�� = %X\r\n")\
    _T("MAP = %X\r\n")\
    _T("MYPOS = %X\r\n")\
    _T("NPCMAP2 = %X\r\n")\
    _T("NPCLIST = %X\r\n")\
    _T("\r\n")\
    _T("��������FF11statEX��������\r\n")\
    _T("���o�s = %X\r\n")\
    _T("�A���P = %X\r\n")\
    _T("�A���Q = %X\r\n")\
    _T("\r\n")\
    , g_Offset.AbilityTable, g_Offset.AbilityRecast, g_Offset.Recast, g_Offset.UtsusemiRecast, g_Offset.BlueMagicHold, g_Offset.Map, g_Offset.Pos, g_Offset.Npcmap2, g_Offset.Npclist, g_Offset.NowSta, g_Offset.Alliance1, g_Offset.Alliance2

#define FORMAT_FFXI_CHOCOBO \
    _T("��������FFXiChocobo��������\r\n")\
    _T("  <AreaIDAddress>%d</AreaIDAddress>\r\n")\
    _T("  <PositionAddress>%d</PositionAddress>\r\n")\
    _T("\r\n")\
    , g_Offset.Map, g_Offset.PositionAddress

#define FORMAT_FFXI_LOGIN_POINT \
    _T("��������FFXiLoginPoint��������\r\n")\
    _T("[OFFSET]\r\n")\
    _T("CHARLIST = %X\r\n")\
    _T("\r\n")\
    , g_Offset.CharList

#define FORMAT_XINPC    \
    _T("��������XiNPC��������\r\n")\
    _T("[OFFSET]\r\n")\
    _T("XINPC = %X\r\n")\
    _T("NPCMAP = %X\r\n")\
    _T("\r\n")\
    , g_Offset.XiNPC, g_Offset.Npcmap

#define FORMAT_CHOCOBO_CHECKER_PLUS \
    _T("��������ChocoboCheckerPlus��������\r\n")\
    _T("  <ItemAddress>%d</ItemAddress>\r\n")\
    _T("  <BazaarAddress>%d</BazaarAddress>\r\n")\
    _T("  <PaddockAddress>%d</PaddockAddress>\r\n")\
    _T("\r\n")\
    , g_Offset.Inventory, g_Offset.OtherPC_Bazaar, g_Offset.Paddock
