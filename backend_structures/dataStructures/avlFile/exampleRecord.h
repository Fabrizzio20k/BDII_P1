#ifndef BDII_EXAMPLERECORD_H
#define BDII_EXAMPLERECORD_H

template<typename TK>
struct Record {
	TK id;
	int left = -1;
	int right = -1;
	int height = 0;
};

#endif //BDII_EXAMPLERECORD_H
