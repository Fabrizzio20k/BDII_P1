#include <iostream>
#include "extendible.h"

using namespace std;

int main(){
	ExtendibleHash eh;

	Record r1 = {1, "Juan", 1};

	eh.insert(r1);

	eh.printAll();

	return 0;
}