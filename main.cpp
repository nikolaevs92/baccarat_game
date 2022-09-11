#include <iostream>
#include <vector>
#include <string>


enum class CardRanks: short {
    Ace, Two, Three, Four, Five, Six, Seven, Eight, Nine, Ten, Jack, Queen, King
};

const std::string RANK_TO_STRING[13]{"A", "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K"};
const uint RANK_TO_VALUE[13]{1,2,3,4,5,6,7,8,9,0,0,0,0};

enum class GameOutcome: short {
    Player, Tie, Banker
};

const uint RANKS_NUMBER = 13;
const uint SUIT_NUMBER = 4;

class virtualRNG {
public:
    virtual ~virtualRNG() {}
    virtual uint GetRand(uint max) = 0;
};

class RNG : public virtualRNG {
public:
    uint GetRand(uint max) override;
};

class NoSuiteDeckShoe {
private:
    virtualRNG *Random;
    uint DeckNumber;
    uint CardsInShoe;
    std::vector<uint> ShoeCards;
public:
    NoSuiteDeckShoe(virtualRNG *rng, uint deckNumber=1) : 
        Random(rng), DeckNumber(deckNumber), CardsInShoe(RANKS_NUMBER*SUIT_NUMBER*deckNumber), ShoeCards(RANKS_NUMBER, SUIT_NUMBER*deckNumber) {}

    void Shuffle();
    CardRanks DrawCard();
};

class Baccarat {
private:
    NoSuiteDeckShoe Shoe;
    const uint RANK_TO_VALUE[13]{1,2,3,4,5,6,7,8,9,0,0,0,0};

    uint GetValue(CardRanks c);
public:
    struct GameResult {
        struct Hand {
            CardRanks First;
            CardRanks Second;
            CardRanks Third;
            bool IsThird;

            Hand() {}
            Hand(CardRanks first, CardRanks second) : First(first), Second(second), IsThird(false) {}
            Hand(CardRanks first, CardRanks second, CardRanks third): First(first), Second(second), Third(third), IsThird(false) {}
        };
        Hand Banker;
        Hand Player;
        GameOutcome Outcome;

        GameResult() {}
        GameResult(Hand banker, Hand player) : Banker(banker), Player(player) {}
        GameResult(Hand banker, Hand player, GameOutcome outcome) : Banker(banker), Player(player), Outcome(outcome) {}
    };

    Baccarat(NoSuiteDeckShoe shoe) : Shoe(shoe) {}
    Baccarat(virtualRNG *rng, uint deckNum) : Shoe{rng, deckNum} {}

    // TODO: should be pointer?
    GameResult RunOne();
};

std::ostream& operator<<(std::ostream &os, const CardRanks& c); 
std::ostream& operator<<(std::ostream &os, const GameOutcome& g); 
std::ostream& operator<<(std::ostream &os, const Baccarat::GameResult::Hand& h);
std::ostream& operator<<(std::ostream &os, const Baccarat::GameResult& r); 

int main() {
    RNG random;
    Baccarat baccarat(&random, 8);
    std::cout << baccarat.RunOne() << std::endl;
    return 0;
}

uint RNG::GetRand(uint max) {
    //max should be less than RandMax 
    if (max>= RAND_MAX) {
        return rand();
   }
    if (max == 0){
        return 0;
    }

    uint r = rand();
    while (r/max == RAND_MAX / max) {
        r = rand();
    }
    return r % max;
}

void NoSuiteDeckShoe::Shuffle() {
    for (uint i = 0; i < RANKS_NUMBER; i++){
        ShoeCards[i] = SUIT_NUMBER*DeckNumber;
    }
    CardsInShoe = RANKS_NUMBER * SUIT_NUMBER * DeckNumber;
}

CardRanks NoSuiteDeckShoe::DrawCard() {
    uint c = Random->GetRand(CardsInShoe);
    CardsInShoe--;

    for (uint i=0; i < RANKS_NUMBER; i++) {
        if (c < ShoeCards[i]) {
            return CardRanks(i);
        }
        c -= ShoeCards[i];
    }
    //todo: remove
    return CardRanks(0);
}


uint Baccarat::GetValue(CardRanks c) {
    return RANK_TO_VALUE[static_cast<typename std::underlying_type<CardRanks>::type>(c)];
}

Baccarat::GameResult Baccarat::RunOne() {
    Shoe.Shuffle();
    Baccarat::GameResult res{{Shoe.DrawCard(), Shoe.DrawCard()}, {Shoe.DrawCard(), Shoe.DrawCard()}};

    uint playerSum = (GetValue(res.Player.First) + GetValue(res.Player.Second)) % 10;
    uint bankerSum = (GetValue(res.Banker.First) + GetValue(res.Banker.Second)) % 10;

    // natural win
    // TODO: maybe move this calculation to Baccarat::GameResult struct
    if (playerSum > 7 && playerSum > bankerSum) {
        res.Outcome = GameOutcome::Player;
        return res;
    }
    if (playerSum > 7 && playerSum == bankerSum) {
        res.Outcome = GameOutcome::Tie;
        return res;
    }
    if (bankerSum > 7) {
        res.Outcome = GameOutcome::Banker;
        return res;
    }

    // player draw third
    if (playerSum < 6) {
        // TODO: maybe move this calculation to Baccarat::GameResult::Hand struct
        res.Player.Third = Shoe.DrawCard();
        res.Player.IsThird = true;
        playerSum = (playerSum + GetValue(res.Player.Third)) % 10;
    }

    // player didn't draw third, baker draw third
    if (!res.Player.IsThird && bankerSum < 6) {
        res.Banker.Third = Shoe.DrawCard();
        res.Banker.IsThird = true;
        bankerSum = (bankerSum + GetValue(res.Banker.Third)) % 10;
    }
    //  player drew third, banker draw third
    if (res.Player.IsThird && (
            (bankerSum <= 2)
            || (bankerSum == 3 && res.Player.Third == CardRanks::Eight)
            || (bankerSum == 4 && res.Player.Third < CardRanks::Eight && res.Player.Third > CardRanks::Ace)
            || (bankerSum == 5 && res.Player.Third < CardRanks::Eight && res.Player.Third > CardRanks::Three)
            || (bankerSum == 6 && res.Player.Third < CardRanks::Eight && res.Player.Third > CardRanks::Five)
    )) {
        res.Banker.Third = Shoe.DrawCard();
        res.Banker.IsThird = true;
        bankerSum = (bankerSum + GetValue(res.Banker.Third)) % 10;
    }
    
    if (playerSum > bankerSum) {
        res.Outcome = GameOutcome::Player;
        return res;
    }
    if (playerSum < bankerSum) {
        res.Outcome = GameOutcome::Banker;
        return res;
    }
    res.Outcome = GameOutcome::Tie;
    return res;
}

std::ostream& operator<<(std::ostream &os, const CardRanks& c) {
    return os  << RANK_TO_STRING[static_cast<typename std::underlying_type<CardRanks>::type>(c)];
}
std::ostream& operator<<(std::ostream &os, const GameOutcome& g) {
    switch (g)
    {
    case GameOutcome::Player:
        return os  << "PLAYER";
        break;
    case GameOutcome::Tie:
        return os  << "TIE";
        break;
    case GameOutcome::Banker:
        return os  << "BANKER";
        break;
    
    default:
        //TODO: exception
        break;
    }
    return os;
}

std::ostream& operator<<(std::ostream &os, const Baccarat::GameResult::Hand& h){
    os << h.First << ',' << h.Second;
    if (h.IsThird) {
        os << ',' << h.Third;
    }
    return os;
}

std::ostream& operator<<(std::ostream &os, const Baccarat::GameResult& r){
    return os << r.Player << " - " << r.Banker << " - " << r.Outcome;
}

