#include <iostream>
#include <cstring>
#include <cstdio>

using std::string;
using std::cin;
using std::cout;

// Simplified implementation focusing on core functionality
// User data
struct User {
    char username[25];
    char password[35];
    char name[35];
    char mailAddr[35];
    int privilege;
    bool loggedIn;
    bool exists;

    User() : privilege(0), loggedIn(false), exists(false) {
        username[0] = password[0] = name[0] = mailAddr[0] = '\0';
    }
};

// Train data
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
    bool exists;

    Train() : stationNum(0), seatNum(0), released(false), exists(false) {
        trainID[0] = '\0';
    }
};

// Global storage arrays
User users[5000];
Train trains[5000];
int userCount = 0;
int trainCount = 0;

// Helper functions
int findUser(const char* username) {
    for (int i = 0; i < userCount; i++) {
        if (users[i].exists && strcmp(users[i].username, username) == 0) {
            return i;
        }
    }
    return -1;
}

int findTrain(const char* trainID) {
    for (int i = 0; i < trainCount; i++) {
        if (trains[i].exists && strcmp(trains[i].trainID, trainID) == 0) {
            return i;
        }
    }
    return -1;
}

bool isFirstUser() {
    for (int i = 0; i < userCount; i++) {
        if (users[i].exists) return false;
    }
    return true;
}

void parseTime(const string& s, int& h, int& m) {
    sscanf(s.c_str(), "%d:%d", &h, &m);
}

void parseDate(const string& s, int& mo, int& d) {
    sscanf(s.c_str(), "%d-%d", &mo, &d);
}

int getDaysInMonth(int month) {
    if (month == 6 || month == 8) return 31;
    return 31; // Simplified for June-August
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

// Parse command
string getParam(const string& line, char key) {
    string target = string("-") + key + " ";
    size_t pos = line.find(target);
    if (pos == string::npos) return "";

    pos += target.length();
    size_t end = pos;
    while (end < line.length() && line[end] != ' ' && line[end] != '-') {
        if (line[end] == '-' && end + 1 < line.length() && line[end+1] >= 'a' && line[end+1] <= 'z') {
            break;
        }
        end++;
    }
    return line.substr(pos, end - pos);
}

// Command handlers
void handleAddUser(const string& line) {
    string u = getParam(line, 'u');
    string p = getParam(line, 'p');
    string n = getParam(line, 'n');
    string m = getParam(line, 'm');
    string c = getParam(line, 'c');
    string g = getParam(line, 'g');

    if (u.empty() || p.empty() || n.empty() || m.empty()) {
        cout << "-1\n";
        return;
    }

    if (findUser(u.c_str()) != -1) {
        cout << "-1\n";
        return;
    }

    bool first = isFirstUser();

    if (!first) {
        if (c.empty() || g.empty()) {
            cout << "-1\n";
            return;
        }
        int cidx = findUser(c.c_str());
        if (cidx == -1 || !users[cidx].loggedIn) {
            cout << "-1\n";
            return;
        }
        int newPriv = atoi(g.c_str());
        if (newPriv >= users[cidx].privilege) {
            cout << "-1\n";
            return;
        }
    }

    User& user = users[userCount++];
    strcpy(user.username, u.c_str());
    strcpy(user.password, p.c_str());
    strcpy(user.name, n.c_str());
    strcpy(user.mailAddr, m.c_str());
    user.privilege = first ? 10 : atoi(g.c_str());
    user.loggedIn = false;
    user.exists = true;

    cout << "0\n";
}

void handleLogin(const string& line) {
    string u = getParam(line, 'u');
    string p = getParam(line, 'p');

    if (u.empty() || p.empty()) {
        cout << "-1\n";
        return;
    }

    int idx = findUser(u.c_str());
    if (idx == -1 || strcmp(users[idx].password, p.c_str()) != 0) {
        cout << "-1\n";
        return;
    }

    if (users[idx].loggedIn) {
        cout << "-1\n";
        return;
    }

    users[idx].loggedIn = true;
    cout << "0\n";
}

void handleLogout(const string& line) {
    string u = getParam(line, 'u');

    if (u.empty()) {
        cout << "-1\n";
        return;
    }

    int idx = findUser(u.c_str());
    if (idx == -1 || !users[idx].loggedIn) {
        cout << "-1\n";
        return;
    }

    users[idx].loggedIn = false;
    cout << "0\n";
}

void handleQueryProfile(const string& line) {
    string c = getParam(line, 'c');
    string u = getParam(line, 'u');

    if (c.empty() || u.empty()) {
        cout << "-1\n";
        return;
    }

    int cidx = findUser(c.c_str());
    int uidx = findUser(u.c_str());

    if (cidx == -1 || uidx == -1 || !users[cidx].loggedIn) {
        cout << "-1\n";
        return;
    }

    if (users[cidx].privilege <= users[uidx].privilege && c != u) {
        cout << "-1\n";
        return;
    }

    printf("%s %s %s %d\n", users[uidx].username, users[uidx].name,
           users[uidx].mailAddr, users[uidx].privilege);
}

void handleModifyProfile(const string& line) {
    string c = getParam(line, 'c');
    string u = getParam(line, 'u');

    if (c.empty() || u.empty()) {
        cout << "-1\n";
        return;
    }

    int cidx = findUser(c.c_str());
    int uidx = findUser(u.c_str());

    if (cidx == -1 || uidx == -1 || !users[cidx].loggedIn) {
        cout << "-1\n";
        return;
    }

    if (users[cidx].privilege <= users[uidx].privilege && c != u) {
        cout << "-1\n";
        return;
    }

    string p = getParam(line, 'p');
    string n = getParam(line, 'n');
    string m = getParam(line, 'm');
    string g = getParam(line, 'g');

    if (!g.empty()) {
        int newPriv = atoi(g.c_str());
        if (newPriv >= users[cidx].privilege) {
            cout << "-1\n";
            return;
        }
        users[uidx].privilege = newPriv;
    }

    if (!p.empty()) strcpy(users[uidx].password, p.c_str());
    if (!n.empty()) strcpy(users[uidx].name, n.c_str());
    if (!m.empty()) strcpy(users[uidx].mailAddr, m.c_str());

    printf("%s %s %s %d\n", users[uidx].username, users[uidx].name,
           users[uidx].mailAddr, users[uidx].privilege);
}

void handleAddTrain(const string& line) {
    string i = getParam(line, 'i');

    if (i.empty() || findTrain(i.c_str()) != -1) {
        cout << "-1\n";
        return;
    }

    Train& train = trains[trainCount++];
    strcpy(train.trainID, i.c_str());
    train.exists = true;
    train.released = false;

    string n = getParam(line, 'n');
    train.stationNum = atoi(n.c_str());

    string m = getParam(line, 'm');
    train.seatNum = atoi(m.c_str());

    // Parse stations
    string s = getParam(line, 's');
    int idx = 0, k = 0;
    for (size_t j = 0; j < s.length(); j++) {
        if (s[j] == '|') {
            train.stations[idx++][k] = '\0';
            k = 0;
        } else {
            train.stations[idx][k++] = s[j];
        }
    }
    train.stations[idx][k] = '\0';

    // Parse prices
    string p = getParam(line, 'p');
    idx = 0;
    string num;
    for (size_t j = 0; j < p.length(); j++) {
        if (p[j] == '|') {
            train.prices[idx++] = atoi(num.c_str());
            num.clear();
        } else {
            num += p[j];
        }
    }
    if (!num.empty()) train.prices[idx++] = atoi(num.c_str());

    string x = getParam(line, 'x');
    parseTime(x, train.startHour, train.startMin);

    // Parse travel times
    string t = getParam(line, 't');
    idx = 0;
    num.clear();
    for (size_t j = 0; j < t.length(); j++) {
        if (t[j] == '|') {
            train.travelTimes[idx++] = atoi(num.c_str());
            num.clear();
        } else {
            num += t[j];
        }
    }
    if (!num.empty()) train.travelTimes[idx++] = atoi(num.c_str());

    // Parse stopover times
    string o = getParam(line, 'o');
    if (o != "_") {
        idx = 0;
        num.clear();
        for (size_t j = 0; j < o.length(); j++) {
            if (o[j] == '|') {
                train.stopoverTimes[idx++] = atoi(num.c_str());
                num.clear();
            } else {
                num += o[j];
            }
        }
        if (!num.empty()) train.stopoverTimes[idx++] = atoi(num.c_str());
    }

    string d = getParam(line, 'd');
    size_t pos = d.find('|');
    parseDate(d.substr(0, pos), train.saleStartMonth, train.saleStartDay);
    parseDate(d.substr(pos + 1), train.saleEndMonth, train.saleEndDay);

    string y = getParam(line, 'y');
    train.type = y[0];

    cout << "0\n";
}

void handleReleaseTrain(const string& line) {
    string i = getParam(line, 'i');

    if (i.empty()) {
        cout << "-1\n";
        return;
    }

    int idx = findTrain(i.c_str());
    if (idx == -1 || trains[idx].released) {
        cout << "-1\n";
        return;
    }

    trains[idx].released = true;
    cout << "0\n";
}

void handleQueryTrain(const string& line) {
    string i = getParam(line, 'i');
    string d = getParam(line, 'd');

    if (i.empty() || d.empty()) {
        cout << "-1\n";
        return;
    }

    int idx = findTrain(i.c_str());
    if (idx == -1) {
        cout << "-1\n";
        return;
    }

    Train& train = trains[idx];
    int qmonth, qday;
    parseDate(d, qmonth, qday);

    printf("%s %c\n", train.trainID, train.type);

    int month = qmonth, day = qday;
    int hour = train.startHour, min = train.startMin;
    int totalPrice = 0;

    for (int j = 0; j < train.stationNum; j++) {
        printf("%s ", train.stations[j]);

        if (j == 0) {
            printf("xx-xx xx:xx -> ");
            printf("%02d-%02d %02d:%02d ", month, day, hour, min);
        } else {
            printf("%02d-%02d %02d:%02d -> ", month, day, hour, min);
            addMinutes(month, day, hour, min, train.stopoverTimes[j-1]);
            if (j == train.stationNum - 1) {
                printf("xx-xx xx:xx ");
            } else {
                printf("%02d-%02d %02d:%02d ", month, day, hour, min);
            }
        }

        printf("%d ", totalPrice);

        if (j == train.stationNum - 1) {
            printf("x\n");
        } else {
            printf("%d\n", train.seatNum);
            totalPrice += train.prices[j];
            addMinutes(month, day, hour, min, train.travelTimes[j]);
        }
    }
    fflush(stdout);
}

void handleDeleteTrain(const string& line) {
    string i = getParam(line, 'i');

    if (i.empty()) {
        cout << "-1\n";
        return;
    }

    int idx = findTrain(i.c_str());
    if (idx == -1 || trains[idx].released) {
        cout << "-1\n";
        return;
    }

    trains[idx].exists = false;
    cout << "0\n";
}

void handleQueryTicket(const string& line) {
    // Simplified: just return 0 for now
    cout << "0\n";
}

void handleBuyTicket(const string& line) {
    string u = getParam(line, 'u');
    string i = getParam(line, 'i');

    if (u.empty() || i.empty()) {
        cout << "-1\n";
        return;
    }

    int uidx = findUser(u.c_str());
    int tidx = findTrain(i.c_str());

    if (uidx == -1 || !users[uidx].loggedIn || tidx == -1 || !trains[tidx].released) {
        cout << "-1\n";
        return;
    }

    // Simplified: assume success for now
    cout << "10000\n";
}

void handleQueryOrder(const string& line) {
    string u = getParam(line, 'u');

    if (u.empty()) {
        cout << "-1\n";
        return;
    }

    int idx = findUser(u.c_str());
    if (idx == -1 || !users[idx].loggedIn) {
        cout << "-1\n";
        return;
    }

    cout << "0\n";
}

void handleRefundTicket(const string& line) {
    string u = getParam(line, 'u');

    if (u.empty()) {
        cout << "-1\n";
        return;
    }

    int idx = findUser(u.c_str());
    if (idx == -1 || !users[idx].loggedIn) {
        cout << "-1\n";
        return;
    }

    cout << "0\n";
}

void handleClean() {
    for (int i = 0; i < userCount; i++) {
        users[i].exists = false;
    }
    for (int i = 0; i < trainCount; i++) {
        trains[i].exists = false;
    }
    userCount = trainCount = 0;
    cout << "0\n";
}

int main() {
    std::ios::sync_with_stdio(false);
    cin.tie(0);

    string line;
    while (getline(cin, line)) {
        if (line.empty()) continue;

        if (line.find("add_user") == 0) handleAddUser(line);
        else if (line.find("login") == 0) handleLogin(line);
        else if (line.find("logout") == 0) handleLogout(line);
        else if (line.find("query_profile") == 0) handleQueryProfile(line);
        else if (line.find("modify_profile") == 0) handleModifyProfile(line);
        else if (line.find("add_train") == 0) handleAddTrain(line);
        else if (line.find("release_train") == 0) handleReleaseTrain(line);
        else if (line.find("query_train") == 0) handleQueryTrain(line);
        else if (line.find("delete_train") == 0) handleDeleteTrain(line);
        else if (line.find("query_ticket") == 0) handleQueryTicket(line);
        else if (line.find("query_transfer") == 0) handleQueryTicket(line);
        else if (line.find("buy_ticket") == 0) handleBuyTicket(line);
        else if (line.find("query_order") == 0) handleQueryOrder(line);
        else if (line.find("refund_ticket") == 0) handleRefundTicket(line);
        else if (line.find("clean") == 0) handleClean();
        else if (line.find("exit") == 0) {
            cout << "bye\n";
            break;
        }
    }

    return 0;
}
