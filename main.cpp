#include <iostream>
#include <vector>
#include <list>
#include <assert.h>
#include <cstdio>
#include <sstream>

#define BOARD_SIZE 9
#define PENTA 5
#define MINI_SQUARE_SIZE 3
#define NUMBER_OF_MINI_SQUARES BOARD_SIZE / MINI_SQUARE_SIZE
typedef int PlayerNumber; // should be signed char

#define NoPlayer -1

using namespace std;

class PentagoBoard;


struct Position {
    int x, y;
    Position(int x, int y): x(x), y(y) {}
};

enum RotationDirection { CLOCKWISE, COUNTERCLOCKWISE };

RotationDirection fromBool(bool dir) {
    return dir ? CLOCKWISE : COUNTERCLOCKWISE;
}

RotationDirection swapDir(RotationDirection dir) {
    return fromBool(dir == COUNTERCLOCKWISE);
}

struct Rotation {
    int x, y;
    RotationDirection dir;
    Rotation(int x, int y, RotationDirection dir): x(x), y(y), dir(dir) {};
};

struct Move {
    Position position;
    Rotation rotation;
    Move(Position pos, Rotation rot): position(pos), rotation(rot) {};
};

void printMove(Move move) {
    printf("%d %d %d %d %c\n", move.position.x, move.position.y, move.rotation.x, move.rotation.y,
           move.rotation.dir == CLOCKWISE ? 'r' : 'l');
}

class Player;

class PentagoBoard {
    PlayerNumber board[BOARD_SIZE][BOARD_SIZE];

    int playedPieces;
public:
    int playerToMoveNext;
    int numberOfPlayers;

    PentagoBoard(int numberOfPlayers): numberOfPlayers(numberOfPlayers) {
        for (int x = 0; x < BOARD_SIZE; x++) {
            for (int y = 0; y < BOARD_SIZE; y++) {
                setAtPosition(x, y, NoPlayer);
            }
        }
        playedPieces = 0;
    }

    PentagoBoard(const PentagoBoard& other) {
        for (int x = 0; x < BOARD_SIZE; x++) {
            for (int y = 0; y < BOARD_SIZE; y++) {
                setAtPosition(x, y, other.getAtPosition(x, y));
            }
        }
        playedPieces = other.playedPieces;
        playerToMoveNext = other.playerToMoveNext;
        numberOfPlayers = other.numberOfPlayers;
    };

    PlayerNumber getAtPosition (int x, int y) const {
        return board[y][x];
    }

    PlayerNumber getAtPosition(Position pos) const {
        return board[pos.y][pos.x];
    }

    void playWhoeverAtPosition(Position pos, PlayerNumber n) {
        assert(getAtPosition(pos) == NoPlayer);
        if (n != NoPlayer) {
            playedPieces++;
            setAtPosition(pos, n);
        }
    }

    void playAtPosition(Position pos) {
        playedPieces ++;
        setAtPosition(pos, playerToMoveNext);
        playerToMoveNext = (playerToMoveNext + 1) % numberOfPlayers;
    }

    void unplayAtPosition(Position pos, PlayerNumber playerNumber) {
        playedPieces --;
        if (playerNumber != getAtPosition(pos)) {
            assert(playerNumber == getAtPosition(pos));
        }

        setAtPosition(pos, NoPlayer);
        playerToMoveNext = playerNumber;
    }

    void rotate(Rotation rotation) {
        if (rotation.dir == CLOCKWISE) {
            rotateSquareClockwise(rotation.x, rotation.y);
        } else {
            rotateSquareClockwise(rotation.x, rotation.y);
            rotateSquareClockwise(rotation.x, rotation.y);
            rotateSquareClockwise(rotation.x, rotation.y);
        }
    }

    void doMove(Move move, PlayerNumber playerNumber) {
        assert(playerToMoveNext == playerNumber);
        assert(getAtPosition(move.position) == NoPlayer);
        playAtPosition(move.position);
        rotate(move.rotation);
    }

    void undoMove(Move move, PlayerNumber playerNumber) {
        rotate(Rotation(move.rotation.x, move.rotation.y, swapDir(move.rotation.dir)));
        unplayAtPosition(move.position, playerNumber);
    }

    void print() const {
        for (int y = 0; y < BOARD_SIZE; y++) {
            for (int x = 0; x < BOARD_SIZE; x++) {
                PlayerNumber atPos = getAtPosition(x, y);
                if (atPos == NoPlayer) {
                    cout << ".";
                } else {
                    printf("%d", atPos);
                }

                if (x % MINI_SQUARE_SIZE == 2) cout << " ";
            }
            cout << endl;
            if (y % MINI_SQUARE_SIZE == 2) cout << endl;
        }
        cout << endl;
    }

    PlayerNumber getWinner() const {
        // vertical wins
        for (int y = 0; y < BOARD_SIZE; y++) {
            PlayerNumber winner = checkLine(0, y, 1, 0);
            if (winner != NoPlayer) {
                return winner;
            }
        }

        // horizontal wins
        for (int x = 0; x < BOARD_SIZE; x++) {
            PlayerNumber winner = checkLine(x, 0, 0, 1);
            if (winner != NoPlayer) {
                return winner;
            }
        }

        // diagonals
        for (int start= 0; start < BOARD_SIZE; start++) {
            PlayerNumber winner = checkLine(start, 0, 1, 1);
            if (winner != NoPlayer) {
                return winner;
            }

            winner = checkLine(start, 0, 1, -1);
            if (winner != NoPlayer) {
                return winner;
            }

            winner = checkLine(0, start, 1, 1);
            if (winner != NoPlayer) {
                return winner;
            }

            winner = checkLine(0, start, 1, -1);
            if (winner != NoPlayer) {
                return winner;
            }
        }

        return NoPlayer;
    }

    bool isDraw() const {
        return playedPieces == BOARD_SIZE * BOARD_SIZE;
    }

    std::vector<Move> getMoves() const {
        std::vector<Move> moves;

        for (int x = 0; x < BOARD_SIZE; x++) {
            for (int y = 0; y < BOARD_SIZE; y++) {
                if (getAtPosition(x, y) != NoPlayer) {
                    continue;
                }

                for (int dir = 0; dir < 2; dir ++) {
                    for (int rotationX = 0; rotationX < NUMBER_OF_MINI_SQUARES; rotationX++) {
                        for (int rotationY = 0; rotationY < NUMBER_OF_MINI_SQUARES; rotationY++) {
                            moves.push_back(Move(Position(x, y), Rotation(rotationX, rotationY, fromBool(dir == 0))));
                        }
                    }
                }
            }
        }

        return moves;
    }

private:
    PlayerNumber checkLine(int startX, int startY, int deltaX, int deltaY) const {
        int endX = startX + deltaX * (PENTA - 1);
        if (endX < 0 || endX > BOARD_SIZE - 1) {
            return NoPlayer;
        }

        int endY = startY + deltaY * (PENTA - 1);
        if (endY < 0 || endY > BOARD_SIZE - 1) {
            return NoPlayer;
        }

        int x = startX;
        int y = startY;

        PlayerNumber prev = NoPlayer;
        char count = 0;

        while (x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE) {
            PlayerNumber piece = getAtPosition(x, y);
            if (piece == prev && piece != NoPlayer) {
                count++;
                if (count == PENTA) {
                    return piece;
                }
            } else {
                prev = piece;
                count = 1;
            }

            x += deltaX;
            y += deltaY;
        }

        return NoPlayer;
    }

    void rotateSquareClockwise(int x, int y) {
        int offsetX = x * MINI_SQUARE_SIZE;
        int offsetY = y * MINI_SQUARE_SIZE;

        PlayerNumber temp = getAtPosition(offsetX, offsetY);
        setAtPosition(offsetX, offsetY, getAtPosition(offsetX, offsetY + 2));
        setAtPosition(offsetX, offsetY + 2, getAtPosition(offsetX + 2, offsetY + 2));
        setAtPosition(offsetX + 2, offsetY + 2, getAtPosition(offsetX + 2, offsetY));
        setAtPosition(offsetX + 2, offsetY, temp);

        temp = getAtPosition(offsetX + 1, offsetY);
        setAtPosition(offsetX + 1, offsetY, getAtPosition(offsetX, offsetY + 1));
        setAtPosition(offsetX, offsetY + 1, getAtPosition(offsetX + 1, offsetY + 2));
        setAtPosition(offsetX + 1, offsetY + 2, getAtPosition(offsetX + 2, offsetY + 1));
        setAtPosition(offsetX + 2, offsetY + 1, temp);
    }

    void setAtPosition(int x, int y, PlayerNumber playerNumber) {
        board[y][x] = playerNumber;
    }

    void setAtPosition(Position pos, PlayerNumber playerNumber) {
        setAtPosition(pos.x, pos.y, playerNumber);
    }
};

class Player {
public:
    virtual Move getMove(const PentagoBoard *board) = 0;
    virtual string getName() = 0;
};

class HumanPlayer: public Player {
    string name;
public:
    HumanPlayer(string name): name(name) {}

    Move getMove(const PentagoBoard *board) {
        int x, y, turnX, turnY;
        char turndir;
        printf("give me your x, y, turndir, turnx, turny: ");
        scanf("%d %d %c %d %d", &x, &y, &turndir, &turnX, &turnY);

        if (board->getAtPosition(x, y) == NoPlayer) {
            printf("\ncoolio!\n\n");
            return Move(Position(x, y), Rotation(turnX, turnY, turndir == 'l' ? COUNTERCLOCKWISE : CLOCKWISE));
        } else {
            printf("That position is already taken! Give me an empty position please.\n");
            return getMove(board);
        }
    }

    string getName() {
        return name;
    }
};

Move getRandomMove(const PentagoBoard *board) {
    int x = rand() % BOARD_SIZE;
    int y = rand() % BOARD_SIZE;

    if (board->getAtPosition(x, y) != NoPlayer) {
        return getRandomMove(board);
    }

    return Move(
            Position(x, y),
            Rotation(rand() % NUMBER_OF_MINI_SQUARES,
                     rand() % NUMBER_OF_MINI_SQUARES,
                     rand() % 2 == 0 ? CLOCKWISE : COUNTERCLOCKWISE));
}

class RandomPlayer: public Player {
    string name;

public:
    RandomPlayer(string name): name(name) {}

    string getName() { return name; }

    Move getMove(const PentagoBoard *board) {
        return getRandomMove(board);
    }
};

Move *getWinningMoveIfExists(const PentagoBoard *board) {
    PentagoBoard *copiedBoard = new PentagoBoard(*board);
    int myPlayerNumber = board->playerToMoveNext;

    for (auto move: copiedBoard->getMoves()) {
        copiedBoard->doMove(move, myPlayerNumber);
        if (copiedBoard->getWinner() == myPlayerNumber) {
            return new Move(move);
        }
        copiedBoard->undoMove(move, myPlayerNumber);
    }

    return nullptr;
}

class AlmostRandomPlayer: public Player {
    string name;
public:
    AlmostRandomPlayer(string name): name(name) {}

    string getName() { return name; }

    Move getMove(const PentagoBoard *board) {
        Move * move = getWinningMoveIfExists(board);
        if (move == nullptr) {
            return getRandomMove(board);
        } else {
            return *move;
        }
    }
};

class HeuristicPlayer: public Player {
    string name;
public:
    HeuristicPlayer(string name): name(name) {}

    string getName() { return name; }

    Move getMove(const PentagoBoard *board) {
        Move * move = getWinningMoveIfExists(board);
        if (move == nullptr) {
            return getHeuristicMove(board);
        } else {
            return *move;
        }
    }

    Move getHeuristicMove(const PentagoBoard *board) {
        float scores[BOARD_SIZE][BOARD_SIZE] = {0.0};

        for (int squareX = 0; squareX < NUMBER_OF_MINI_SQUARES; squareX++) {
            for (int squareY = 0; squareY < NUMBER_OF_MINI_SQUARES; squareY++) {
                getScoresForMiniSquare(board, squareX, squareY, scores);
            }
        }

        Position bestPosition = getRandomMove(board).position;
        float bestScore = 0;

        // print the heuristic function:
//        for (int y = 0; y < BOARD_SIZE; y++) {
//            for (int x = 0; x < BOARD_SIZE; x++) {
//                printf("%f ", scores[y][x]);
//            }
//            printf("\n");
//        }

        for (int x = 0; x < BOARD_SIZE; x++) {
            for (int y = 0; y < BOARD_SIZE; y++) {
                if (board->getAtPosition(x, y) != NoPlayer) {
                    assert(scores[y][x] == 0.0);
                }

                if (scores[y][x] > bestScore) {
                    bestPosition = Position(x, y);
                    bestScore = scores[y][x];
                }
            }
        }

        return Move(bestPosition, getRandomMove(board).rotation);
    }

    void getScoresForMiniSquare(const PentagoBoard *board, int squareX, int squareY, float scores[BOARD_SIZE][BOARD_SIZE]) {
        assert(MINI_SQUARE_SIZE == 3);

        int offsetX = squareX * MINI_SQUARE_SIZE;
        int offsetY = squareY * MINI_SQUARE_SIZE;

        // vertical
        for (int x = offsetX; x < offsetX + 3; x++) {
            getScoresForThree(board, x, offsetY, x, offsetY + 1, x, offsetY + 2, scores);
        }

        // horizontal
        for (int y = offsetY; y < offsetY + 3; y++) {
            getScoresForThree(board, offsetX, y, offsetX + 1, y, offsetX + 2, y, scores);
        }
    }

    void getScoresForThree(const PentagoBoard *board, int x1, int y1, int x2, int y2, int x3, int y3,
                           float scores[BOARD_SIZE][BOARD_SIZE]) {
        PlayerNumber curr = board->playerToMoveNext;

        int empties = 0;
        // 8 is more players that will ever play.
        int counts[8] = {0};

        PlayerNumber places[3] = {-1, -1, -1};

        places[0] = board->getAtPosition(x1, y1);
        places[1] = board->getAtPosition(x2, y2);
        places[2] = board->getAtPosition(x3, y3);

        for (int i = 0; i < 3; i++) {
            if (places[i] == NoPlayer) {
                empties ++;
            } else {
                counts[places[i]] ++;
            }
        }

        if (counts[curr] == 3) {
            return;
        } else if (counts[curr] == 2 && empties == 1) {
            if (places[0] == NoPlayer)
                scores[y1][x1] += 2.1;
            if (places[1] == NoPlayer)
                scores[y2][x2] += 2.1;
            if (places[2] == NoPlayer)
                scores[y3][x3] += 2.1;

            return;
        } else if (empties == 1) {
            if (places[0] == places[1]) {
                scores[y3][x3] += 1.5;
            } else if (places[1] == places[2]) {
                scores[y1][x1] += 1.5;
            } else if (places[0] == places[2]) {
                scores[y2][x2] += 1.5;
            }
        } else if (empties == 2) {
            if (counts[curr] == 1) {
                if (places[0] == NoPlayer)
                    scores[y1][x1] += 1;
                if (places[1] == NoPlayer)
                    scores[y2][x2] += 1;
                if (places[2] == NoPlayer)
                    scores[y3][x3] += 1;
            } else {
                if (places[0] == NoPlayer)
                    scores[y1][x1] += 0.4;
                if (places[1] == NoPlayer)
                    scores[y2][x2] += 0.4;
                if (places[2] == NoPlayer)
                    scores[y3][x3] += 0.4;
            }
        }
    }
};


class PentagoGame {
    PentagoBoard *board;
    vector<Player*> *players;
public:
    PentagoGame(vector<Player*> *players): players(players) {
        board = new PentagoBoard(players->size());
    }

    PlayerNumber playGame(bool verbose) {
        int idx;
        while (!isFinished()) {
            idx = 0;
            for (Player *player: *players) {
                if (verbose) {
                    cout << "It is " << player->getName() << "'s turn!" << endl;
                    board->print();
                }
                Move move = player->getMove(board);
                board->playAtPosition(move.position);
                if (isFinished()) goto end;
                board->rotate(move.rotation);
                if (isFinished()) goto end;
                idx++;
            }
        }

        end:
            if (verbose) {
                board->print();
                printf("%d wins!\n", board->getWinner());
            }
            return board->getWinner();
    }

    bool isFinished() {
        return board->getWinner() != NoPlayer || board->isDraw();
    }
};

int test_main() {
    srand (time(NULL));

    vector<Player *> players { new HumanPlayer("Daniel"), new HeuristicPlayer("Buck")};

    PentagoGame *game = new PentagoGame(&players);

    int betterPlayerWinCount = 0;

    for (int i = 0; i < 1; ++i) {
        if (i%100 == 0) {
            printf("%d\n", i);
        }
        PentagoGame *game = new PentagoGame(&players);
        betterPlayerWinCount += game->playGame(true) == 0;
    }
    printf("%d\n", betterPlayerWinCount);


    return 0;
}

int main(int argc, const char* argv[]) {
    float duration;
    int numberOfPlayers;
    int mySymbol;

    sscanf(argv[1], "%f", &duration);
    sscanf(argv[2], "%d", &numberOfPlayers);
    sscanf(argv[3], "%d", &mySymbol);

    PentagoBoard *board = new PentagoBoard(numberOfPlayers);

    printf("%lf %d %d\n", duration, numberOfPlayers, mySymbol);

    string line;
    std::getline(std::cin, line);

    string word;
    vector<string> words;

    for (stringstream s(line); s >> word; )
        words.push_back(word);

    for (int i=0; i<words.size(); i++)
    {
        int val;
        sscanf(words[i].c_str(), "%d", &val);

        board->playWhoeverAtPosition(Position(i % BOARD_SIZE, i / BOARD_SIZE), val - 1);
    }

    board->playerToMoveNext = mySymbol;

//    board->print();

    HeuristicPlayer player("Buck");
    printMove(player.getMove(board));


    return 0;
}