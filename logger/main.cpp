#include <vector>
#include <random>
#include <map>


typedef unsigned int idT;
typedef unsigned int moneyT;

// TODO: Maybe GablingWatcher should give gameId
class GablingWatcher {
    virtual void StartGame(idT gameId);
    virtual void WagerInform(moneyT wager, idT gameId);
    virtual void WinInform(moneyT win, idT gameId);
    virtual void EndtGame(idT gameId);
};

struct GameInfo {
    idT ID;
    moneyT Wager;
    moneyT Win;
    GameInfo(idT id) : ID(id), Wager(0), Win(0) {}
    GameInfo(idT id, moneyT wager, moneyT win) : ID(id), Wager(wager), Win(win) {}
};

class GamesLogKeeper {
private:
    std::vector<GameInfo> Games;
public:
    void LogGame(const GameInfo &game) {
        Games.push_back(game);
    }
};

class GablingLogger: public GablingWatcher {
private:
    GamesLogKeeper Keeper;
    std::map<idT, GameInfo> CurrentGames;
public:
    GablingLogger(GamesLogKeeper keeper) : Keeper(keeper) {}

    void StartGame(idT gameId) override {
        //todo: check gameId
        CurrentGames[gameId] = GameInfo{gameId};
    }

    void WagerInform(moneyT wager, idT gameId) override {
        CurrentGames[gameId].Wager += wager;
    }

    void WinInform(moneyT win, idT gameId) override {
        CurrentGames[gameId].Win += win;
    }

    void EndtGame(idT gameId) override {
        Keeper.LogGame(CurrentGames[gameId]);
        CurrentGames.erase(gameId);
    }
};

int maint() {
    return 0;
}