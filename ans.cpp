#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <utility>
#include <cmath>
#include <cassert>

using namespace std;

#ifdef LOCAL_DEBUG
#undef _MSC_VER
#endif

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

class Field {
    public:
    Board board;
    int h, w;

    Field();
    Field(int h_, int w_, vector<string> board_): h(h_), w(w_) {
        board = Board(h, BoardRow(w, -1));

        for (int i = 0; i < h; ++i) {
            for (int j = 0; j < w; ++j) {
                board[i][j] = board_[i][j] - '0';
            }
        }
    }

    void dump() {
        for (int i = 0; i < h; ++i) {
            for (int j = 0; j < w; ++j) {
                cerr << (int) board[i][j];
            }
            cerr << endl;
        }
        return;
    }

    /**
     * 隣接をとっていくだけ
     */
    vector<string> hoge() {
        vector<string> seq;
        for (int x = 0; x < h; ++x) {
            for (int y = 0; y < w; ++y) {
                for (int i = 0; i < 4; ++i) {
                    if (board[x][y] < 0) continue;

                    int nx = x + dx[i];
                    int ny = y + dy[i];

                    if (!isWithin(0, nx, h)) continue;
                    if (!isWithin(0, ny, w)) continue;
                    if (board[x][y] != board[nx][ny]) continue;

                    board[x][y] = -1;
                    board[nx][ny] = -1;
                    seq.push_back(to_string(x) + " " + to_string(y) + " " + to_string(nx) + " " + to_string(ny));
                }
            }
        }
        return seq;
    }

    vector<string> fuga() {
        auto seq = hoge();

        bool updated = true;
        while(updated) {
            updated = false;
            for (int x = 0; x < h; ++x) {
                for (int y = 0; y < w; ++y) {
                    bool updatedxy = false;
                    if (board[x][y] < 0) continue;
                    for (int nx = x; nx < h; ++nx) {
                        for (int ny = 0; ny < w; ++ny) {
                            if (check(x,y,nx,ny)) {
                                board[x][y] = -1;
                                board[nx][ny] = -1;
                                seq.push_back(to_string(x) + " " + to_string(y) + " " + to_string(nx) + " " + to_string(ny));
                                updated = true;
                                updatedxy = true;
                            }
                            if (updatedxy) break;
                        }
                        if (updatedxy) break;
                    }
                }
            }
        }
        return seq;
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

/**
 * SameColorPairs
 */
class SameColorPairs {
public:
    vector<string> removePairs(vector<string> board) {
        int H = board.size(), W = board[0].size();

        Field field(H, W, board);
        // field.dump();

        auto ans = field.fuga();
        auto score = calcScore(H,W,ans.size());
        // cerr << score << endl;

        return ans;
    }

    double calcScore(int h, int w, int seqSize) {
        double hw = h*w;
        return seqSize * 2 / hw;
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
