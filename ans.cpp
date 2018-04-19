#include <iostream>
#include <string>
#include <vector>
#include <cmath>

using namespace std;

#ifdef LOCAL_DEBUG
// 使うクラスの定義
#endif

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

/**
 * SameColorPairs
 */
class SameColorPairs {
public:
    vector<string> removePairs(vector<string> board) {
        vector<string> ret;
        int H = board.size(), W = board[0].size();
        // find the first pair of horizontally adjacent tiles and remove them
        for (int i = 0; i < H; ++i)
        for (int j = 1; j < W; ++j)
            if (board[i][j] == board[i][j-1]) {
                ret.push_back(to_string(i) + " " + to_string(j) + " " + to_string(i) + " " + to_string(j-1));
                return ret;
            }
        return ret;
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
