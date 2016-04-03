#include <iostream>
#include <vector>
#include <list>

#define BOARD_SIZE 9
#define PENTA 5
#define MINI_SQUARE_SIZE 3
#define NUMBER_OF_MINI_SQUARES BOARD_SIZE / MINI_SQUARE_SIZE
typedef signed char PlayerNumber;

#define NoPlayer -1

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

struct Move {
    Position position;
    Rotation rotation;
    Move(Position pos, Rotation rot): position(pos), rotation(rot) {};
};

class Player;

class PentagoBoard {
    PlayerNumber board[BOARD_SIZE][BOARD_SIZE];
    int playedPieces;

public:
    PentagoBoard() {
        for (int x = 0; x < BOARD_SIZE; x++) {
            for (int y = 0; y < BOARD_SIZE; y++) {
                setAtPosition(x, y, NoPlayer);
            }
        }
        playedPieces = 0;
    }

    PlayerNumber getAtPosition(int x, int y) {
        return board[y][x];
    }

    PlayerNumber getAtPosition(Position pos) {
        return board[pos.y][pos.x];
    }

    void playAtPosition(Position pos, PlayerNumber playerNumber) {
        playedPieces ++;
        setAtPosition(pos, playerNumber);
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
        playAtPosition(move.position, playerNumber);
        rotate(move.rotation);
    }

    void print() {
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

    PlayerNumber getWinner() {
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

    bool isDraw() {
        return playedPieces == BOARD_SIZE * BOARD_SIZE;
    }

private:

    PlayerNumber checkLine(int startX, int startY, int deltaX, int deltaY) {
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
        setAtPosition(offsetX + 1, offsetY + 2, temp);
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
    virtual Move getMove(PentagoBoard *board) = 0;
    virtual string getName() = 0;
};

class HumanPlayer: public Player {
    string name;
public:
    HumanPlayer(string name): name(name) {}

    Move getMove(PentagoBoard *board) {
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

class RandomPlayer: public Player {
    string name;

public:
    RandomPlayer(string name): name(name) {}

    string getName() {
        return name;
    }

    Move getMove(PentagoBoard *board) {
        int x = rand() % BOARD_SIZE;
        int y = rand() % BOARD_SIZE;

        if (board->getAtPosition(x, y) != NoPlayer) {
            return getMove(board);
        }

        return Move(
                Position(x, y),
                Rotation(rand() % NUMBER_OF_MINI_SQUARES,
                         rand() % NUMBER_OF_MINI_SQUARES,
                         rand() % 2 == 0 ? CLOCKWISE : COUNTERCLOCKWISE));
    }
};

class PentagoGame {
    PentagoBoard *board;
    vector<Player*> *players;
public:
    PentagoGame(vector<Player*> *players): players(players) {
        board = new PentagoBoard();
    }

    void playGame() {
        int idx;
        while (!isFinished()) {
            idx = 0;
            for (Player *player: *players) {
                cout << "It is " << player->getName() << "'s turn!" << endl;
                board->print();
                Move move = player->getMove(board);
                board->playAtPosition(move.position, idx);
                if (isFinished()) goto end;
                board->rotate(move.rotation);
                if (isFinished()) goto end;
                idx++;
            }
        }
        end:
            board->print();
            printf("%d wins!\n", board->getWinner());
    }

    bool isFinished() {
        return board->getWinner() != NoPlayer || board->isDraw();
    }
};

int main() {
    srand (time(NULL));

    vector<Player *> players { new RandomPlayer("Buck"), new RandomPlayer("Daniel"), new RandomPlayer("Aubrey"), new RandomPlayer("Patrick")};

    PentagoGame *game = new PentagoGame(&players);
    game->playGame();
}