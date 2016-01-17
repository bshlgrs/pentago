#include <iostream>
#include <vector>
#include <list>

using namespace std;

class PentagoBoard;

struct Position {
    int x, y;
    Position(int x, int y): x(x), y(y) {}
};

enum RotationDirection { CLOCKWISE, COUNTERCLOCKWISE };

struct Rotation {
    int x, y;
    RotationDirection dir;
    Rotation(int x, int y, RotationDirection dir): x(x), y(y), dir(dir) {};
};


class Player;

class PentagoBoard {
    Player* board[9][9];

public:
    Player* getAtPosition(int x, int y) {
        return board[y][x];
    }

    Player* getAtPosition(Position pos) {
        return board[pos.y][pos.x];
    }

    void setAtPosition(int x, int y, Player &player) {
        board[y][x] = &player;
    }

    void setAtPosition(Position pos, Player &player) {
        board[pos.y][pos.x] = &player;
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

    void rotateSquareClockwise(int x, int y) {
        int offsetX = x * 3;
        int offsetY = y * 3;

        Player* temp = getAtPosition(offsetX, offsetY);
        setAtPosition(offsetX, offsetY, *getAtPosition(offsetX, offsetY + 2));
        setAtPosition(offsetX, offsetY + 2, *getAtPosition(offsetX + 2, offsetY + 2));
        setAtPosition(offsetX + 2, offsetY + 2, *getAtPosition(offsetX + 2, offsetY));
        setAtPosition(offsetX + 2, offsetY, *temp);

        temp = getAtPosition(offsetX + 1, offsetY);
        setAtPosition(offsetX + 1, offsetY, *getAtPosition(offsetX, offsetY + 1));
        setAtPosition(offsetX, offsetY + 1, *getAtPosition(offsetX + 1, offsetY + 2));
        setAtPosition(offsetX + 1, offsetY + 2, *getAtPosition(offsetX + 2, offsetY + 1));
        setAtPosition(offsetX + 1, offsetY + 2, *temp);
    }

    void print(vector<Player *> *players) {
        for (int y = 0; y < 9; y++) {
            for (int x = 0; x < 9; x++) {
                Player * atPos = getAtPosition(x, y);
                if (atPos == nullptr) {
                    cout << ".";
                } else {
                    int count = 0;
                    for (Player * player : *players) {
                        if (atPos == player) {
                            cout << count;
                            break;
                        }
                        count ++;
                    }
                }

                if (x % 3 == 2) cout << " ";
            }
            cout << endl;
            if (y % 3 == 2) cout << endl;
        }
        cout << endl;
    }

    Player *getWinner() {
        vector<pair<int, int>> directions = { {0, 1}, {1, 0}, {1, 1}, {1, -1} };
        for (pair<int, int> direction : directions) {
            for (int x = 0; x < 9; x++) {
                for (int y = 0; y < 9; y++) {
                    int xx = x;
                    int yy = y;
                    int runLength = 0;
                    Player *player = NULL;

                    while (xx < 9 && x >= 0 && yy < 9 && yy >= 0) {
//                        cout << xx << "," << yy << endl;
                        if (player == getAtPosition(xx, yy)) {
                            runLength ++;
                            if (runLength == 5 && player != NULL) {
                                return player;
                            }
                        }
                        player = getAtPosition(xx, yy);
                        runLength = 1;
                        xx += direction.first;
                        yy += direction.second;
                    }
                }
            }
        }

        return nullptr;
    }
};

class Player {
public:
    virtual Position getMove(PentagoBoard *board) = 0;
    virtual Rotation getRotation(PentagoBoard *board) = 0;
    virtual string getName() = 0;
};

class HumanPlayer: public Player {
    string name;
public:
    HumanPlayer(string name): name(name) {}

    Position getMove(PentagoBoard *board) {
        int x, y;
        printf("give me your x: ");
        scanf("%d", &x);
        printf("\ngive me your y: ");
        scanf("%d", &y);

        if (board->getAtPosition(x, y) == nullptr) {
            printf("\ncoolio!\n\n");
            return Position(x, y);
        } else {
            printf("That position is already taken! Give me an empty position please.\n");
            return getMove(board);
        }
    }

    Rotation getRotation(PentagoBoard *board) {
        int x, y;
        printf("give me your x: ");
        scanf("%d", &x);
        printf("\ngive me your y: ");
        scanf("%d", &y);

        int dir;
        printf("type 0 for clockwise, 1 for counterclockwise: ");
        scanf("%d", &dir);

        RotationDirection rotationDirection = dir ? COUNTERCLOCKWISE : CLOCKWISE;

        return Rotation(x, y, rotationDirection);
    }

    string getName() {
        return name;
    }
};

class PentagoGame {
    PentagoBoard *board;
    vector<Player*> *players;
public:
    PentagoGame(vector<Player*> *players): players(players) {
        board = new PentagoBoard;
    }

    void playGame() {
        while (!isFinished()) {
            for(Player* player: *players) {
                cout << "It is " << player->getName() << "'s turn!" << endl;
                board->print(players);
                Position move = player->getMove(board);
                board->setAtPosition(move, *player);

                cout << "It is " << player->getName() << "'s rotation!" << endl;
                board->print(players);
                Rotation rot = player->getRotation(board);
                board->rotate(rot);
            }
        }
    }

    bool isFinished() {
        return board->getWinner() != nullptr;
    }
};

int main() {
    vector<Player *> players { new HumanPlayer("Buck"), new HumanPlayer("Daniel")};

    PentagoGame *game = new PentagoGame(&players);
    game->playGame();
}