// A simple visualiser for the arbiter's output - simply pipe it to this program.


#include <iostream>
#include <string>
using namespace std;


const int MAX_SIZE = 50;

bool blocked[MAX_SIZE][MAX_SIZE];
int px[2], py[2], size;
string names[2];

void display();

int main() {
	string data;
	cout.sync_with_stdio(false);

	// name versus name
	cin >> names[0] >> data >> names[1];
	cout << names[0] << ' ' << data << ' ' << names[1] << endl << endl;

	// Size xx
	cin >> data >> size;

	// 0 starts at x y
	cin >> data >> data >> data;
	cin >> px[0] >> py[0];

	// 1 starts at x y
	cin >> data >> data >> data;
	cin >> px[1] >> py[1];

	// 0 RGB xxx xxx xxx
	cin >> data >> data >> data >> data >> data;
	// 1 RGB xxx xxx xxx
	cin >> data >> data >> data >> data >> data;

	while(true) {
		// pid:
		int pid;
		cin >> pid >> data;

		if(data == "wins") {
			cout << names[pid] << " (" << pid << ") wins!" << endl << endl;
			return 0;
		} else if(data == "moved") {
			string dir;
			// dir to x y:
			cin >> dir >> data >> px[pid] >> py[pid];
		} else if(data == "blocked") {
			// x y:
			int x,y;
			cin >> x >> y;
			blocked[y][x] = true;

		} else {
			cout << data << ' ';
			while(cin >> data) cout << data << ' ';
			cout << endl << endl;
			return 1;
		}

		display();
	}
}

const char * SEPARATOR = "";
const char * BOT_CHAR = "OX";
const char * WALL = "█";
const char * EMPTY = "░";

void display() {
	cout << names[0] << " (" << BOT_CHAR[0] << ") versus " << names[1] << " (" << BOT_CHAR[1] << "):\n";
	for(int y = 0; y < size; ++y) {
		for(int x = 0; x < size; ++x) {
			if(y==py[0]&&x==px[0]) cout << BOT_CHAR[0] << SEPARATOR;
			else if(y==py[1]&&x==px[1]) cout << BOT_CHAR[1] << SEPARATOR;
			else if(blocked[y][x]) cout << WALL << SEPARATOR;
			else cout << EMPTY << SEPARATOR;
		}
		cout << '\n';
	}
	cout << endl;
}
