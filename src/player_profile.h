#pragma once
#ifndef __PLAYER_PROFILE_HEADER__
#define __PLAYER_PROFILE_HEADER__

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/object.hpp>
using namespace godot;

#include <type_traits>
#include <map>
#include <string>
#include <vector>

#include "config_settings.h"

class NamedStatistic GDX_SUBCLASS(Node)
{
    GDX_CLASS_PREFIX(NamedStatistic, Node);

public:
    NamedStatistic();
    virtual ~NamedStatistic() {};

    void initialize(String stringId, const double initialValue);

    String getStringId() const { return stringId_; }
    void setStringId(String stringId) { stringId_ = stringId; }

    double getValue() const { return value_; }
    void setValue(double newValue) { value_ = newValue; }
    void addValue(double newValue) { value_ += newValue; }

private:
    String stringId_;
    double value_;
};


class PlayerProfile GDX_SUBCLASS(Node)
{
    GDX_CLASS_PREFIX(PlayerProfile, Node);

public:
    PlayerProfile();
    virtual ~PlayerProfile();

    // returns array of named statistics
    Array getStatistics();
    // returns a specific statistic.  will create it if it doesn't exist.
    NamedStatistic* getStatistic(String stringId);
    
    void setPlayerId(const String newId) { playerId_ = newId; }
    String getPlayerId() const { return playerId_; }
    void setPlayerName(const String newName) { playerName_ = newName; }
    String getPlayerName() const { return playerName_; }
    void setPortraitFile(const String filename) { portraitFile_ = filename; }
    String getPortraitFile() const { return portraitFile_; }

    void setUseSettings(bool state) { useSettings_ = state; }
    bool getUseSettings() const { return useSettings_; }

    ConfigSettings* getSettings() { return gameplaySettings_; }

    PlayerProfile* fromFile(const String filename);
    bool toFile(const String filename);

private:
    String playerId_;
    String playerName_;
    String portraitFile_;
    bool useSettings_;
    std::vector<NamedStatistic*> statistics_;
    ConfigSettings* gameplaySettings_;
};



#endif /// __PLAYER_PROFILE_HEADER__
