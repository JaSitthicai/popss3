
#include	<YSI_Coding\y_hooks>
#define     MAX_INVENTORY       	(1500)

enum INV_DATA {
	invExists,
	invID,
	invItem[32],
	invQuantity
};
new invData[MAX_PLAYERS][MAX_INVENTORY][INV_DATA];

enum ITEM_NAME_DATA {
	itemName[32]
};

//--> �������
new const itemData[][ITEM_NAME_DATA] = {
	{ "�ͻ����" },
	{ "�ͻ�������ٻ" },
	{ "�ԫ���" }, 
	{ "�����������" }, 
	{ "�Һ" },
	{ "�絵����" },
	{ "�ٹ" },
	{ "��ʴ" },
	{ "�����ٻ" },
	{ "��ʴ����ٻ" },
	{ "���" }
};

forward OpenItemList(playerid);
public OpenItemList(playerid){
	new string[512],string2[512],string3[256],count,var[32];
    for (new i = 0; i < playerData[playerid][MaxItem]; i ++){
 		if (invData[playerid][i][invExists]) {
   			format(string, sizeof(string), "%s\t%d\n", invData[playerid][i][invItem], invData[playerid][i][invQuantity]);
   			strcat(string2, string);
   			format(var, sizeof(var), "itemlist%d", count);
   			SetPVarInt(playerid, var, i);
   			count++;
		}
	}
	if (!count) {
        Dialog_Show(playerid, Show, DIALOG_STYLE_MSGBOX, "{FF0000}ERROR", "{FFFFFF}�͹���㹵�Ǥس�������������˹��¹�", "�Դ", "");
		return 1;
	}
	playerData[playerid][ItemSelect] = 0;
	format(string, sizeof(string), "{FFFFFF}��¡��\t{FFFFFF}�ӹǹ�����\n%s",string2);
	format(string3, sizeof(string3), "{FF0000}INVENTORY %d/%d", Inventory_Items(playerid), playerData[playerid][MaxItem]);
	return Dialog_Show(playerid, DIALOG_INVENTORY, DIALOG_STYLE_TABLIST_HEADERS, string3, string, "���͡", "��͹��Ѻ");
}
stock Inventory_Clear(playerid)
{
	static string[64];

	for (new i = 0; i < MAX_INVENTORY; i ++)
	{
	    if (invData[playerid][i][invExists])
	    {
	        invData[playerid][i][invExists] = 0;
	        invData[playerid][i][invQuantity] = 0;
		}
	}
	mysql_format(Database, string, sizeof(string), "DELETE FROM `Inventory` WHERE `invOwner` = '%d'", playerData[playerid][ID]);
	return mysql_tquery(Database, string);
}

stock Inventory_Set(playerid, const item[], amount)
{
	new itemid = Inventory_GetItemID(playerid, item);

	if (itemid == -1 && amount > 0)
	{
		Inventory_Add(playerid, item, amount);
	}
	else if (amount > 0 && itemid != -1)
	{
	    Inventory_SetQuantity(playerid, item, amount);
	}
	else if (amount < 1 && itemid != -1)
	{
	    Inventory_Remove(playerid, item, -1);
	}
	return 1;
}

Inventory_GetItemID(playerid, const item[])
{
	for (new i = 0; i < MAX_INVENTORY; i ++)
	{
	    if (!invData[playerid][i][invExists])
	        continue;

		if (!strcmp(invData[playerid][i][invItem], item)) return i;
	}
	return -1;
}

Inventory_GetFreeID(playerid)
{
	if (Inventory_Items(playerid) >= playerData[playerid][MaxItem])
		return -1;

	for (new i = 0; i < MAX_INVENTORY; i ++)
	{
	    if (!invData[playerid][i][invExists])
	        return i;
	}
	return -1;
}

Inventory_Items(playerid)
{
    new count;

    for (new i = 0; i != MAX_INVENTORY; i ++) if (invData[playerid][i][invExists]) {
        count++;
	}
	return count;
}

Inventory_Count(playerid, const item[])
{
	new itemid = Inventory_GetItemID(playerid, item);

	if (itemid != -1)
	    return invData[playerid][itemid][invQuantity];

	return 0;
}

stock Inventory_SetQuantity(playerid, const item[], quantity)
{
	new
	    itemid = Inventory_GetItemID(playerid, item),
	    string[128];

	if (itemid != -1)
	{
		mysql_format(Database, string, sizeof(string), "UPDATE `Inventory` SET `invQuantity` = %d WHERE `invOwner` = '%d' AND `invID` = '%d'", quantity, playerData[playerid][ID], invData[playerid][itemid][invID]);
	    mysql_tquery(Database, string);

	    invData[playerid][itemid][invQuantity] = quantity;
	}
	return 1;
}
stock Inventory_HasItem(playerid, const item[])
{
	return (Inventory_GetItemID(playerid, item) != -1);
}
Inventory_Add(playerid, const item[], quantity)
{
	new
		itemid = Inventory_GetItemID(playerid, item),
		string[128];

	if (itemid == -1)
	{
	    itemid = Inventory_GetFreeID(playerid);

	    if (itemid != -1)
	    {
	        invData[playerid][itemid][invExists] = true;
	        //invData[playerid][itemid][invModel] = model;
	        invData[playerid][itemid][invQuantity] = quantity;

	        format(invData[playerid][itemid][invItem], 32, item);

			mysql_format(Database, string, sizeof(string), "INSERT INTO `Inventory` (`invOwner`, `invItem`, `invQuantity`) VALUES ('%d', '%e', '%d')", playerData[playerid][ID], item, quantity);
			mysql_tquery(Database, string, "OnInventoryAdd", "dd", playerid, itemid);

	        return itemid;
		}
		return -1;
	}
	else
	{
	    mysql_format(Database, string, sizeof(string), "UPDATE `Inventory` SET `invQuantity` = `invQuantity` + %d WHERE `invOwner` = '%d' AND `invID` = '%d'", quantity, playerData[playerid][ID], invData[playerid][itemid][invID]);
	    mysql_tquery(Database, string);

	    invData[playerid][itemid][invQuantity] += quantity;
	}
	return itemid;
}

Inventory_Remove(playerid, const item[], quantity = 1)
{
	new
		itemid = Inventory_GetItemID(playerid, item),
		string[128];

	if (itemid != -1)
	{
	    if (invData[playerid][itemid][invQuantity] > 0)
	    {
	        invData[playerid][itemid][invQuantity] -= quantity;
		}
		if (quantity == -1 || invData[playerid][itemid][invQuantity] < 1)
		{
		    invData[playerid][itemid][invExists] = false;
		    invData[playerid][itemid][invQuantity] = 0;

		    mysql_format(Database, string, sizeof(string), "DELETE FROM `Inventory` WHERE `invOwner` = '%d' AND `invID` = '%d'", playerData[playerid][ID], invData[playerid][itemid][invID]);
	        mysql_tquery(Database, string);
		}
		else if (quantity != -1 && invData[playerid][itemid][invQuantity] > 0)
		{
			mysql_format(Database, string, sizeof(string), "UPDATE `Inventory` SET `invQuantity` = `invQuantity` - %d WHERE `invOwner` = '%d' AND `invID` = '%d'", quantity, playerData[playerid][ID], invData[playerid][itemid][invID]);
            mysql_tquery(Database, string);
		}
		return 1;
	}
	return 0;
}

forward Inventory_Load(playerid);
public Inventory_Load(playerid)
{
	static
	    rows;

	cache_get_row_count(rows);

	for (new i = 0; i < rows && i < MAX_INVENTORY; i ++) {
	    invData[playerid][i][invExists] = true;
	    cache_get_value_name_int(i, "invID", invData[playerid][i][invID]);
        cache_get_value_name_int(i, "invQuantity", invData[playerid][i][invQuantity]);

		cache_get_value_name(i, "invItem", invData[playerid][i][invItem], 32);
	}
	return 1;
}


Dialog:DIALOG_INVENTORYMENU(playerid, response, listitem, inputtext[])
{
	if (response)
	{
		switch(listitem)
		{
		    case 0:
		    {
		        OnPlayerUseItem(playerid, invData[playerid][playerData[playerid][ItemSelect]][invItem]);
		    }
		    case 1:
			{
				new string[1000], var[15], count;
				foreach(new i : Player) {
                    
					if (IsPlayerNearPlayer(playerid, i, 5.0))
					{
                        if(i == playerid) continue; // ���������ç��� ��������ͧ价ӧҹ��ǹ���
						format(string, sizeof(string), "[ID: %d]\t%s\n", i, GetPlayerNameEx(i));
						count++;
						format(var, sizeof(var), "ID%d", count);
						SetPVarInt(playerid, var, i);
                        playerData[playerid][ItemOfferID] = i;
					}
				}
				if (!count)
				{
                    Dialog_Show(playerid, Show, DIALOG_STYLE_MSGBOX, "{FF0000}ERROR", "{FFFFFF}�͹�����������������Ѻ�س!", "�Դ", "");
					return 1;
				}
				Dialog_Show(playerid, DIALOG_INVENTORYGIVEID, DIALOG_STYLE_TABLIST, invData[playerid][playerData[playerid][ItemSelect]][invItem], string, "���͡", "�Դ");
		    }
		    case 2:
			{
		        new string[128],
				itemquantity = invData[playerid][playerData[playerid][ItemSelect]][invQuantity];
		        format(string, sizeof(string), "{FFFFFF}� {FF0000}PleaseSelect{FFFFFF} � \n���ӹǹ����ͧ��èз�� �س������ {FF0000}%d", itemquantity);
				Dialog_Show(playerid, DIALOG_INVENTORYDROP, DIALOG_STYLE_INPUT, invData[playerid][playerData[playerid][ItemSelect]][invItem], string, "��ŧ", "�Դ");
			}
		}
	}
	return 1;
}

Dialog:DIALOG_INVENTORYGIVEID(playerid, response, listitem, inputtext[])
{
    if (response)
    {
        new string[256], var[15];
        new itemid = playerData[playerid][ItemSelect];
        format(var, sizeof(var), "PID%d", listitem);
        new id = playerData[playerid][ItemOfferID];
        SendClientMessageEx(playerid, -1, "%s", ReturnPlayerName(id));
        //if(userid == playerid) continue; // ���������ç��� ��������ͧ价ӧҹ��ǹ���
        format(string, sizeof(string), "{FFFFFF}���ӹǹ���س��ͧ��è���� {00FF00}%s {FFFFFF}�Ѻ {33CCFF}%s", invData[playerid][itemid][invItem], GetPlayerNameEx(id));
        Dialog_Show(playerid, DIALOG_INVENTORYGIVE, DIALOG_STYLE_INPUT, invData[playerid][itemid][invItem], string, "��ŧ", "�Դ");
        playerData[playerid][ItemOfferID] = id;
    }
    return 1;
}

Dialog:DIALOG_INVENTORYGIVE(playerid, response, listitem, inputtext[])
{
	if (!response) return 1;
	
	new amount, string[256],
	itemid = playerData[playerid][ItemSelect],
	itemquantity = invData[playerid][itemid][invQuantity],
	userid = playerData[playerid][ItemOfferID],
	count = Inventory_Count(userid, invData[playerid][itemid][invItem])+amount;
			
	if (sscanf(inputtext, "d", amount))
	{
		format(string, sizeof(string), "{FFFFFF}���ӹǹ���س��ͧ��è���� {00FF00}%s{FFFFFF} �Ѻ {33CCFF}%s", invData[playerid][itemid][invItem], GetPlayerNameEx(userid));
		Dialog_Show(playerid, DIALOG_INVENTORYGIVE, DIALOG_STYLE_INPUT, invData[playerid][itemid][invItem], string, "��ŧ", "�Դ");
		return 1;
	}
	if (amount < 1 || amount > 20)
	{
		format(string, sizeof(string), "{FFFFFF}���ӹǹ���س��ͧ��è���� {00FF00}%s{FFFFFF} �Ѻ {33CCFF}%s\n{FF0000}*** �ӹǹ��ͧ����ӡ��� 1 �������Թ 20", invData[playerid][itemid][invItem], GetPlayerNameEx(userid));
		Dialog_Show(playerid, DIALOG_INVENTORYGIVE, DIALOG_STYLE_INPUT, invData[playerid][itemid][invItem], string, "��ŧ", "�Դ");
		return 1;
	}
	if (invData[playerid][itemid][invQuantity] < amount)
	{
		format(string, sizeof(string), "{FFFFFF}���ӹǹ���س��ͧ��è���� {00FF00}%s{FFFFFF} �Ѻ {33CCFF}%s\n{FF0000}*** %s �ͧ�س�������§�ͷ������ {FFFFFF}(%d/%d)", invData[playerid][itemid][invItem], GetPlayerNameEx(userid), invData[playerid][itemid][invItem], amount, itemquantity);
		Dialog_Show(playerid, DIALOG_INVENTORYGIVE, DIALOG_STYLE_INPUT, invData[playerid][itemid][invItem], string, "���͡", "�Դ");
		return 1;
	}
	if (count > playerData[userid][ItemAmount])
	{
		format(string, sizeof(string), "{FFFFFF}���ӹǹ���س��ͧ��è���� {00FF00}%s{FFFFFF} �Ѻ {33CCFF}%s\n{FF0000}*** %s �ͧ�������ʹչ��������� {FFFFFF}(%d/%d)", invData[playerid][itemid][invItem], GetPlayerNameEx(userid), invData[playerid][itemid][invItem], amount, count);
		Dialog_Show(playerid, DIALOG_INVENTORYGIVE, DIALOG_STYLE_INPUT, invData[playerid][itemid][invItem], string, "���͡", "�Դ");
		return 1;
	}
	if (!IsPlayerNearPlayer(playerid, userid, 6.0))
	{
		format(string, sizeof(string), "{FFFFFF}���ӹǹ���س��ͧ��è���� {00FF00}%s{FFFFFF} �Ѻ {33CCFF}%s\n{FF0000}*** �������ʹչ��������������س", invData[playerid][itemid][invItem], GetPlayerNameEx(userid));
		Dialog_Show(playerid, DIALOG_INVENTORYGIVE, DIALOG_STYLE_INPUT, invData[playerid][itemid][invItem], string, "���͡", "�Դ");
		return 1;
	}

	new id = Inventory_Add(playerData[playerid][ItemOfferID], invData[playerid][itemid][invItem], amount);
	if (id == -1)
	{
		format(string, sizeof(string), "{FFFFFF}���ӹǹ���س��ͧ��è���� {00FF00}%s{FFFFFF} �Ѻ {33CCFF}%s\n{FF0000}*** �������ʹչ�������������� {FFFFFF}(%d/%d)", invData[playerid][itemid][invItem], GetPlayerNameEx(userid), Inventory_Items(userid), playerData[userid][MaxItem]);
		Dialog_Show(playerid, DIALOG_INVENTORYGIVE, DIALOG_STYLE_INPUT, invData[playerid][itemid][invItem], string, "���͡", "�Դ");
		return 1;
	}
	Inventory_Remove(playerid, invData[playerid][itemid][invItem], amount);
	SendClientMessageEx(userid, -1, "%s{FFFFFF} ���������� {00FF00}%s{FFFFFF} �ӹǹ {00FF00}%d{FFFFFF} �Ѻ�س", GetPlayerNameEx(playerid), invData[playerid][itemid][invItem], amount);
	SendClientMessageEx(playerid, -1, "�س���������� {00FF00}%s{FFFFFF} �ӹǹ {00FF00}%d{FFFFFF} �Ѻ {33CCFF}%s", invData[playerid][itemid][invItem], amount, GetPlayerNameEx(userid));
	playerData[playerid][ItemOfferID] = INVALID_PLAYER_ID;
	return 1;
}

Dialog:DIALOG_INVENTORYDROP(playerid, response, listitem, inputtext[])
{
	if (response)
	{
	    new amount, string[256],
	    itemid = playerData[playerid][ItemSelect],
		itemquantity = invData[playerid][itemid][invQuantity];
		if (sscanf(inputtext, "d", amount))
		{
	        format(string, sizeof(string), "{FFFFFF}���ӹǹ����ͧ��èз�� �س������ {00FF00}%d", itemquantity);
			Dialog_Show(playerid, DIALOG_INVENTORYDROP, DIALOG_STYLE_INPUT, invData[playerid][itemid][invItem], string, "��ŧ", "�Դ");
	    	return 1;
		}
		if (amount < 1 || amount > 100)
		{
	        format(string, sizeof(string), "{FFFFFF}���ӹǹ����ͧ��èз�� �س������ {00FF00}%d\n{FF0000}*** �ӹǹ��ͧ����ӡ��� 1 �������Թ 100", itemquantity);
			Dialog_Show(playerid, DIALOG_INVENTORYDROP, DIALOG_STYLE_INPUT, invData[playerid][itemid][invItem], string, "��ŧ", "�Դ");
	    	return 1;
		}
		if (invData[playerid][itemid][invQuantity] < amount)
		{
	        format(string, sizeof(string), "{FFFFFF}���ӹǹ����ͧ��èз�� �س������ {00FF00}%d\n{FF0000}*** %s �ͧ�س�������§�ͷ��з�� {FFFFFF}(%d/%d)", itemquantity, invData[playerid][itemid][invItem], amount, itemquantity);
			Dialog_Show(playerid, DIALOG_INVENTORYDROP, DIALOG_STYLE_INPUT, invData[playerid][itemid][invItem], string, "��ŧ", "�Դ");
		    return 1;
		}
		Inventory_Remove(playerid, invData[playerid][itemid][invItem], amount);
		ApplyAnimation(playerid, "GRENADE", "WEAPON_throwu", 3.0, 0, 0, 0, 0, 0);
		SendClientMessageEx(playerid, -1, "�س��������� {00FF00}%s {FFFFFF}�ӹǹ {00FF00}%d {FFFFFF}���", invData[playerid][itemid][invItem], amount);
	}
	return 1;
}

Dialog:DIALOG_INVENTORY(playerid, response, listitem, inputtext[])
{
	if (response)
	{
		new var[32];
		format(var, sizeof(var), "itemlist%d", listitem);
		new item = GetPVarInt(playerid, var);

        OnPlayerClickItem(playerid, item, invData[playerid][item][invItem]);
	}
	return 1;
}

//--> ����������
forward OnPlayerUseItem(playerid, const name[]);
public OnPlayerUseItem(playerid, const name[])
{
	if (!strcmp(name, "�ͻ����", true)) 
    {
         Dialog_Show(playerid, Show, DIALOG_STYLE_MSGBOX, name, "{FFFFFF}������鹹�� {F3FF00}%s{FFFFFF}\n��ö��仢�������Ҵ\n���͹�����ٻ\n���������Ҥ��Թ���", "", "�Դ",name);
	}
	else if (!strcmp(name, "�ԫ���", true))//--> �ԫ���
	{
	    if (playerData[playerid][pHungry] >= 100)
	        return Dialog_Show(playerid, Show, DIALOG_STYLE_MSGBOX, "{FF0000}ERROR", "{FFFFFF}�س������㹢�й��", "�Դ", "");

		playerData[playerid][pHungry] += 60;
		Inventory_Remove(playerid, name);
		SendClientMessage(playerid, -1, "{FF0000}|========= ��ҵͺ᷹ =========|");
	    SendClientMessageEx(playerid, -1, "{FF0000}[FOOD] {FFFFFF}: �س��ҹ %s {FF9B00}+60{FFFFFF} ��������", name);
	}	
	else if (!strcmp(name, "�����������", true))//--> �����������
	{
	    if (playerData[playerid][pHungry] >= 100)
	        return Dialog_Show(playerid, Show, DIALOG_STYLE_MSGBOX, "{FF0000}ERROR", "{FFFFFF}�س������㹢�й��", "�Դ", "");

		playerData[playerid][pHungry] += 40;
		Inventory_Remove(playerid, name);
		SendClientMessage(playerid, -1, "{FF0000}|========= ��ҵͺ᷹ =========|");
	    SendClientMessageEx(playerid, -1, "{FF0000}[FOOD] {FFFFFF}: �س��ҹ %s {FF9B00}+60{FFFFFF} ��������", name);
	}	
	else if (!strcmp(name, "�Һ", true))//--> �Һ
	{
	    if (playerData[playerid][pHungry] >= 100)
	        return Dialog_Show(playerid, Show, DIALOG_STYLE_MSGBOX, "{FF0000}ERROR", "{FFFFFF}�س������㹢�й��", "�Դ", "");

		playerData[playerid][pHungry] += 20;
		Inventory_Remove(playerid, name);
		SendClientMessage(playerid, -1, "{FF0000}|========= ��ҵͺ᷹ =========|");
	    SendClientMessageEx(playerid, -1, "{FF0000}[FOOD] {FFFFFF}: �س��ҹ %s {FF9B00}+60{FFFFFF} ��������", name);
	}	
	else if (!strcmp(name, "�絵����", true))//--> �絵����
	{
	    Dialog_Show(playerid, Show, DIALOG_STYLE_MSGBOX, "{FF0000}�絵����", "{FFFFFF}��㹡�÷ӧҹ�����", "�Դ", "");
	}	
	return 1;
}

forward OnPlayerClickItem(playerid, itemid, const name[]);
public OnPlayerClickItem(playerid, itemid, const name[])
{
	if (!strcmp(name, "�����������", true)) {
        Dialog_Show(playerid, DIALOG_INVENTORYMENU, DIALOG_STYLE_LIST, name, "�Թ\n���\n���", "���͡", "�Դ");
	}
	else if (!strcmp(name, "�Һ", true)) {
        Dialog_Show(playerid, DIALOG_INVENTORYMENU, DIALOG_STYLE_LIST, name, "�Թ\n���\n���", "���͡", "�Դ");
	}
	else if (!strcmp(name, "�ԫ���", true)) {
        Dialog_Show(playerid, DIALOG_INVENTORYMENU, DIALOG_STYLE_LIST, name, "�Թ\n���\n���", "���͡", "�Դ");
	}
	else
	{
		Dialog_Show(playerid, DIALOG_INVENTORYMENU, DIALOG_STYLE_LIST, name, "��\n���\n���", "���͡", "�Դ");
	}
	playerData[playerid][ItemSelect] = itemid;
 	return 1;
}
CMD:itemlist(playerid, params[])
{
    if (playerData[playerid][pAdmin] < 6)
	    return 1;

	static
	    string[1024];

	if (!strlen(string)) {
		for (new i = 0; i < sizeof(itemData); i ++) {
			format(string, sizeof(string), "%s%s\n", string, itemData[i][itemName]);
		}
	}
	return Dialog_Show(playerid, DIALOG_SHOW, DIALOG_STYLE_LIST, "[��ª��������������]", string, "�Դ", "");
}

CMD:giveitem(playerid,params[])
{
    if (playerData[playerid][pAdmin] < 6)
	    return 1;

	new userid, item[32], amount;
	if(sscanf(params, "us[32]d", userid, item, amount)) return SendClientMessage(playerid, COLOR_WHITE, "/giveitem [�ʹ�/����] [���������] [�ӹǹ]");

	new count = Inventory_Count(userid, item)+amount;

	if (count > playerData[userid][ItemAmount])
        return SendClientMessageEx(playerid, COLOR_RED, "[�к�] {FFFFFF}�����آͧ {00FF00}%s{FFFFFF} �ͧ�����蹹��������� (%d/%d)", item, Inventory_Count(userid, item), playerData[userid][ItemAmount]);

	for (new i = 0; i < sizeof(itemData); i ++) if (!strcmp(itemData[i][itemName], item, true))
	{
		new id = Inventory_Add(userid, itemData[i][itemName], amount);

		if (id == -1)
		    return SendClientMessageEx(playerid, COLOR_RED, "[�к�] {FFFFFF}�����آͧ�����������§�� (%d/%d)", Inventory_Items(playerid), playerData[playerid][MaxItem]);

//	    Inventory_Add(userid, itemData[i][itemName], amount);
	    SendAdminMessage(COLOR_ADMIN, "AdmLog: %s ���������� %s �ӹǹ %d �Ѻ %s", GetPlayerNameEx(playerid), item, amount, GetPlayerNameEx(userid));
		SendClientMessageEx(playerid, COLOR_WHITE, "�س���������� {00FF00}%s{FFFFFF} �ӹǹ {00FF00}%d{FFFFFF} �Ѻ {33CCFF}%s", item, amount, GetPlayerNameEx(userid));
		SendClientMessageEx(userid, COLOR_WHITE, "�س���Ѻ����� {00FF00}%s{FFFFFF} �ӹǹ {00FF00}%d{FFFFFF} �ҡ�ʹ�Թ {33CCFF}%s", item, amount, GetPlayerNameEx(userid));
		return 1;
	}
	SendClientMessageEx(playerid, COLOR_RED, "[�к�] {FFFFFF}���������� %s ������к� (����� /itemlist 㹡������ª��������)", item);
	return 1;
}

CMD:setitem(playerid,params[])
{
    if (playerData[playerid][pAdmin] < 6)
	    return 1;

	new userid, item[32], amount;
	if(sscanf(params, "us[32]d", userid, item, amount)) return SendClientMessage(playerid, COLOR_WHITE, "/setitem [�ʹ�/����] [���������] [�ӹǹ]");

	for (new i = 0; i < sizeof(itemData); i ++) if (!strcmp(itemData[i][itemName], item, true))
	{
	    Inventory_Set(userid, itemData[i][itemName], amount);
	    SendAdminMessage(COLOR_ADMIN, "AdmLog: %s ���Ѻ����� %s �ӹǹ %d ���Ѻ %s", GetPlayerNameEx(playerid), item, amount, GetPlayerNameEx(userid));
		SendClientMessageEx(playerid, COLOR_WHITE, "�س���Ѻ����� {00FF00}%s{FFFFFF} �ӹǹ {00FF00}%d{FFFFFF} ���Ѻ {33CCFF}%s", item, amount, GetPlayerNameEx(userid));
		SendClientMessageEx(userid, COLOR_WHITE, "�س�١��Ѻ����� {00FF00}%s{FFFFFF} �ӹǹ {00FF00}%d{FFFFFF} ���ʹ�Թ {33CCFF}%s", item, amount, GetPlayerNameEx(playerid));
		return 1;
	}
	SendClientMessageEx(playerid, COLOR_RED, "[�к�] {FFFFFF}���������� %s ������к� (����� /itemlist 㹡������ª��������)", item);
	return 1;
}

CMD:deleteitem(playerid,params[])
{
    if (playerData[playerid][pAdmin] < 6)
	    return 1;

	new userid, item[32], amount;
	if(sscanf(params, "us[32]d", userid, item, amount)) return SendClientMessage(playerid, COLOR_WHITE, "/deleteitem [�ʹ�/����] [���������] [�ӹǹ]");

	for (new i = 0; i < sizeof(itemData); i ++) if (!strcmp(itemData[i][itemName], item, true))
	{
	    Inventory_Remove(userid, item, amount);

        SendAdminMessage(COLOR_ADMIN, "AdmLog: %s ��ź����� %s �ӹǹ %d ���Ѻ %s", GetPlayerNameEx(playerid), item, amount, GetPlayerNameEx(userid));
		SendClientMessageEx(playerid, COLOR_WHITE, "�سź����� {00FF00}%s{FFFFFF} �ӹǹ {00FF00}%d{FFFFFF} �ͧ {33CCFF}%s", item, amount, GetPlayerNameEx(userid));
		SendClientMessageEx(userid, COLOR_WHITE, "�س�١ź����� {00FF00}%s{FFFFFF} �ӹǹ {00FF00}%d{FFFFFF} ���ʹ�Թ {33CCFF}%s", item, amount, GetPlayerNameEx(playerid));
		return 1;
	}
	SendClientMessageEx(playerid, COLOR_RED, "[�к�] {FFFFFF}���������� %s ������к�", item);
	return 1;
}

CMD:clearitem(playerid,params[])
{
    if (playerData[playerid][pAdmin] < 6)
	    return 1;

	new userid;
	if(sscanf(params, "u", userid)) return SendClientMessage(playerid, COLOR_WHITE, "/clearitem [�ʹ�/����]");

	Inventory_Clear(userid);

    SendAdminMessage(COLOR_ADMIN, "AdmLog: %s ��ź������������ͧ %s", GetPlayerNameEx(playerid), GetPlayerNameEx(userid));
	SendClientMessageEx(playerid, COLOR_WHITE, "�سź������������ͧ {33CCFF}%s", GetPlayerNameEx(userid));
	SendClientMessageEx(userid, COLOR_WHITE, "�س�١ź��������������ʹ�Թ {33CCFF}%s", GetPlayerNameEx(playerid));
	return 1;
}

hook OnPlayerKeyStateChange(playerid, newkeys, oldkeys){

    if(newkeys == KEY_YES){

        OpenItemList(playerid);
    }
    return 1;
}