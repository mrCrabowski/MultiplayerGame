#include "game.h"
#include "../Common/protocol.pb.h"

using namespace game;

Game::Game() : map_(20, 20)
{
    restart(2);
}

Game::~Game()
{
}

void Game::restart(const size_t playerCount)
{
    playerCount_ = playerCount;
    activePlayer_ = 0;
    currentPlayerCount_ = 0;
    units_.reset();
}

bool Game::consumeCommand(size_t player, const Command& command)
{
    if (player != activePlayer_)
        return false;

    units_.setUser(activePlayer_);

    bool gameChanged = false;

    if (command.has_end_turn()) {
        gameChanged = applyCommand(command.end_turn());
    }
    else if (command.has_spawn()) {
        gameChanged = applyCommand(command.spawn());
    }

    if (!gameChanged)
        return false;

    removeDeadUnits();
    return gameChanged;
}

size_t Game::activePlayer() const
{
    return activePlayer_;
}

void Game::addPlayer()
{
    if (started())
        return;

    // Create starting units for player
    units_.setUser(currentPlayerCount_);
    units_.create(UnitType::WORKER, { static_cast<int>(currentPlayerCount_), 0 });

    currentPlayerCount_++;
}

bool Game::started()
{
    return currentPlayerCount_ == playerCount_;
}

GameState Game::getState() const
{
    GameState state;

    state.set_active_player(activePlayer());
    state.set_player(-1);   // Invalid player, should be reset by server for each client
    

    for (auto it = units_.begin(); it != units_.end(); ++it) {
        const auto id = it->first;
        auto unit = it->second;
        Unit data;
        data.set_type(unit->getType());
        data.set_health(unit->getHealth());
        data.set_player(unit->owner());
        auto pos = new Position();
        pos->set_x(unit->getCoords().x);
        pos->set_y(unit->getCoords().y);
        data.set_allocated_position(pos);
        (*state.mutable_units())[id] = data;
    }
    return state;
}

bool Game::applyCommand(const Move& moveCommand)
{
    return false;
}

bool Game::applyCommand(const Spawn& spawnCommand)
{
    return false;
}

bool Game::applyCommand(const EndTurn& endCommand)
{
    activePlayer_++;
    if (activePlayer_ >= playerCount_)
        activePlayer_ = 0;

    return true;
}

void Game::removeDeadUnits()
{
    for (auto it = units_.begin(); it != units_.end();)
    {
        if (it->second->getHealth() <= 0)
            it = units_.erase(it);
        else
            ++it;
    }
}