#pragma once
#ifndef __PLAYER_PROFILE_HEADER__
#define __PLAYER_PROFILE_HEADER__

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/object.hpp>
using namespace godot;

#include "config_settings.h"
#include <map>
#include <vector>


class NamedStatistics GDX_SUBCLASS(Node)
{
    GDX_CLASS_PREFIX(NamedStatistics, Node);

public:
    NamedStatistics() : items_() {}
    virtual ~NamedStatistics() {}

    Dictionary getItems() const;
    void setItems(Dictionary items);
    void appendItems(Dictionary items);

private:
    std::map<std::string, std::string> items_;
};


class PlayerProfile GDX_SUBCLASS(Node)
{
    GDX_CLASS_PREFIX(PlayerProfile, Node);

public:
    PlayerProfile();
    virtual ~PlayerProfile();

    NamedStatistics* getStatistics() const { return statistics_; }
    ConfigItems* getSettings() { return gameplaySettings_; }

    void setPlayerId(const String newId) { playerId_ = newId; }
    String getPlayerId();
    void setPlayerName(const String newName) { playerName_ = newName; }
    String getPlayerName() const { return playerName_; }
    void setPortraitFile(const String filename) { portraitFile_ = filename; }
    String getPortraitFile() const { return portraitFile_; }

    void setUseSettings(bool state) { useSettings_ = state; }
    bool getUseSettings() const { return useSettings_; }

private:
    String playerId_;
    String playerName_;
    String portraitFile_;
    bool useSettings_;
    
    NamedStatistics* statistics_;
    ConfigItems* gameplaySettings_;
};

#endif /// __PLAYER_PROFILE_HEADER__
