#include <iostream>
#include <string>
using namespace std;
#include <unistd.h>
#include "pipe/duopipe.hpp"
#include "ioblock.h"
#include "ChildProcess.hpp"
using namespace cpipe;

int main(int argc, char *argv[]) {
	if(argc == 1) return 1;
	ChildProcess p(argv[1]);


	string data;
	p << "25 0 0 0 24 24" << endl;
	p << "turn" << endl;
	while(p >> data) {
		cout << data << endl;
	}
}
