//ユニコードでコンパイル
#ifndef UNICODE
#define UNICODE
#endif
#ifndef _UNICODE
#define _UNICODE
#endif


#include <windows.h>
#include <tchar.h>
#include "main.h"
#include "searchaddr.h"


int GetOffsetAddressAll(HANDLE hProcess, DWORD dwDllAddress)
{
	int ret = 0;

	ZeroMemory(&g_Offset, sizeof(FFXIOFFSET));//0.3aで追加
	g_Offset.dwDllAddress = dwDllAddress;//0.05で追加

	g_Offset.Npcmap = GetOffsetAddress(hProcess, dwDllAddress, constNpcmapPattern);
	g_Offset.Map = GetOffsetAddress(hProcess, dwDllAddress, constMapPattern);
	g_Offset.NowSta = GetOffsetAddress(hProcess, dwDllAddress, constNowstaPattern);
	if (g_Offset.NowSta) {
		g_Offset.NowSta += NOWSTA_HOSEI;
	}
	g_Offset.Recast = GetOffsetAddress(hProcess, dwDllAddress, constRecastPattern);
	g_Offset.Cast = GetOffsetAddress(hProcess, dwDllAddress, constCastPattern);
	g_Offset.Viewstyle = GetOffsetAddress(hProcess, dwDllAddress, constVeiwstylePattern);
	g_Offset.Npclist = GetOffsetAddress(hProcess, dwDllAddress, constNpclistPattern);
	g_Offset.Activityinfo = GetOffsetAddress(hProcess, dwDllAddress, constActivityinfoPattern);
	if (g_Offset.Activityinfo) {
		g_Offset.Activityinfo += ACTIVITY_INFO_HOSEI;
	}
	g_Offset.Target = GetOffsetAddress(hProcess, dwDllAddress, constTargetPattern);
	g_Offset.Weather = GetOffsetAddress(hProcess, dwDllAddress, constWeatherPattern);
	g_Offset.Bazaar = GetOffsetAddress(hProcess, dwDllAddress, constBazaarPattern);
	g_Offset.Camera = GetOffsetAddress(hProcess, dwDllAddress, constCameraPattern);

	//倉庫番用 0.03で追加
	g_Offset.ItemInfo = GetOffsetAddress(hProcess, dwDllAddress, constItemInfoPattern);
	g_Offset.MenuHelp = GetOffsetAddress(hProcess, dwDllAddress, constMenuHelpPattern);
	g_Offset.MenuName = GetOffsetAddress(hProcess, dwDllAddress, constMenuNamePattern);
	DWORD Inventory1, Inventory2;//0.5で変更
	DWORD buf = GetOffsetAddress(hProcess, dwDllAddress, constInventory1Pattern);
	ReadProcessMemory(hProcess, (LPCVOID)buf, &Inventory1, sizeof(DWORD), NULL);//0.05で変更
	Inventory2 = GetOffsetAddress(hProcess, dwDllAddress, constInventory2Pattern);
	if (Inventory1 && Inventory2) {
		g_Offset.Inventory = Inventory1 + Inventory2 + INVENTORY_INFO_HOSEI - dwDllAddress;//0.05で変更
	}
	g_Offset.ItemNameList = GetOffsetAddress(hProcess, dwDllAddress, constItemNameListPattern);

	//その他 0.04で追加
#pragma region Realtimeloggets
	g_Offset.Login = GetOffsetAddress(hProcess, dwDllAddress, constLoginPattern);
	g_Offset.Loginfo = GetOffsetAddress(hProcess, dwDllAddress, constLogInfoPattern);
	g_Offset.Logserialno = GetOffsetAddress(hProcess, dwDllAddress, constLogSerialNoPattern);
	g_Offset.Status = GetOffsetAddress(hProcess, dwDllAddress, constStatusPattern); //状態変化(Buf)
	g_Offset.Pcmap = GetOffsetAddress(hProcess, dwDllAddress, constPcmapPattern);
	if (g_Offset.Npcmap) {
		g_Offset.Npcmap2 = g_Offset.Npcmap + NPCMAP2_HOSEI;//0.05で#define値に変更
	}

	buf = GetOffsetAddress(hProcess, dwDllAddress, constServer1Pattern);
	DWORD RtServer1, RtServer2;
	ReadProcessMemory(hProcess, (LPCVOID)buf, &RtServer1, sizeof(DWORD), NULL);
	if (RtServer1) {
		RtServer1 -= dwDllAddress;
	}
	RtServer2 = GetOffsetAddress(hProcess, dwDllAddress, constServer2Pattern);
	if (RtServer1 && RtServer2) {
		g_Offset.Server = RtServer1 + RtServer2;
	}

	buf = GetOffsetAddress(hProcess, dwDllAddress, constPos1Pattern);
	DWORD Pos1, Pos2;
	ReadProcessMemory(hProcess, (LPCVOID)buf, &Pos1, sizeof(DWORD), NULL);
	if (Pos1) {
		Pos1 -= dwDllAddress;
	}
	Pos2 = GetOffsetAddress(hProcess, dwDllAddress, constPos2Pattern);
	if (Pos1 && Pos2) {
		g_Offset.Pos = Pos1 + Pos2;
	}

	buf = GetOffsetAddress(hProcess, dwDllAddress, constInventorymax1Pattern);
	DWORD Inventorymax1, Inventorymax2;
	ReadProcessMemory(hProcess, (LPCVOID)buf, &Inventorymax1, sizeof(DWORD), NULL);
	if (Inventorymax1) {
		Inventorymax1 -= dwDllAddress;
	}
	Inventorymax2 = GetOffsetAddress(hProcess, dwDllAddress, constInventorymax2Pattern);//0.05で変更
	if (Inventorymax1 && Inventorymax2) {
		g_Offset.Inventorymax = Inventorymax1 + Inventorymax2;
	}

	DWORD PartyInfo1, PartyInfo2;//0.05aで変更
	buf = GetOffsetAddress(hProcess, dwDllAddress, constPartyinfo1Pattern);
	ReadProcessMemory(hProcess, (LPCVOID)buf, &PartyInfo1, sizeof(DWORD), NULL);
	if (PartyInfo1) {
		PartyInfo1 -= dwDllAddress;
	}
	PartyInfo2 = GetOffsetAddress(hProcess, dwDllAddress, constPartyinfo2Pattern);
	if (PartyInfo1 != 0 && PartyInfo2 != 0) {
		g_Offset.PartyInfo = PartyInfo1 + PartyInfo2;
	}
#pragma endregion

#pragma region XIACE
	g_Offset.Fish = GetOffsetAddress(hProcess, dwDllAddress, constFishPattern);
	if (g_Offset.Fish) {
		g_Offset.Fish -= FISH_INFO_HOSEI;//0.05で#define値に変更
	}
	g_Offset.Jobinfo = GetOffsetAddress(hProcess, dwDllAddress, constJobInfoPattern);
	g_Offset.Maxhpmpinfo = GetOffsetAddress(hProcess, dwDllAddress, constMaxHpMpInfoPattern);
	g_Offset.XIACE_Iteminfo = GetOffsetAddress(hProcess, dwDllAddress, constXIACEItemInfoPattern);
	g_Offset.Inventorycount = GetOffsetAddress(hProcess, dwDllAddress, constInventoryCountPattern);
	g_Offset.Loggedin = GetOffsetAddress(hProcess, dwDllAddress, constLoggedinPattern);

	buf = GetOffsetAddress(hProcess, dwDllAddress, constEquipInfo1Pattern);
	DWORD XIACEEquipInfo1, XIACEEquipInfo2;
	ReadProcessMemory(hProcess, (LPCVOID)buf, &XIACEEquipInfo1, sizeof(DWORD), NULL);//0.05で変更
	if (XIACEEquipInfo1)XIACEEquipInfo1 -= dwDllAddress;
	XIACEEquipInfo2 = GetOffsetAddress(hProcess, dwDllAddress, constEquipInfo2Pattern);//0.05で変更
	if (XIACEEquipInfo1 && XIACEEquipInfo2) {
		g_Offset.Equipinfo = XIACEEquipInfo1 + XIACEEquipInfo2;
	}
#pragma endregion

#pragma region NPC_MEM
	g_Offset.NpcMemRadian = (GetOffsetAddress(hProcess, dwDllAddress, constNpcMemRadianPattern) & 0xFF);
	g_Offset.NpcMemChangedId = (GetOffsetAddress(hProcess, dwDllAddress, constNpcMemChangedIdPattern) & 0xFF);
	g_Offset.NpcMemFixedId = (GetOffsetAddress(hProcess, dwDllAddress, constNpcMemFixedIdPattern) & 0xFF);
	g_Offset.NpcMemName = (GetOffsetAddress(hProcess, dwDllAddress, constNpcMemNamePattern) & 0xFF);//0.05aで変更
	g_Offset.NpcMemViewAddress = GetOffsetAddress(hProcess, dwDllAddress, constNpcMemViewAddressPattern);
	g_Offset.NpcMemDistance = GetOffsetAddress(hProcess, dwDllAddress, constNpcMemDistancePattern);
	g_Offset.NpcMemHP = GetOffsetAddress(hProcess, dwDllAddress, constNpcMemHpPattern);
	g_Offset.NpcMemGraphicsId = GetOffsetAddress(hProcess, dwDllAddress, constNpcMemGraphicsIdPattern);//0.05aで追加
	g_Offset.NpcMemNpcType = GetOffsetAddress(hProcess, dwDllAddress, constNpcMemNpcTypePattern);
	g_Offset.NpcMemBazaar = GetOffsetAddress(hProcess, dwDllAddress, constNpcMemBazaarPattern);
	if (g_Offset.NpcMemBazaar) {
		g_Offset.NpcMemBazaar += NPC_MEM_BAZAAR_HOSEI;
	}
	if (g_Offset.NpcMemNpcType) {
		g_Offset.NpcMemView = g_Offset.NpcMemNpcType + 1;
	}
	g_Offset.NpcMemInRange = GetOffsetAddress(hProcess, dwDllAddress, constNpcMemInRangePattern);
	g_Offset.NpcMemState = GetOffsetAddress(hProcess, dwDllAddress, constNpcMemStatePattern);
	g_Offset.NpcMemLastHateId = GetOffsetAddress(hProcess, dwDllAddress, constNpcMemLastHateIdPattern);
#pragma endregion

#pragma region FFXI小道具
	if (g_Offset.Pos) {
		g_Offset.SendBox = g_Offset.Pos - SendBox_HOSEI;
	}
	g_Offset.Sender = GetOffsetAddress(hProcess, dwDllAddress, constSenderPattern);
	g_Offset.CursorPos = GetOffsetAddress(hProcess, dwDllAddress, constCursorPosPattern);
	g_Offset.Order = GetOffsetAddress(hProcess, dwDllAddress, constOrderPattern);
	g_Offset.SendTos = GetOffsetAddress(hProcess, dwDllAddress, constSendTosPattern);
	g_Offset.InputMoney = GetOffsetAddress(hProcess, dwDllAddress, constInputMoneyPattern);
	DWORD Money1, Money2;//0.05aで追加
	buf = GetOffsetAddress(hProcess, dwDllAddress, constMoney1Pattern);
	ReadProcessMemory(hProcess, (LPCVOID)buf, &Money1, sizeof(DWORD), NULL);
	if (Money1) {
		Money1 -= dwDllAddress;
	}
	Money2 = GetOffsetAddress(hProcess, dwDllAddress, constMoney2Pattern);
	if (Money1 != 0 && Money2 != 0) {
		g_Offset.Money = Money1 + Money2 + MONEY_HOSEI;
	}
#pragma endregion

#pragma region Fish改
	g_Offset.LogAddr = GetOffsetAddress(hProcess, dwDllAddress, constLogAddrPattern);
#pragma endregion

#ifdef HOOK_INFO//0.05で追加
	g_offset.Ffhook_log = GetOffsetAddress(hProcess, dwDllAddress, constFfhook_logPattern);
	g_offset.Ffhook_cmd = GetOffsetAddress(hProcess, dwDllAddress, constFfhook_cmdPattern);
	ReadProcessMemory(hProcess, (LPCVOID)(g_offset.Ffhook_log + dwDllAddress + 1), &buf, sizeof(DWORD), NULL);
	ReadProcessMemory(hProcess, (LPCVOID)(g_offset.Ffhook_log + dwDllAddress + 5 + buf), &g_offset.Ffxihook_log, ARRAYSIZE(g_offset.Ffxihook_log), NULL);
	ReadProcessMemory(hProcess, (LPCVOID)(g_offset.Ffhook_cmd + dwDllAddress), &g_offset.Ffxihook_cmd, ARRAYSIZE(g_offset.Ffxihook_cmd), NULL);
#endif

#pragma region 泥男
	g_Offset.ArgusInitCode = GetOffsetAddress(hProcess, dwDllAddress, constArgusInitCodePattern);
	g_Offset.OwnPosition = GetOffsetAddress(hProcess, dwDllAddress, constOwnPositionPattern);
	if (g_Offset.OwnPosition) {
		g_Offset.OwnPosition += OwnPosition_HOSEI;
	}
	g_Offset.Delivery = GetOffsetAddress(hProcess, dwDllAddress, constDeliveryPattern);
	if (g_Offset.Delivery) {
		g_Offset.Delivery += Delivery_HOSEI;
	}
	g_Offset.Wait0DeliveryBox = GetOffsetAddress(hProcess, dwDllAddress, constWait0DeliveryBoxPattern);
	g_Offset.Wait0MogDeliveryBox = GetOffsetAddress(hProcess, dwDllAddress, constWait0MogDeliveryBoxPattern);
	if (g_Offset.Wait0MogDeliveryBox) {
		g_Offset.Wait0MogDeliveryBox += Wait0MogDeliveryBox_HOSEI;
	}
	g_Offset.Wait0SalesStatus = GetOffsetAddress(hProcess, dwDllAddress, constWait0SalesStatusPattern);
	if (g_Offset.Wait0SalesStatus) {
		g_Offset.Wait0SalesStatus += Wait0SalesStatus_HOSEI;
	}
	g_Offset.Wait0Bid = GetOffsetAddress(hProcess, dwDllAddress, constWait0BidPattern);
	g_Offset.Wait0Synthesis1 = GetOffsetAddress(hProcess, dwDllAddress, constWait0Synthesis1Pattern);
	if (g_Offset.Wait0Synthesis1) {
		g_Offset.Wait0Synthesis1 += Wait0Synthesis1_HOSEI;
	}
	g_Offset.Wait0Synthesis2 = GetOffsetAddress(hProcess, dwDllAddress, constWait0Synthesis2Pattern);
	if (g_Offset.Wait0Synthesis2) {
		g_Offset.Wait0Synthesis2 += Wait0Synthesis2_HOSEI;
	}
	g_Offset.WallThrough = GetOffsetAddress(hProcess, dwDllAddress, constWallThroughPattern);

	if (g_Offset.Pos) {
		g_Offset.AuctionHouseArea = g_Offset.Pos - AuctionHouseArea_HOSEI;
	}
	g_Offset.WideCamera = GetOffsetAddress(hProcess, dwDllAddress, constWideCameraPattern);
	if (g_Offset.WideCamera) {
		g_Offset.WideCamera += WideCamera_HOSEI;
	}
	g_Offset.MeripoMenu = GetOffsetAddress(hProcess, dwDllAddress, constMeripoMenuPattern);
	if (g_Offset.MeripoMenu) {
		g_Offset.MeripoMenu += MeripoMenu_HOSEI;
	}
	g_Offset.AuctionHouseMenu = GetOffsetAddress(hProcess, dwDllAddress, constAuctionHouseMenuPattern);
	if (g_Offset.AuctionHouseMenu) {
		g_Offset.AuctionHouseMenu += AuctionHouseMenu_HOSEI;
	}
	g_Offset.ViewMogHouse = GetOffsetAddress(hProcess, dwDllAddress, constViewMogHousePattern);
	if (g_Offset.ViewMogHouse) {
		g_Offset.ViewMogHouse += ViewMogHouse_HOSEI;
	}
	g_Offset.ViewMogHouseMenu = GetOffsetAddress(hProcess, dwDllAddress, constViewMogHouseMenuPattern);
	if (g_Offset.ViewMogHouseMenu) {
		g_Offset.ViewMogHouseMenu += ViewMogHouseMenu_HOSEI;
	}
	g_Offset.ChatLogColumn = GetOffsetAddress(hProcess, dwDllAddress, constChatLogColumnPattern);
	if (g_Offset.ChatLogColumn) {
		g_Offset.ChatLogColumn += ChatLogColumn_HOSEI;
	}
	g_Offset.CampaignInfo = GetOffsetAddress(hProcess, dwDllAddress, constCampaignInfoPattern);
	g_Offset.OwnAreaID = GetOffsetAddress(hProcess, dwDllAddress, constOwnAreaIDPattern);
	g_Offset.Bazaar2Meripo = (WORD)GetOffsetAddress(hProcess, dwDllAddress, constBazaar2MeripoPattern);
	if (g_Offset.Bazaar2Meripo) {
		g_Offset.Bazaar2Meripo += Bazaar2Meripo_HOSEI;
	}
	g_Offset.Meripo2Bazaar = (WORD)GetOffsetAddress(hProcess, dwDllAddress, constMeripo2BazaarPattern);
	if (g_Offset.Meripo2Bazaar) {
		g_Offset.Meripo2Bazaar += Meripo2Bazaar_HOSEI;
	}
	g_Offset.Flee = GetOffsetAddress(hProcess, dwDllAddress, constFleePattern);
	if (g_Offset.Flee) {
		g_Offset.Flee -= Flee_HOSEI;
	}
	g_Offset.FleeDefault1 = (BYTE)GetOffsetAddress(hProcess, dwDllAddress, constFleeDefaultPattern1);
	g_Offset.FleeDefault2 = (BYTE)GetOffsetAddress(hProcess, dwDllAddress, constFleeDefaultPattern2);
	g_Offset.FleeDefault3 = (BYTE)GetOffsetAddress(hProcess, dwDllAddress, constFleeDefaultPattern3);
	g_Offset.FleeDefault4 = (BYTE)GetOffsetAddress(hProcess, dwDllAddress, constFleeDefaultPattern4);
	g_Offset.FleeDefault5 = (BYTE)GetOffsetAddress(hProcess, dwDllAddress, constFleeDefaultPattern5);
	g_Offset.FleeDefault6 = (BYTE)GetOffsetAddress(hProcess, dwDllAddress, constFleeDefaultPattern6);
	g_Offset.CharaThrough = GetOffsetAddress(hProcess, dwDllAddress, constCharaThroughPattern);
	if (g_Offset.CharaThrough) {
		g_Offset.CharaThrough -= CharaThrough_HOSEI;
	}
	g_Offset.CharaThroughDefault1 = (BYTE)GetOffsetAddress(hProcess, dwDllAddress, constCharaThroughDefaultPattern1);
	g_Offset.CharaThroughDefault2 = (BYTE)GetOffsetAddress(hProcess, dwDllAddress, constCharaThroughDefaultPattern2);
	g_Offset.CharaThroughDefault3 = (BYTE)GetOffsetAddress(hProcess, dwDllAddress, constCharaThroughDefaultPattern3);
	g_Offset.CharaThroughDefault4 = (BYTE)GetOffsetAddress(hProcess, dwDllAddress, constCharaThroughDefaultPattern4);
	g_Offset.CharaThroughDefault5 = (BYTE)GetOffsetAddress(hProcess, dwDllAddress, constCharaThroughDefaultPattern5);
	if (g_Offset.NpcMemNpcType) {
		g_Offset.PlusValue = g_Offset.NpcMemNpcType + PlusValue_HOSEI;
	}
#pragma endregion

#pragma region 泥道
	g_Offset.BackGroundFlag = GetOffsetAddress(hProcess, dwDllAddress, constBackGroundFlagSearchPattern);
	if (g_Offset.BackGroundFlag) {
		g_Offset.BackGroundFlag -= BackGroundFlag_HOSEI;
	}
	else {
		g_Offset.BackGroundFlag = GetOffsetAddress(hProcess, dwDllAddress, constBackGroundFlagPattern);
	}
#pragma endregion

	//ぱぺすて
	if (g_Offset.Pos) {
		g_Offset.MainPuppet = g_Offset.Pos + MainPuppet_HOSEI;
	}
	g_Offset.PuppetTP = GetOffsetAddress(hProcess, dwDllAddress, constPuppetTPPattern);
	g_Offset.AbilityTable = GetOffsetAddress(hProcess, dwDllAddress, constAbilityTablePattern);

	//CAST
	if (g_Offset.AbilityTable) {
		g_Offset.AbilityRecast = g_Offset.AbilityTable + AbilityRecast_HOSEI;
	}
	g_Offset.BlueMagicHold = GetOffsetAddress(hProcess, dwDllAddress, constBlueMagicHoldPattern);
	g_Offset.MeripoValue = GetOffsetAddress(hProcess, dwDllAddress, constMeripoValuePattern);
	g_Offset.LinkShellName = GetOffsetAddress(hProcess, dwDllAddress, constLinkShellNamePattern);

	//そのほか
	if (g_Offset.Recast) {
		g_Offset.UtsusemiRecast = g_Offset.Recast + UtsusemiRecast_HOSEI;
	}
	if (g_Offset.NowSta) {
		g_Offset.Alliance1 = g_Offset.NowSta + Alliance1_HOSEI;
		g_Offset.Alliance2 = g_Offset.NowSta + Alliance2_HOSEI;
	}
	//チョコボの宝探し補助ツール(FFXiChocobo)
	g_Offset.PositionAddress = GetOffsetAddress(hProcess, dwDllAddress, constPositionAddressPattern);

	//FFXiLoginPoint
	if (g_Offset.Server) {
		g_Offset.CharList = g_Offset.Server - CharList_HOSEI;
	}
	//XiNPC
	g_Offset.XiNPC = GetOffsetAddress(hProcess, dwDllAddress, constXiNPCPattern);
	if (g_Offset.XiNPC) {
		g_Offset.XiNPC += XiNPC_HOSEI;
	}

	//NpcMem追加
	DWORD TargetPtr;
	BYTE Target1;
	ReadProcessMemory(hProcess, (LPCVOID)(dwDllAddress + g_Offset.Target), &TargetPtr, sizeof(DWORD), NULL);
	Target1 = (BYTE)GetOffsetAddress(hProcess, TargetPtr, constNpcMemLockOnSearchPattern);
	if (Target1) {
		g_Offset.NpcMemLockOn = Target1 - NpcMemLockOn_HOSEI;
		g_Offset.NpcMemSubTarget = g_Offset.NpcMemLockOn - NpcMemSubTarget_HOSEI;
	}

	//ChocoboCheckerPlus
	if (g_Offset.Pos) {
		g_Offset.OtherPC_Bazaar = g_Offset.Pos - 0x1480;
		g_Offset.Paddock = g_Offset.Pos - 0x338;
	}

	//ビットフラグ 0.03でret++に変更
	if (g_Offset.Npcmap == 0) { ret++; }
	if (g_Offset.Map == 0) { ret++; }
	if (g_Offset.NowSta == 0) { ret++; }
	if (g_Offset.Recast == 0) { ret++; }
	if (g_Offset.Cast == 0) { ret++; }
	if (g_Offset.Viewstyle == 0) { ret++; }
	if (g_Offset.Npclist == 0) { ret++; }
	if (g_Offset.Activityinfo == 0) { ret++; }
	if (g_Offset.Target == 0) { ret++; }
	if (g_Offset.Weather == 0) { ret++; }
	if (g_Offset.Bazaar == 0) { ret++; }

	//0.03で追加
	if (g_Offset.ItemInfo == 0) { ret++; }
	if (g_Offset.MenuHelp == 0) { ret++; }
	if (g_Offset.MenuName == 0) { ret++; }
	if (g_Offset.Inventory == 0) { ret++; }
	if (g_Offset.ItemNameList == 0) { ret++; }

	//0.04で追加
	if (g_Offset.Login == 0) { ret++; }
	if (g_Offset.Loginfo == 0) { ret++; }
	if (g_Offset.Logserialno == 0) { ret++; }
	if (g_Offset.PartyInfo == 0) { ret++; }
	if (g_Offset.Status == 0) { ret++; }
	if (g_Offset.Server == 0) { ret++; }
	if (g_Offset.Pos == 0) { ret++; }
	if (g_Offset.Pcmap == 0) { ret++; }
	if (g_Offset.Npcmap2 == 0) { ret++; }
	if (g_Offset.Fish == 0) { ret++; }
	if (g_Offset.Jobinfo == 0) { ret++; }
	if (g_Offset.Maxhpmpinfo == 0) { ret++; }
	if (g_Offset.XIACE_Iteminfo == 0) { ret++; }
	if (g_Offset.Inventorycount == 0) { ret++; }
	if (g_Offset.Loggedin == 0) { ret++; }
	if (g_Offset.Equipinfo == 0) { ret++; }
	if (g_Offset.Inventorymax == 0) { ret++; }

	//0.05で追加
	if (g_Offset.NpcMemRadian == 0) { ret++; }
	if (g_Offset.NpcMemChangedId == 0) { ret++; }
	if (g_Offset.NpcMemFixedId == 0) { ret++; }
	if (g_Offset.NpcMemName == 0) { ret++; }
	if (g_Offset.NpcMemViewAddress == 0) { ret++; }
	if (g_Offset.NpcMemDistance == 0) { ret++; }
	if (g_Offset.NpcMemHP == 0) { ret++; }
	if (g_Offset.NpcMemGraphicsId == 0) { ret++; }//0.05aで追加
	if (g_Offset.NpcMemNpcType == 0) { ret++; }
	if (g_Offset.NpcMemView == 0) { ret++; }
	if (g_Offset.NpcMemBazaar == 0) { ret++; }
	if (g_Offset.NpcMemInRange == 0) { ret++; }
	if (g_Offset.NpcMemState == 0) { ret++; }
	if (g_Offset.NpcMemLastHateId == 0) { ret++; }
	if (g_Offset.NpcMemLockOn == 0) { ret++; }
	if (g_Offset.NpcMemSubTarget == 0) { ret++; }
	if (g_Offset.CursorPos == 0) { ret++; }
	if (g_Offset.Order == 0) { ret++; }
	if (g_Offset.InputMoney == 0) { ret++; }
	if (g_Offset.LogAddr == 0) { ret++; }

#ifdef HOOK_INFO
	if (g_offset.Ffhook_log == 0) { ret++; }
	if (g_offset.Ffhook_cmd == 0) { ret++; }
#endif

	if (g_Offset.ArgusInitCode == 0) { ret++; }
	if (g_Offset.OwnPosition == 0) { ret++; }
	if (g_Offset.Delivery == 0) { ret++; }
	if (g_Offset.Wait0DeliveryBox == 0) { ret++; }
	if (g_Offset.Wait0MogDeliveryBox == 0) { ret++; }
	if (g_Offset.Wait0SalesStatus == 0) { ret++; }
	if (g_Offset.Wait0Bid == 0) { ret++; }
	if (g_Offset.Wait0Synthesis1 == 0) { ret++; }
	if (g_Offset.Wait0Synthesis2 == 0) { ret++; }
	if (g_Offset.WallThrough == 0) { ret++; }
	if (g_Offset.AuctionHouseArea == 0) { ret++; }
	if (g_Offset.WideCamera == 0) { ret++; }
	if (g_Offset.MeripoMenu == 0) { ret++; }
	if (g_Offset.AuctionHouseMenu == 0) { ret++; }
	if (g_Offset.ViewMogHouse == 0) { ret++; }
	if (g_Offset.ViewMogHouseMenu == 0) { ret++; }
	if (g_Offset.ChatLogColumn == 0) { ret++; }
	if (g_Offset.CampaignInfo == 0) { ret++; }
	if (g_Offset.OwnAreaID == 0) { ret++; }
	if (g_Offset.Flee == 0) { ret++; }
	if (g_Offset.CharaThrough == 0) { ret++; }
	if (g_Offset.Bazaar2Meripo == 0) { ret++; }
	if (g_Offset.Meripo2Bazaar == 0) { ret++; }
	//泥道
	if (g_Offset.BackGroundFlag == 0) { ret++; }
	 //ぱぺすて
	if (g_Offset.MainPuppet == 0) { ret++; }
	if (g_Offset.PuppetTP == 0) { ret++; }
	if (g_Offset.AbilityTable == 0) { ret++; }
	//CAST
	if (g_Offset.AbilityRecast == 0) { ret++; }
	if (g_Offset.BlueMagicHold == 0) { ret++; }
	if (g_Offset.MeripoValue == 0) { ret++; }
	if (g_Offset.LinkShellName == 0) { ret++; }
	//そのほか
	if (g_Offset.UtsusemiRecast == 0) { ret++; }
	if (g_Offset.Alliance1 == 0) { ret++; }
	if (g_Offset.Alliance2 == 0) { ret++; }
	if (g_Offset.PositionAddress == 0) { ret++; }
	if (g_Offset.CharList == 0) { ret++; }
	if (g_Offset.XiNPC == 0) { ret++; }

	return ret;
}

/// <summary>
/// オフセット取得
/// </summary>
/// <param name="hProcess"></param>
/// <param name="dwDllAddress">FFXiMain.dllのアドレス(ベースアドレス)</param>
/// <param name="pattern">バイトパターン</param>
/// <param name="dwStartAddress">サーチ開始アドレス（未指定時はdwDllAddressと同じ)</param>
/// <param name="dwSearchRange">未使用</param>
/// <returns></returns>
DWORD GetOffsetAddress(HANDLE hProcess, DWORD dwDllAddress, const BYTE* pattern, DWORD dwStartAddress, DWORD dwSearchRange)//0.05で変更
{
	DWORD dwAddress;
	DWORD dwOffset = 0;
	BYTE buf[SEARCH_BLOCK + PATTERN_MAX_RANGE];
	if (dwStartAddress == 0) {
		dwStartAddress = dwDllAddress;//サーチ開始アドレスをFFXiMain.dllのアドレスにする 0.05で追加
	}

	for (int i = 0; i <= (int)SEARCH_RANGE / SEARCH_BLOCK; i++) {//0.05で変更
		dwAddress = dwStartAddress + SEARCH_BLOCK * i;//0.05で変更
		if (0 == ReadProcessMemory(hProcess, (LPCVOID)dwAddress, buf, SEARCH_BLOCK + PATTERN_MAX_RANGE, NULL)) {
			break;
		}

		if ((dwOffset = SearchAddress(buf, pattern, dwAddress, dwDllAddress)) != 0) {
			break;
		}
	}

	return dwOffset;
}

/// <summary>
/// パターン検索
/// </summary>
/// <param name="base"></param>
/// <param name="pattern"></param>
/// <param name="dwReadAddress">開始アドレス</param>
/// <param name="dwDllAddress">FFXiMain.dllのアドレス(ベースアドレス)</param>
/// <returns></returns>
DWORD SearchAddress(BYTE *base, const BYTE *pattern, DWORD dwReadAddress, DWORD dwDllAddress)
{
	BYTE	dat = 0, ptrn[256], mask[256], *p1, *p2;
	char	c, toggle = 0;
	int		n, cnt = 0, fp;
	DWORD	dwAddress = 0;

	//検索パターン展開
	ZeroMemory(ptrn, sizeof(ptrn));
	ZeroMemory(mask, sizeof(mask));

	toggle = 0;
	cnt = 0;

	for (n = 0; n < 100; n++) {
		c = (char)tolower(pattern[n]);
		if (c == ' ') {
			continue;
		}
		if (c == 0) { //文字列の終端コード
			mask[cnt] = 1;
			break;
		}
		if (c == '.') {
			mask[cnt] = 1;		//「.」マスクパターン
		}
		else if (c == 'x') {
			mask[cnt] = 2;		//「x」アドレス部(ベースアドレス+オフセット値)
		}
		else if (c == 'p') {//0.05から利用可
			mask[cnt] = 3;		// 「p」オフセット部
		}
		else if (c == 'z') {
			mask[cnt] = 4;		// 「z」オフセット部
		}
		else if ('0' <= c && c <= '9') {
			dat |= (c - '0');
		}
		else if ('a' <= c && c <= 'f') {
			dat |= (c - 'a' + 10);
		}
		else {
			return false;
		}

		if (toggle) {
			ptrn[cnt++] = dat;
			dat = 0;
		}
		else {
			dat <<= 4;
		}
		toggle ^= 1; //上位4bit,下位4bitの切り替えフラグ
	}

	// 最初に有効なパターンの位置を検索(.を飛ばす)
	for (n = 0; n < cnt; n++) {
		if (mask[n] == 0) {
			//「.」のときスキップ
			break;
		}
	}

	fp = n;

	for (p1 = base; p1 < base + SEARCH_BLOCK; p1++) {
		if (*p1 != ptrn[fp]) {
			continue;
		}
		p2 = p1 + 1;
		for (n = fp + 1; n < cnt; n++, p2++) {
			if (mask[n]) {
				continue;
			}
			else if (*p2 != ptrn[n]) {
				break;
			}
		}
		if (n == cnt) {
			//一致したFFXiMain.dll先頭からの差分アドレス(xpが存在しないパターンはこの値が使われる)
			dwAddress = (DWORD)(p1 - fp - base) + dwReadAddress - dwDllAddress;
			for (n = 0; n < cnt; n++) {
				if (mask[n] == 2) {//x:アドレス部
					memcpy(&dwAddress, &base[(DWORD)(p1 - fp - base) + n], sizeof(DWORD));
					dwAddress -= dwDllAddress;//こっちでベースアドレスを引く
					break;
				}
			}
			for (n = 0; n < cnt; n++) {//pに対応
				if (mask[n] == 3) {//p:オフセット部
					memcpy(&dwAddress, &base[(DWORD)(p1 - fp - base) + n], sizeof(DWORD));
					break;
				}
			}
			for (n = 0; n < cnt; n++) {//zに対応
				if (mask[n] == 4) {//z:byte値
					memcpy(&dwAddress, &base[(DWORD)(p1 - fp - base) + n], sizeof(BYTE));
					break;
				}
			}
			break;
		}
	}

	return dwAddress;
}