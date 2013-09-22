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
		cout << data << endl;
		if(data == "wins") {
			cout << names[pid] << " (" << pid << ") wins!" << endl << endl;
			return 0;
		} else if(data == "moved") {
			string dir;
			// dir to x y:
			cin >> dir >> px[pid] >> py[pid];
		} else if(data == "blocked") {
			// x y:
			int x,y;
			cin >> x >> y;
			blocked[y][x] = true;

		} else {
			while(cin >> data) cout << data << ' ';
			cout << endl << endl;
			return 1;
		}

		display();
	}
}

void display() {
	cout << names[0] << " (0) versus " << names[1] << " (1):" << endl;
	for(int y = 0; y < size; ++y) {
		for(int x = 0; x < size; ++x) {
			if(y==py[0]&&x==px[0]) cout << '0';
			else if(y==py[1]&&x==px[1]) cout << '1';
			else if(blocked[y][x]) cout << 'X';
			else cout << '.';
		}
		cout << '\n';
	}
	cout << endl;
}
