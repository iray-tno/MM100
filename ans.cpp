#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <unordered_set>
#include <queue>
#include <utility>
#include <cmath>
#include <cassert>
#include <memory>

// localでのみassertを使う
#ifndef LOCAL_DEBUG
#define NDEBUG
#endif

#ifdef LOCAL_DEBUG
#undef _MSC_VER
#endif

using namespace std;

template<typename T> inline bool isWithin(T begin, T value, T end) { return begin <= value && value < end; }

constexpr int dx[] = { 0, 1, 0, -1 };
constexpr int dy[] = { 1, 0, -1, 0 };

static unsigned int x = 123456789, y = 362436069, z = 521288629, w = 88675123;
inline unsigned int randxor() {
    unsigned int t;
    t = (x ^ (x << 11));
    x = y;
    y = z;
    z = w;
    return (w = (w ^ (w >> 19)) ^ (t ^ (t >> 8)));
}

template<class T = unsigned int>
class ZobristHash {
 vector<T> hashItems;
 int maxW;
 int toIndex(int h, int w) { return h*w; }
public:
 ZobristHash() = default;
 ZobristHash(int h, int w) {
   maxW = w;
   int hw = toIndex(h,w);
   hashItems = vector<T>(hw, -1);
   for (int i = 0; i < hw; ++i) {
      hashItems[i] = randxor();
   }
  }
  T getItem(int h, int w) { return hashItems[toIndex(h,w)]; }
};

#ifdef _MSC_VER
#include <Windows.h>
#else
#include <sys/time.h>
#endif

class Timer {
private:
    double start_time;

#ifdef _MSC_VER
    double getMs() { return (double)GetTickCount64() / 1000; }
#else
    double getMs() {
        struct timeval t;
        gettimeofday(&t, NULL);
        return (double)t.tv_sec * 1000 + (double)t.tv_usec / 1000;
    }
#endif
    double getSec() { return getMs() / 1000; }

public:
    Timer() {}

    void start() { start_time = getSec(); }
    double getElapsed() { return getSec() - start_time; }
};

typedef char Color;

typedef vector<Color> BoardRow;
typedef vector<BoardRow> Board;

class Move {
public:
    int ax, ay, bx, by;
    Move() = default;
    Move(int ax_, int ay_, int bx_, int by_): ax(ax_), ay(ay_), bx(bx_), by(by_) {}

    static const Move EMPTY;
    static const Move SKIP;

    bool operator==(const Move& move) {
        return (ax == move.ax) && (ay == move.ay) && (bx == move.bx) && (by == move.by);
    };
};

const Move Move::EMPTY = Move(-1, -1, -1, -1);
const Move Move::SKIP = Move(-114514, -114514, -114514, -114514);

class BeamState {
public:
    shared_ptr<BeamState> prev;
    Move move;
    Color color;
    int turns;
    int index;
    int score;
    int hash;

    BeamState() = default;
    BeamState(shared_ptr<BeamState> prev_, Move move_, Color color_, int turns_, int index_, int score_, int hash_):
    prev(prev_), color(color_), turns(turns_), index(index_), score(score_), hash(hash_) {}
};

struct Compare : public binary_function<BeamState, BeamState, bool> {
    bool operator()(const BeamState &a, const BeamState &b) {
        return a.score < b.score;
    }
};

class TemporaryField {
public:
    Board board;
    int h, w;
    int score;
    unsigned int hash;
    ZobristHash<unsigned int> hashGenerator;

// ボード
// スコア, hash
// update関数
// 選択肢列挙
// スコアや選択肢に使う状態(setなど)

    TemporaryField() = default;
    TemporaryField(const int h_, const int w_, const vector<string>& board_): h(h_), w(w_) {
        board = Board(h, BoardRow(w, -1));
        hashGenerator = ZobristHash<unsigned int>(h, w);
        hash = 0;
        score = 0;
        for (int i = 0; i < h; ++i) {
            for (int j = 0; j < w; ++j) {
                board[i][j] = board_[i][j] - '0';
                hash ^= hashGenerator.getItem(i, j);
            }
        }
    }

    void next(const Move& move) {
        if (move.ax == Move::SKIP.ax) return;
        assert(isWithin(0, move.ax, h) && isWithin(0, move.ay, w));
        assert(isWithin(0, move.bx, h) && isWithin(0, move.by, w));

        assert(0 <= board[move.ax][move.ay]);
        assert(0 <= board[move.bx][move.ay]);

        board[move.ax][move.ay] *= -1;
        board[move.bx][move.by] *= -1;

        hash ^= hashGenerator.getItem(move.ax, move.ay);
        hash ^= hashGenerator.getItem(move.bx, move.by);

        score += 2;
    }

    void prev(const Move& move) {
        if (move.ax == Move::SKIP.ax) return;
        assert(isWithin(0, move.ax, h) && isWithin(0, move.ay, w));
        assert(isWithin(0, move.bx, h) && isWithin(0, move.by, w));

        assert(board[move.ax][move.ay] < 0);
        assert(board[move.bx][move.ay] < 0);

        board[move.ax][move.ay] *= -1;
        board[move.bx][move.by] *= -1;

        hash ^= hashGenerator.getItem(move.ax, move.ay);
        hash ^= hashGenerator.getItem(move.bx, move.by);

        score -= 2;
    }

    vector<Move> getMoves() {
        vector<Move> moves;

        for (int x = 0; x < h; ++x) {
            for (int y = 0; y < w; ++y) {
                bool updatedxy = false;
                if (board[x][y] < 0) continue;
                for (int nx = x; nx < h; ++nx) {
                    for (int ny = 0; ny < w; ++ny) {
                        if (check(x, y, nx, ny)) {
                            moves.emplace_back(Move(x, y, nx, ny));
                            // updatedxy = true;
                        }
                        if (updatedxy) break;
                    }
                    if (updatedxy) break;
                }
            }
        }
        return moves;
    }

    bool check(const int ax, const int ay, const int bx, const int by) {
        assert(isWithin(0, ax, h)); assert(isWithin(0, ay, w));
        assert(isWithin(0, bx, h)); assert(isWithin(0, by, w));

        if(board[ax][ay] != board[bx][by]) return false;
        if(ax == bx && ay == by) return false;
        if(board[ax][ay] < 0) return false;

        Color color = board[ax][ay];

        int minx = min(ax,bx), maxx = max(ax,bx);
        int miny = min(ay,by), maxy = max(ay,by);
        for (int i = minx; i <= maxx; ++i) {
            for (int j = miny; j <= maxy; ++j) {
                if (board[i][j] != -1 && board[i][j] != color) return false;
            }
        }

        return true;
    }
};

class Brain {
    TemporaryField field;
    int h,w;
public:
    Brain(const vector<string>& board) {
        h = board.size(), w = board[0].size();

        field = TemporaryField(h, w, board);
        // field.dump();
        // cerr << score << endl;
    }

    vector<string> think() {
        vector<string> ans;

        typedef priority_queue<BeamState, vector<BeamState>, Compare> BeamStates;
        BeamStates beamStates;

        BeamStates& currentBeamStates = beamStates;


        BeamState firstState(nullptr, Move::EMPTY, -1, 0, 0, 0, field.hash);
        BeamState firstState2(nullptr, Move::EMPTY, -1, 3, 0, 3, field.hash);
        beamStates.push(firstState);
        beamStates.push(firstState2);

        auto lastState = firstState;

        int turn = 0;
        while(turn < 1) {
            BeamStates nextStateis;

            while(currentBeamStates.size() != 0) {
                BeamState currentState = currentBeamStates.top(); currentBeamStates.pop();
                cerr << currentState.score << endl;
                continue;
                if (field.hash != currentState.hash) {
                    // cerr << ",";
                    vector<Move> redos, undos;
                    BeamState from = lastState, to = currentState;

                    while(from.turns < to.turns) {
                        redos.push_back(to.move);
                        to = *to.prev;
                    }
                    while(to.turns < from.turns) {
                        undos.push_back(from.move);
                        from = *from.prev;
                    }
                    while(from.hash != to.hash) {
                        redos.push_back(to.move);
                        undos.push_back(from.move);
                        to = *to.prev;
                        from = *from.prev;
                    }

                    reverse(redos.begin(), redos.end());

                    for (auto& undo: undos) {
                        field.prev(undo);
                    }
                    for (auto& redo: redos) {
                        field.next(redo);
                    }
                }
                lastState = currentState;

                auto moves = field.getMoves();
                if (moves.size() == 0) break;
            }


            unordered_set<unsigned int> visitedHash;

            turn++;
        }
        return ans;
    }

    double calcScore(int h, int w, int seqSize) {
        double hw = h * w;
        return seqSize * 2 / hw;
    }
};

/**
 * SameColorPairs
 */
class SameColorPairs {
public:
    vector<string> removePairs(vector<string> board) {
        Brain brain(board);
        return brain.think();
    }
};

#ifdef LOCAL_DEBUG
// -------8<------- end of solution submitted to the website -------8<-------

template<class T> void getVector(vector<T>& v) {
    for (int i = 0; i < v.size(); ++i)
        cin >> v[i];
}

int main(int, char **) {
    cin.tie(0); ios::sync_with_stdio(false);

    SameColorPairs scp;
    int H;
    cin >> H;
    vector<string> board(H);
    getVector(board);

    vector<string> ret = scp.removePairs(board);
    cout << ret.size() << endl;
    for (int i = 0; i < (int)ret.size(); ++i)
        cout << ret[i] << endl;
    cout.flush();

    return 0;
}

#endif
