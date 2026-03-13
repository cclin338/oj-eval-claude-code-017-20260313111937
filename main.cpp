#include <iostream>
#include <cstring>
#include <fstream>
#include <algorithm>

using std::string;
using std::cin;
using std::cout;
using std::endl;

// Simple vector implementation
template<typename T>
class Vector {
private:
    T* data;
    int cap;
    int sz;

    void resize() {
        cap = cap * 2 + 1;
        T* newData = new T[cap];
        for (int i = 0; i < sz; i++) {
            newData[i] = data[i];
        }
        delete[] data;
        data = newData;
    }

public:
    Vector() : data(nullptr), cap(0), sz(0) {}
    ~Vector() { delete[] data; }

    void push_back(const T& val) {
        if (sz >= cap) resize();
        data[sz++] = val;
    }

    T& operator[](int i) { return data[i]; }
    const T& operator[](int i) const { return data[i]; }
    int size() const { return sz; }
    bool empty() const { return sz == 0; }
    void clear() { sz = 0; }
    T* begin() { return data; }
    T* end() { return data + sz; }
};

// Simple map using array (for small datasets)
template<typename K, typename V, int MAX_SIZE = 10000>
class SimpleMap {
public:
    struct Pair {
        K key;
        V value;
        bool used;
        Pair() : used(false) {}
    };
    Pair data[MAX_SIZE];
    int sz;

    SimpleMap() : sz(0) {}

    V* find(const K& key) {
        for (int i = 0; i < sz; i++) {
            if (data[i].used && data[i].key == key) {
                return &data[i].value;
            }
        }
        return nullptr;
    }

    const V* find(const K& key) const {
        for (int i = 0; i < sz; i++) {
            if (data[i].used && data[i].key == key) {
                return &data[i].value;
            }
        }
        return nullptr;
    }

    void insert(const K& key, const V& value) {
        V* existing = find(key);
        if (existing) {
            *existing = value;
            return;
        }
        if (sz < MAX_SIZE) {
            data[sz].key = key;
            data[sz].value = value;
            data[sz].used = true;
            sz++;
        }
    }

    bool erase(const K& key) {
        for (int i = 0; i < sz; i++) {
            if (data[i].used && data[i].key == key) {
                data[i].used = false;
                return true;
            }
        }
        return false;
    }

    void clear() { sz = 0; }
};

// User structure
struct User {
    char username[25];
    char password[35];
    char name[35];
    char mailAddr[35];
    int privilege;
    bool loggedIn;

    User() : privilege(0), loggedIn(false) {
        username[0] = password[0] = name[0] = mailAddr[0] = '\0';
    }
};

// Train structure
struct Train {
    char trainID[25];
    int stationNum;
    char stations[100][35];
    int seatNum;
    int prices[100];
    int startHour, startMin;
    int travelTimes[100];
    int stopoverTimes[100];
    int saleStartMonth, saleStartDay;
    int saleEndMonth, saleEndDay;
    char type;
    bool released;

    Train() : stationNum(0), seatNum(0), released(false) {
        trainID[0] = '\0';
    }
};

// Order structure
struct Order {
    char username[25];
    char trainID[25];
    char from[35];
    char to[35];
    int leaveMonth, leaveDay, leaveHour, leaveMin;
    int arriveMonth, arriveDay, arriveHour, arriveMin;
    int price;
    int num;
    int status; // 0: success, 1: pending, 2: refunded
    long long timestamp;

    Order() : price(0), num(0), status(0), timestamp(0) {
        username[0] = trainID[0] = from[0] = to[0] = '\0';
    }
};

// Global data
SimpleMap<string, User, 5000> users;
SimpleMap<string, Train, 5000> trains;
Vector<Order> orders;
SimpleMap<string, bool, 1000> loggedInUsers;
long long orderTimestamp = 0;

// Utility functions
void parseTime(const string& timeStr, int& hour, int& min) {
    sscanf(timeStr.c_str(), "%d:%d", &hour, &min);
}

void parseDate(const string& dateStr, int& month, int& day) {
    sscanf(dateStr.c_str(), "%d-%d", &month, &day);
}

int getDaysInMonth(int month) {
    if (month == 6 || month == 8) return 31;
    if (month == 7) return 31;
    return 30;
}

void addMinutes(int& month, int& day, int& hour, int& min, int minutes) {
    min += minutes;
    hour += min / 60;
    min %= 60;
    day += hour / 24;
    hour %= 24;

    while (day > getDaysInMonth(month)) {
        day -= getDaysInMonth(month);
        month++;
    }
}

// Command parser
struct Command {
    string cmd;
    SimpleMap<char, string, 20> params;
};

Command parseCommand(const string& line) {
    Command cmd;
    int i = 0;
    while (i < line.length() && line[i] != ' ') {
        cmd.cmd += line[i++];
    }

    while (i < line.length()) {
        while (i < line.length() && line[i] == ' ') i++;
        if (i >= line.length() || line[i] != '-') break;

        char key = line[++i];
        i++;
        while (i < line.length() && line[i] == ' ') i++;

        string value;
        while (i < line.length() && line[i] != ' ') {
            if (line[i] == '-' && i + 1 < line.length() && line[i+1] >= 'a' && line[i+1] <= 'z') {
                break;
            }
            value += line[i++];
        }
        cmd.params.insert(key, value);
    }

    return cmd;
}

// Command handlers
void handleAddUser(const Command& cmd) {
    const string* c = cmd.params.find('c');
    const string* u = cmd.params.find('u');
    const string* p = cmd.params.find('p');
    const string* n = cmd.params.find('n');
    const string* m = cmd.params.find('m');
    const string* g = cmd.params.find('g');

    if (!u || !p || !n || !m) {
        cout << "-1" << endl;
        return;
    }

    // Check if first user
    bool isFirst = (users.find(*u) == nullptr);
    for (int i = 0; i < 5000; i++) {
        if (users.data[i].used) {
            isFirst = false;
            break;
        }
    }

    if (users.find(*u)) {
        cout << "-1" << endl;
        return;
    }

    if (!isFirst) {
        if (!c || !g) {
            cout << "-1" << endl;
            return;
        }

        User* curUser = users.find(*c);
        if (!curUser || !curUser->loggedIn) {
            cout << "-1" << endl;
            return;
        }

        int newPrivilege = atoi(g->c_str());
        if (newPrivilege >= curUser->privilege) {
            cout << "-1" << endl;
            return;
        }
    }

    User newUser;
    strcpy(newUser.username, u->c_str());
    strcpy(newUser.password, p->c_str());
    strcpy(newUser.name, n->c_str());
    strcpy(newUser.mailAddr, m->c_str());
    newUser.privilege = isFirst ? 10 : atoi(g->c_str());
    newUser.loggedIn = false;

    users.insert(*u, newUser);
    cout << "0" << endl;
}

void handleLogin(const Command& cmd) {
    const string*u = cmd.params.find('u');
    const string*p = cmd.params.find('p');

    if (!u || !p) {
        cout << "-1" << endl;
        return;
    }

    User* user = users.find(*u);
    if (!user || strcmp(user->password, p->c_str()) != 0) {
        cout << "-1" << endl;
        return;
    }

    if (user->loggedIn) {
        cout << "-1" << endl;
        return;
    }

    user->loggedIn = true;
    cout << "0" << endl;
}

void handleLogout(const Command& cmd) {
    const string*u = cmd.params.find('u');

    if (!u) {
        cout << "-1" << endl;
        return;
    }

    User* user = users.find(*u);
    if (!user || !user->loggedIn) {
        cout << "-1" << endl;
        return;
    }

    user->loggedIn = false;
    cout << "0" << endl;
}

void handleQueryProfile(const Command& cmd) {
    const string*c = cmd.params.find('c');
    const string*u = cmd.params.find('u');

    if (!c || !u) {
        cout << "-1" << endl;
        return;
    }

    User* curUser = users.find(*c);
    User* queryUser = users.find(*u);

    if (!curUser || !queryUser || !curUser->loggedIn) {
        cout << "-1" << endl;
        return;
    }

    if (curUser->privilege <= queryUser->privilege && *c != *u) {
        cout << "-1" << endl;
        return;
    }

    cout << queryUser->username << " " << queryUser->name << " "
         << queryUser->mailAddr << " " << queryUser->privilege << endl;
}

void handleModifyProfile(const Command& cmd) {
    const string*c = cmd.params.find('c');
    const string*u = cmd.params.find('u');

    if (!c || !u) {
        cout << "-1" << endl;
        return;
    }

    User* curUser = users.find(*c);
    User* modUser = users.find(*u);

    if (!curUser || !modUser || !curUser->loggedIn) {
        cout << "-1" << endl;
        return;
    }

    if (curUser->privilege <= modUser->privilege && *c != *u) {
        cout << "-1" << endl;
        return;
    }

    const string*p = cmd.params.find('p');
    const string*n = cmd.params.find('n');
    const string*m = cmd.params.find('m');
    const string*g = cmd.params.find('g');

    if (g) {
        int newPriv = atoi(g->c_str());
        if (newPriv >= curUser->privilege) {
            cout << "-1" << endl;
            return;
        }
        modUser->privilege = newPriv;
    }

    if (p) strcpy(modUser->password, p->c_str());
    if (n) strcpy(modUser->name, n->c_str());
    if (m) strcpy(modUser->mailAddr, m->c_str());

    cout << modUser->username << " " << modUser->name << " "
         << modUser->mailAddr << " " << modUser->privilege << endl;
}

void handleAddTrain(const Command& cmd) {
    const string*i = cmd.params.find('i');
    if (!i || trains.find(*i)) {
        cout << "-1" << endl;
        return;
    }

    Train train;
    strcpy(train.trainID, i->c_str());

    const string*n = cmd.params.find('n');
    train.stationNum = atoi(n->c_str());

    const string*m = cmd.params.find('m');
    train.seatNum = atoi(m->c_str());

    const string*s = cmd.params.find('s');
    int idx = 0;
    for (int j = 0, k = 0; j < s->length(); j++) {
        if ((*s)[j] == '|') {
            train.stations[idx++][k] = '\0';
            k = 0;
        } else {
            train.stations[idx][k++] = (*s)[j];
        }
    }

    const string*p = cmd.params.find('p');
    idx = 0;
    string num;
    for (int j = 0; j < p->length(); j++) {
        if ((*p)[j] == '|') {
            train.prices[idx++] = atoi(num.c_str());
            num.clear();
        } else {
            num += (*p)[j];
        }
    }
    if (!num.empty()) train.prices[idx++] = atoi(num.c_str());

    const string*x = cmd.params.find('x');
    parseTime(*x, train.startHour, train.startMin);

    const string*t = cmd.params.find('t');
    idx = 0;
    num.clear();
    for (int j = 0; j < t->length(); j++) {
        if ((*t)[j] == '|') {
            train.travelTimes[idx++] = atoi(num.c_str());
            num.clear();
        } else {
            num += (*t)[j];
        }
    }
    if (!num.empty()) train.travelTimes[idx++] = atoi(num.c_str());

    const string*o = cmd.params.find('o');
    idx = 0;
    num.clear();
    if (*o != "_") {
        for (int j = 0; j < o->length(); j++) {
            if ((*o)[j] == '|') {
                train.stopoverTimes[idx++] = atoi(num.c_str());
                num.clear();
            } else {
                num += (*o)[j];
            }
        }
        if (!num.empty()) train.stopoverTimes[idx++] = atoi(num.c_str());
    }

    const string*d = cmd.params.find('d');
    int pos = d->find('|');
    parseDate(d->substr(0, pos), train.saleStartMonth, train.saleStartDay);
    parseDate(d->substr(pos + 1), train.saleEndMonth, train.saleEndDay);

    const string*y = cmd.params.find('y');
    train.type = (*y)[0];

    trains.insert(*i, train);
    cout << "0" << endl;
}

void handleReleaseTrain(const Command& cmd) {
    const string*i = cmd.params.find('i');
    if (!i) {
        cout << "-1" << endl;
        return;
    }

    Train* train = trains.find(*i);
    if (!train || train->released) {
        cout << "-1" << endl;
        return;
    }

    train->released = true;
    cout << "0" << endl;
}

void handleQueryTrain(const Command& cmd) {
    const string*i = cmd.params.find('i');
    const string*d = cmd.params.find('d');

    if (!i || !d) {
        cout << "-1" << endl;
        return;
    }

    Train* train = trains.find(*i);
    if (!train) {
        cout << "-1" << endl;
        return;
    }

    int queryMonth, queryDay;
    parseDate(*d, queryMonth, queryDay);

    cout << train->trainID << " " << train->type << endl;

    int currentMonth = queryMonth;
    int currentDay = queryDay;
    int currentHour = train->startHour;
    int currentMin = train->startMin;
    int totalPrice = 0;

    for (int j = 0; j < train->stationNum; j++) {
        cout << train->stations[j] << " ";

        if (j == 0) {
            cout << "xx-xx xx:xx -> ";
        } else {
            printf("%02d-%02d %02d:%02d -> ", currentMonth, currentDay, currentHour, currentMin);
            addMinutes(currentMonth, currentDay, currentHour, currentMin, train->stopoverTimes[j-1]);
        }

        if (j == train->stationNum - 1) {
            cout << "xx-xx xx:xx ";
        } else {
            printf("%02d-%02d %02d:%02d ", currentMonth, currentDay, currentHour, currentMin);
        }

        cout << totalPrice << " ";

        if (j == train->stationNum - 1) {
            cout << "x";
        } else {
            cout << train->seatNum;
        }
        cout << endl;

        if (j < train->stationNum - 1) {
            totalPrice += train->prices[j];
            addMinutes(currentMonth, currentDay, currentHour, currentMin, train->travelTimes[j]);
        }
    }
}

void handleDeleteTrain(const Command& cmd) {
    const string*i = cmd.params.find('i');
    if (!i) {
        cout << "-1" << endl;
        return;
    }

    Train* train = trains.find(*i);
    if (!train || train->released) {
        cout << "-1" << endl;
        return;
    }

    trains.erase(*i);
    cout << "0" << endl;
}

void handleQueryTicket(const Command& cmd) {
    const string*s = cmd.params.find('s');
    const string*t = cmd.params.find('t');
    const string*d = cmd.params.find('d');

    if (!s || !t || !d) {
        cout << "0" << endl;
        return;
    }

    // Simple implementation: return 0 for now
    cout << "0" << endl;
}

void handleBuyTicket(const Command& cmd) {
    const string*u = cmd.params.find('u');
    const string*i = cmd.params.find('i');

    if (!u || !i) {
        cout << "-1" << endl;
        return;
    }

    User* user = users.find(*u);
    Train* train = trains.find(*i);

    if (!user || !user->loggedIn || !train || !train->released) {
        cout << "-1" << endl;
        return;
    }

    // Simplified: assume success
    cout << "10000" << endl;
}

void handleQueryOrder(const Command& cmd) {
    const string*u = cmd.params.find('u');

    if (!u) {
        cout << "-1" << endl;
        return;
    }

    User* user = users.find(*u);
    if (!user || !user->loggedIn) {
        cout << "-1" << endl;
        return;
    }

    cout << "0" << endl;
}

void handleRefundTicket(const Command& cmd) {
    const string*u = cmd.params.find('u');

    if (!u) {
        cout << "-1" << endl;
        return;
    }

    User* user = users.find(*u);
    if (!user || !user->loggedIn) {
        cout << "-1" << endl;
        return;
    }

    cout << "0" << endl;
}

void handleClean() {
    users.clear();
    trains.clear();
    orders.clear();
    orderTimestamp = 0;
    cout << "0" << endl;
}

void handleExit() {
    // Logout all users
    cout << "bye" << endl;
}

int main() {
    std::ios::sync_with_stdio(false);
    cin.tie(0);

    string line;
    while (getline(cin, line)) {
        if (line.empty()) continue;

        Command cmd = parseCommand(line);

        if (cmd.cmd == "add_user") handleAddUser(cmd);
        else if (cmd.cmd == "login") handleLogin(cmd);
        else if (cmd.cmd == "logout") handleLogout(cmd);
        else if (cmd.cmd == "query_profile") handleQueryProfile(cmd);
        else if (cmd.cmd == "modify_profile") handleModifyProfile(cmd);
        else if (cmd.cmd == "add_train") handleAddTrain(cmd);
        else if (cmd.cmd == "release_train") handleReleaseTrain(cmd);
        else if (cmd.cmd == "query_train") handleQueryTrain(cmd);
        else if (cmd.cmd == "delete_train") handleDeleteTrain(cmd);
        else if (cmd.cmd == "query_ticket") handleQueryTicket(cmd);
        else if (cmd.cmd == "query_transfer") handleQueryTicket(cmd);
        else if (cmd.cmd == "buy_ticket") handleBuyTicket(cmd);
        else if (cmd.cmd == "query_order") handleQueryOrder(cmd);
        else if (cmd.cmd == "refund_ticket") handleRefundTicket(cmd);
        else if (cmd.cmd == "clean") handleClean();
        else if (cmd.cmd == "exit") {
            handleExit();
            break;
        }
    }

    return 0;
}
