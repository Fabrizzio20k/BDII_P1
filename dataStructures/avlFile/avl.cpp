#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <iomanip>

using namespace std;

template<typename TK>
struct Record
{
	TK cod;
	char nombre[12];
	int ciclo;
	long left = -1, right = -1;
	int height = 0;

	void setData()
	{
		cout << "Codigo:";
		cin >> cod;
		cout << "Nombre: ";
		cin >> nombre;
		cout << "Ciclo: ";
		cin >> ciclo;
	}

	void showData()
	{
		cout << "\nCodigo: " << cod;
		cout << "\nNombre: " << nombre;
		cout << "\nCiclo : " << ciclo << endl;
		cout << "Izq : " << left << endl;
		cout << "Der : " << right << endl;
		cout << "Altura : " << height << endl;
	}
};

template<typename TK>
class AVLFile{
private:
	string filename;
	long pos_root;
public:
	AVLFile(string filename){
		fstream file(filename, ios::binary | ios::in | ios::app);
		this->pos_root = -1;
		this->filename = filename;
		if (!file)
		{
			cout << "File not found, creating new file\n";
			ofstream file2(this->filename, ios::binary);
			file2.write((char *)(&pos_root), sizeof(long));
			file2.close();
		} else {
			file.read((char *)(&pos_root), sizeof(long));
		}
		file.close();
	}

	vector<Record<TK>> search(TK key){
		fstream file(this->filename, ios::binary | ios::in | ios::out);
		vector<Record<TK>> records;
		search(records, pos_root, key, file);
		file.close();
		return records;
	}

	void insert(Record<TK> record){
		fstream file(this->filename, ios::binary | ios::in | ios::out);
		insert(-1, pos_root, record, file);
		file.close();
	}

	vector<Record<TK>> inorder(){
		fstream file(this->filename, ios::binary | ios::in | ios::out);
		vector<Record<TK>> result;
		inorder(pos_root, result, file);
		file.close();
		return result;
	}

	void clear(){
		fstream file(filename, ios::out);
		file.close();
	}

	void printAll(){
		ifstream file(filename, ios::in | ios::binary);

		long pos = 0;
		Record<TK> record;

		file.seekg(0, ios::beg);
		file.read((char *)(&pos), sizeof(long));

		cout << "Root: " << pos << endl;

		int count = 0;
		while (file.read((char *)(&record), sizeof(Record<TK>)))
			cout << count++ << ": " << record.cod << " | " << record.nombre << " | " << record.ciclo << " | " << record.left << " | " << record.right << " | " << record.height << endl;

		file.close();
	}

	void displayPretty(){
		fstream file(filename, ios::binary | ios::in | ios::out);
		displayPretty(pos_root, 1, file);
		file.close();
	}

private:
	void search(vector<Record<TK>>& records, long pos_node, int key, fstream &file)
	{
		if (pos_node == -1) return;

		Record<TK> record;
		file.seekg(sizeof(long) + pos_node * sizeof(Record<TK>), ios::beg);
		file.read((char *)&record, sizeof(Record<TK>));

		if (key < record.cod)
			search(records, record.left, key, file);
		else if (record.cod < key)
			search(records, record.right, key, file);
		else {
			records.push_back(record);
			search(records, record.left, key, file);
			search(records, record.right, key, file);
		}
	}

	void insert(long parent, long pos_node, Record<TK> record, fstream &file)
	{
		if (pos_node == -1)
		{
			this->pos_root = 0;
			file.seekp(0, ios::beg);
			file.write((char *)(&pos_root), sizeof(long));
			file.write((char *)(&record), sizeof(Record<TK>));
			return;
		}
		Record<TK> a;
		file.seekg(sizeof(long) + pos_node * sizeof(Record<TK>), ios::beg);
		file.read((char *)(&a), sizeof(Record<TK>));

		if (record.cod > a.cod){
			if (a.right == -1){
				int indx = 0;
				file.seekg(0, ios::end);
				indx = file.tellp() / sizeof(Record<TK>);
				file.write((char *)(&record), sizeof(Record<TK>));

				file.seekg(sizeof(int) + (pos_node + 1) * sizeof(Record<TK>) - (sizeof(long) + sizeof(int)), ios::beg);
				file.write((char *)(&indx), sizeof(int));
			} else {
				insert(pos_node, a.right, record, file);
			}
		} else if (record.cod < a.cod){
			if (a.left == -1){
				int indx = 0;
				file.seekg(0, ios::end);
				indx = file.tellg() / sizeof(Record<TK>);
				file.write((char *)(&record), sizeof(Record<TK>));

				file.seekg(sizeof(int) + (pos_node + 1) * sizeof(Record<TK>) - (2 * sizeof(long) + sizeof(int)), ios::beg);
				file.write((char *)(&indx), sizeof(int));
			} else {
				insert(pos_node, a.left, record, file);
			}
		} else {
			// Add to the left node always the node with the same root exists
			if (a.left == -1){
				int indx = 0;
				file.seekg(0, ios::end);
				indx = file.tellg() / sizeof(Record<TK>);
				file.write((char *)(&record), sizeof(Record<TK>));

				file.seekg(sizeof(int) + (pos_node + 1) * sizeof(Record<TK>) - (2 * sizeof(long) + sizeof(int)), ios::beg);
				file.write((char *)(&indx), sizeof(int));
			} else {
				insert(pos_node, a.left, record, file);
			}
		}

		balance(parent, pos_node, file);
	}

	vector<Record<TK>> inorder(long pos_node, vector<Record<TK>> &result, fstream &file)
	{
		if (pos_node == -1) return result;

		Record<TK> a;
		file.seekg(sizeof(long) + pos_node * sizeof(Record<TK>), ios::beg);
		file.read((char *)(&a), sizeof(Record<TK>));
		inorder(a.left, result, file);
		result.push_back(a);
		inorder(a.right, result, file);
		return result;
	}

	/* AVL Stuff */

	int height(long pos_node, fstream &file)
	{
		if (pos_node == -1) return -1;

		Record<TK> a;
		file.seekg(sizeof(long) + pos_node * sizeof(Record<TK>), ios::beg);
		file.read((char *)(&a), sizeof(Record<TK>));
		return a.height;
	}

	void displayPretty(long pos_node, int level, fstream &file){
		if (pos_node == -1)
			return;

		int indent = 4;

		Record<TK> a;
		file.seekg(sizeof(long) + pos_node * sizeof(Record<TK>), ios::beg);
		file.read((char *)(&a), sizeof(Record<TK>));

		displayPretty(a.right, level + 1, file);

		if (level != 0) {
			cout << setw(level * indent) << " ";
		}

		cout << a.cod << endl;

		displayPretty(a.left, level + 1, file);
	};

	int balancingFactor(long pos_node, fstream &file)
	{
		if (pos_node == -1) return 0;

		Record<TK> a;
		file.seekg(sizeof(long) + pos_node * sizeof(Record<TK>), ios::beg);
		file.read((char *)(&a), sizeof(Record<TK>));

		int h = height(a.left, file) - height(a.right, file);

		return h;
	}

	void updateHeight(long pos_node, fstream &file)
	{
		if (pos_node == -1) return;

		Record<TK> a;
		file.seekg(sizeof(long) + pos_node * sizeof(Record<TK>), ios::beg);
		file.read((char *)(&a), sizeof(Record<TK>));

		a.height = max(height(a.left, file), height(a.right, file)) + 1;

		file.seekp(sizeof(long) + pos_node * sizeof(Record<TK>), ios::beg);
		file.write((char *)(&a), sizeof(Record<TK>));
	}

	// LR rotation
	int left_rotate(long pos_node, fstream &file)
	{
		Record<TK> a;
		file.seekg(sizeof(long) + pos_node * sizeof(Record<TK>), ios::beg);
		file.read((char *)(&a), sizeof(Record<TK>));

		if (balancingFactor(a.left, file) >= 0){
			int n2 = a.left;

			Record<TK> b;
			file.seekg(sizeof(long) + n2 * sizeof(Record<TK>), ios::beg);
			file.read((char *)(&b), sizeof(Record<TK>));

			a.left = b.right;
			b.right = pos_node;

			a.height = max(height(a.left, file), height(a.right, file)) + 1;
			file.seekp(sizeof(long) + pos_node * sizeof(Record<TK>), ios::beg);
			file.write((char *)(&a), sizeof(Record<TK>));

			b.height = max(height(b.left, file), height(b.right, file)) + 1;
			file.seekp(sizeof(long) + n2 * sizeof(Record<TK>), ios::beg);
			file.write((char *)(&b), sizeof(Record<TK>));

			return n2;
		} else {
			int n2 = a.left;

			Record<TK> b;
			file.seekg(sizeof(long) + n2 * sizeof(Record<TK>), ios::beg);
			file.read((char *)(&b), sizeof(Record<TK>));

			int n3 = b.right;

			Record<TK> c;
			file.seekg(sizeof(long) + n3 * sizeof(Record<TK>), ios::beg);
			file.read((char *)(&c), sizeof(Record<TK>));

			b.right = c.left;
			c.left = n2;
			a.left = c.right;
			c.right = pos_node;

			a.height = max(height(a.left, file), height(a.right, file)) + 1;
			file.seekp(sizeof(long) + pos_node * sizeof(Record<TK>), ios::beg);
			file.write((char *)(&a), sizeof(Record<TK>));

			b.height = max(height(b.left, file), height(b.right, file)) + 1;
			file.seekp(sizeof(long) + n2 * sizeof(Record<TK>), ios::beg);
			file.write((char *)(&b), sizeof(Record<TK>));

			c.height = max(height(c.left, file), height(c.right, file)) + 1;
			file.seekp(sizeof(long) + n3 * sizeof(Record<TK>), ios::beg);
			file.write((char *)(&c), sizeof(Record<TK>));

			return n3;
		}
	}

	int right_rotate(long pos_node, fstream &file)
	{
		Record<TK> a;
		file.seekg(sizeof(long) + pos_node * sizeof(Record<TK>), ios::beg);
		file.read((char *)(&a), sizeof(Record<TK>));

		if (balancingFactor(a.right, file) <= 0){
			int n2 = a.right;

			Record<TK> b;
			file.seekg(sizeof(long) + n2 * sizeof(Record<TK>), ios::beg);
			file.read((char *)(&b), sizeof(Record<TK>));

			a.right = b.left;
			b.left = pos_node;

			a.height = max(height(a.left, file), height(a.right, file)) + 1;
			file.seekp(sizeof(long) + pos_node * sizeof(Record<TK>), ios::beg);
			file.write((char *)(&a), sizeof(Record<TK>));

			b.height = max(height(b.left, file), height(b.right, file)) + 1;
			file.seekp(sizeof(long) + n2 * sizeof(Record<TK>), ios::beg);
			file.write((char *)(&b), sizeof(Record<TK>));

			return n2;
		} else {
			int n2 = a.right;

			Record<TK> b;
			file.seekg(sizeof(long) + n2 * sizeof(Record<TK>), ios::beg);
			file.read((char *)(&b), sizeof(Record<TK>));

			int n3 = b.left;

			Record<TK> c;
			file.seekg(sizeof(long) + n3 * sizeof(Record<TK>), ios::beg);
			file.read((char *)(&c), sizeof(Record<TK>));

			b.left = c.right;
			c.right = n2;
			a.right = c.left;
			c.left = pos_node;

			a.height = max(height(a.left, file), height(a.right, file)) + 1;
			file.seekp(sizeof(long) + pos_node * sizeof(Record<TK>), ios::beg);
			file.write((char *)(&a), sizeof(Record<TK>));

			b.height = max(height(b.left, file), height(b.right, file)) + 1;
			file.seekp(sizeof(long) + n2 * sizeof(Record<TK>), ios::beg);
			file.write((char *)(&b), sizeof(Record<TK>));

			c.height = max(height(c.left, file), height(c.right, file)) + 1;
			file.seekp(sizeof(long) + n3 * sizeof(Record<TK>), ios::beg);
			file.write((char *)(&c), sizeof(Record<TK>));

			return n3;
		}
	}

	void balance(long parent, long pos_node, fstream &file)
	{
		if (pos_node == -1) return;

		int h = balancingFactor(pos_node, file);
		int l = -1, r = -1;

		if (h > 1)
			l = left_rotate(pos_node, file);
		else if (h < -1)
			r = right_rotate(pos_node, file);
		else
			updateHeight(pos_node, file);

		if (parent == -1 && (l != -1 || r != -1)){
			pos_root = (l == -1) ? r : l;
			file.seekp(0, ios::beg);
			file.write((char *)(&pos_root), sizeof(long));
		} else if (parent != -1 && (l != -1 || r != -1)){
			Record<TK> a;
			file.seekg(sizeof(long) + parent * sizeof(Record<TK>), ios::beg);
			file.read((char *)(&a), sizeof(Record<TK>));

			if (r != -1) a.right = r;
			else a.left = l;

			file.seekp(sizeof(long) + parent * sizeof(Record<TK>), ios::beg);
			file.write((char *)(&a), sizeof(Record<TK>));
		}
	}
};

void test(string filename){
	AVLFile<int> file(filename);

//	file.clear();
	Record<int> r0 = {1000, "Juan", 1};
//	Record<int> r1 = {1001, "Maria", 2};
//	Record<int> r2 = {1002, "Luis", 3};
//	Record<int> r3 = {1003, "Jose", 4};
//	Record<int> r4 = {1004, "Miguel", 5};
//	Record<int> r5 = {1005, "Fabricio", 5};
//	Record<int> r6 = {1006, "Carlos", 6};
//	Record<int> r7 = {1007, "Pedro", 7};
//	Record<int> r8 = {1008, "Jorge", 8};
//	Record<int> r9 = {1009, "Raul", 9};
//
	file.insert(r0);
//	file.insert(r1);
//	file.insert(r2);
//	file.insert(r4);
//	file.insert(r3);
//	file.insert(r6);
//	file.insert(r5);
//	file.insert(r8);
//	file.insert(r7);
//	file.insert(r9);

	file.printAll();
	cout << endl;
	file.displayPretty();

//	cout << "--------- find data -----------\n";
//	vector<Record<int>> records = file.search(1000);
//
//	for (auto &r : records)
//		r.showData();
}

int main(){
	test("data1.dat");
	return 0;
}
