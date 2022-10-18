/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU AGPL v3 license: https://github.com/azerothcore/azerothcore-wotlk/blob/master/LICENSE-AGPL3
 */

#include "ScriptMgr.h"
#include "Player.h"
#include "Config.h"
#include "Chat.h"
#include "GossipDef.h"
#include "ScriptedGossip.h"

enum Specs
{
    ALCH_ELIXIR = 28677,
    ALCH_POTION = 28675,
    ALCH_TRANSMUTE = 28672,
    BSMITH_ARMOR = 9788,
    BSMITH_WEAPON = 9787,
    BSMITH_SWORD = 17039,
    BSMITH_AXE = 17041,
    BSMITH_HAMMER = 17040,
    ENG_GOBLIN = 20219,
    ENG_GNOME = 20222,
    LEATHER_DRAGON = 10656,
    LEATHER_ELEMENT = 10658,
    LEATHER_TRIBAL = 10660,
    TAILOR_MOON = 26798,
    TAILOR_SPELL = 26797,
    TAILOR_SHADOW = 26801
};

bool EnableAlch, EnableBSmith, EnableEng, EnableLeather, EnableTailor, EnableCost;
int speccost;


class ProfSpec : public CreatureScript
{
public:
    ProfSpec() : CreatureScript("npc_profession_specializations") { }

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        if (!sConfigMgr->GetOption<bool>("ProfSpec.Enable", true))
        {
            return false;
        }

        player->PlayerTalkClass->ClearMenus();

        if ((player->HasSkill(SKILL_ALCHEMY) && EnableAlch))
        {
            AddGossipItemFor(player, GOSSIP_ICON_TRAINER, "Alchemy", GOSSIP_SENDER_MAIN, SKILL_ALCHEMY);
        }
        if ((player->HasSkill(SKILL_BLACKSMITHING) && EnableBSmith))
        {
            AddGossipItemFor(player, GOSSIP_ICON_TRAINER, "Blacksmithing", GOSSIP_SENDER_MAIN, SKILL_BLACKSMITHING);
        }
        if ((player->HasSkill(SKILL_ENGINEERING) && EnableEng))
        {
            AddGossipItemFor(player, GOSSIP_ICON_TRAINER, "Engineering", GOSSIP_SENDER_MAIN, SKILL_ENGINEERING);
        }
        if ((player->HasSkill(SKILL_LEATHERWORKING) && EnableLeather))
        {
            AddGossipItemFor(player, GOSSIP_ICON_TRAINER, "Leatherworking", GOSSIP_SENDER_MAIN, SKILL_LEATHERWORKING);
        }
        if ((player->HasSkill(SKILL_TAILORING) && EnableTailor))
        {
            AddGossipItemFor(player, GOSSIP_ICON_TRAINER, "Tailoring", GOSSIP_SENDER_MAIN, SKILL_TAILORING);
        }


        SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 Sender, uint32 SKILL)
    {
        player->PlayerTalkClass->ClearMenus();

        if (Sender == GOSSIP_SENDER_MAIN)
        {
            switch (SKILL)
            {
            case SKILL_ALCHEMY:
                AddGossipItemFor(player, GOSSIP_ICON_TRAINER, "Elixir Master", GOSSIP_SENDER_INFO, ALCH_ELIXIR);
                AddGossipItemFor(player, GOSSIP_ICON_TRAINER, "Potion Master", GOSSIP_SENDER_INFO, ALCH_POTION);
                AddGossipItemFor(player, GOSSIP_ICON_TRAINER, "Transmute Master", GOSSIP_SENDER_INFO, ALCH_TRANSMUTE);
                break;

            case SKILL_BLACKSMITHING:
                AddGossipItemFor(player, GOSSIP_ICON_TRAINER, "Armorsmith", GOSSIP_SENDER_INFO, BSMITH_ARMOR);
                if (player->HasSpell(BSMITH_WEAPON))
                {
                    AddGossipItemFor(player, GOSSIP_ICON_TRAINER, "Master Swordsmith", GOSSIP_SENDER_INFO, BSMITH_SWORD);
                    AddGossipItemFor(player, GOSSIP_ICON_TRAINER, "Master Axesmith", GOSSIP_SENDER_INFO, BSMITH_AXE);
                    AddGossipItemFor(player, GOSSIP_ICON_TRAINER, "Master Hammersmith", GOSSIP_SENDER_INFO, BSMITH_HAMMER);
                }
                else
                {
                    AddGossipItemFor(player, GOSSIP_ICON_TRAINER, "Weaponsmith", GOSSIP_SENDER_INFO, BSMITH_WEAPON);
                }
                break;

            case SKILL_ENGINEERING:
                AddGossipItemFor(player, GOSSIP_ICON_TRAINER, "Gnomish Engineering", GOSSIP_SENDER_INFO, ENG_GNOME);
                AddGossipItemFor(player, GOSSIP_ICON_TRAINER, "Goblin Engineering", GOSSIP_SENDER_INFO, ENG_GOBLIN);
                break;

            case SKILL_LEATHERWORKING:
                AddGossipItemFor(player, GOSSIP_ICON_TRAINER, "Dragonscale", GOSSIP_SENDER_INFO, LEATHER_DRAGON);
                AddGossipItemFor(player, GOSSIP_ICON_TRAINER, "Elemental", GOSSIP_SENDER_INFO, LEATHER_ELEMENT);
                AddGossipItemFor(player, GOSSIP_ICON_TRAINER, "Tribal", GOSSIP_SENDER_INFO, LEATHER_TRIBAL);
                break;

            case SKILL_TAILORING:
                AddGossipItemFor(player, GOSSIP_ICON_TRAINER, "Shadoweave", GOSSIP_SENDER_INFO, TAILOR_SHADOW);
                AddGossipItemFor(player, GOSSIP_ICON_TRAINER, "Mooncloth", GOSSIP_SENDER_INFO, TAILOR_MOON);
                AddGossipItemFor(player, GOSSIP_ICON_TRAINER, "Spellfire", GOSSIP_SENDER_INFO, TAILOR_SPELL);
                break;
            }

            SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
            return true;
        }

        if (Sender == GOSSIP_SENDER_INFO)
        {
            if (player->HasSkill(SKILL))
            {
                player->GetSession()->SendNotification("You already have this Specialization.");
            }
            else if (!EnableCost)
            {
                player->learnSpell(SKILL);
            }
            else
            {
                if (player->GetMoney() >= speccost)
                {
                    player->learnSpell(SKILL);
                    player->ModifyMoney(-speccost);
                }
                
            }

            CloseGossipMenuFor(player);
            return true;
        }

    }



};

class ProfSpecConf : public WorldScript
{
public:
    ProfSpecConf() : WorldScript("ProfSpecConf") {}

    void OnBeforeConfigLoad(bool) override
    {

        EnableAlch = sConfigMgr->GetOption<bool>("ProfSpec.Alchemy", true);
        EnableBSmith = sConfigMgr->GetOption<bool>("ProfSpec.Blacksmithing", true);
        EnableEng = sConfigMgr->GetOption<bool>("ProfSpec.Engineering", true);
        EnableLeather = sConfigMgr->GetOption<bool>("ProfSpec.Leatherworking", true);
        EnableTailor = sConfigMgr->GetOption<bool>("ProfSpec.Tailoring", true);
        EnableCost = sConfigMgr->GetOption<bool>("ProfSpec.CostEnabled", false);
        speccost = sConfigMgr->GetOption<int32>("ProfSpec.Cost", 10000);
    }
};



// Add all scripts in one
void AddProfSpec()
{
    new ProfSpec();
    new ProfSpecConf();
}
