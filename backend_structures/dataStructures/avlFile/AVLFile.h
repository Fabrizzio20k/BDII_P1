#ifndef AVLFILE_CPP
#define AVLFILE_CPP

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <iomanip>
#include "appleRecord.h"

using namespace std;

template <typename TK>
class AVLFile
{
private:
	string filename;
	long pos_root;

public:
	AVLFile(string filename)
	{
		fstream file(filename, ios::binary | ios::in | ios::app);
		this->pos_root = -1;
		this->filename = filename;

		if (file)
		{
			cout << "File already exists\n";
			file.seekg(0, ios::beg);
			file.read((char *)(&pos_root), sizeof(long));
		}
		else
		{
			cout << "File not found, creating new file\n";
			ofstream file2(this->filename, ios::binary);
			file2.write((char *)(&pos_root), sizeof(long));
		}
		file.close();
	}

	vector<Record<TK>> search(TK key)
	{
		fstream file(this->filename, ios::binary | ios::in | ios::out);
		vector<Record<TK>> records;
		search(records, pos_root, key, file);
		file.close();
		return records;
	}

	vector<Record<TK>> rangeSearch(TK beginKey, TK endKey)
	{
		fstream file(this->filename, ios::binary | ios::in | ios::out);
		vector<Record<TK>> records;
		rangeSearch(records, pos_root, beginKey, endKey, file);
		file.close();
		return records;
	}

	bool insert(Record<TK> record)
	{
		fstream file(this->filename, ios::binary | ios::in | ios::out);
		bool res = insert(-1, pos_root, record, file);
		file.close();
		return res;
	}

	bool remove(TK key)
	{
		fstream file(this->filename, ios::binary | ios::in | ios::out);
		bool res = false;
		remove(-1, pos_root, key, file, res);
		file.close();
		return res;
	}

	vector<Record<TK>> inorder()
	{
		fstream file(this->filename, ios::binary | ios::in | ios::out);
		vector<Record<TK>> result;
		inorder(pos_root, result, file);
		file.close();
		return result;
	}

	void clear()
	{
		fstream file(filename, ios::out);
		pos_root = -1;
		file.write((char *)(&pos_root), sizeof(Record<TK>));
		file.close();
	}

	void printAll()
	{
		ifstream file(filename, ios::in | ios::binary);

		long pos = 0;
		Record<TK> record;

		file.seekg(0, ios::beg);
		file.read((char *)(&pos), sizeof(long));

		cout << "Root: " << pos << endl;

		int count = 0;

		while (file.read((char *) (&record), sizeof(Record<TK>)))
			cout << count++ << ": " << record.id << " | " << record.left << " | " << record.right << " | " << record.height << endl;

		file.close();
	}

	void displayPretty()
	{
		fstream file(filename, ios::binary | ios::in | ios::out);
		displayPretty(pos_root, 1, file);
		file.close();
	}

private:
	Record<TK> read_record(long pos, fstream &file)
	{
		if (pos < 0)
			return Record<TK>{};

		file.seekg(sizeof(long) + pos * sizeof(Record<TK>), ios::beg);
		Record<TK> record;
		file.read((char *)&record, sizeof(Record<TK>));

		return record;
	}

	void write_record(long pos, Record<TK> record, fstream &file)
	{
		if (pos < 0)
			return;
		file.seekg(sizeof(long) + pos * sizeof(Record<TK>), ios::beg);
		file.write((char *)&record, sizeof(Record<TK>));
	}

	void search(vector<Record<TK>> &records, long pos_node, int key, fstream &file)
	{
		if (pos_node == -1)
			return;

		Record<TK> record;
		file.seekg(sizeof(long) + pos_node * sizeof(Record<TK>), ios::beg);
		file.read((char *)&record, sizeof(Record<TK>));

		if (key < record.id)
			search(records, record.left, key, file);
		else if (record.id < key)
			search(records, record.right, key, file);
		else
		{
			records.push_back(record);
			search(records, record.left, key, file);
			search(records, record.right, key, file);
		}
	}

	void rangeSearch(vector<Record<TK>> &records, long pos_node, TK beginKey, TK endKey, fstream &file)
	{
		if (pos_node == -1)
			return;

		Record<TK> node = read_record(pos_node, file);

		if (node.id >= beginKey)
			rangeSearch(records, node.left, beginKey, endKey, file);

		if (node.id >= beginKey && node.id <= endKey)
			records.push_back(node);

		if (node.id <= endKey)
			rangeSearch(records, node.right, beginKey, endKey, file);
	}

	bool insert(long parent, long pos_node, Record<TK> record, fstream &file)
	{
		if (pos_node == -1)
		{
			this->pos_root = 0;
			file.seekp(0, ios::beg);
			file.write((char *)(&pos_root), sizeof(long));
			file.write((char *)(&record), sizeof(Record<TK>));
			return true;
		}

		Record<TK> node = read_record(pos_node, file);

		if (record.id < node.id)
		{
			if (node.left == -1)
			{
				file.seekg(0, ios::end);
				long posLeftChildNode = file.tellp() / sizeof(Record<TK>);
				file.write((char *)(&record), sizeof(Record<TK>));

				node.left = posLeftChildNode;
				write_record(pos_node, node, file);
			}
			else
			{
				insert(pos_node, node.left, record, file);
			}
		}
		else if (record.id > node.id)
		{
			if (node.right == -1)
			{
				file.seekg(0, ios::end);
				long posRightChildNode = file.tellp() / sizeof(Record<TK>);
				file.write((char *)(&record), sizeof(Record<TK>));

				node.right = posRightChildNode;
				write_record(pos_node, node, file);
			}
			else
			{
				insert(pos_node, node.right, record, file);
			}
		}
		else
		{
			// Add to the left node always the node with the same root exists
			if (node.left == -1)
			{
				file.seekg(0, ios::end);
				long posLeftChildNode = file.tellp() / sizeof(Record<TK>);
				file.write((char *)(&record), sizeof(Record<TK>));

				node.left = posLeftChildNode;
				write_record(pos_node, node, file);
			}
			else
			{
				insert(pos_node, node.left, record, file);
			}
		}

		balance(parent, pos_node, file);
		return true;
	}

	void remove(long pos_parent, long pos_node, TK key, fstream &file, bool &res)
	{
		if (pos_node == -1)
			return;

		Record<TK> node = read_record(pos_node, file);

		if (key < node.id)
			remove(pos_node, node.left, key, file, res);
		else if (key > node.id)
			remove(pos_node, node.right, key, file, res);
		else
		{
			// Caso 1: Hoja
			if (node.right == -1 && node.left == -1)
			{
				// Si es el único elemento
				if (pos_parent == -1)
				{
					this->pos_root = -1;
					file.seekg(0, ios::beg);
					file.write((char *)&pos_root, sizeof(long));
					res = true;
					return;
				}

				// Si es nodo intermedio
				Record<TK> parentNode = read_record(pos_parent, file);

				if (parentNode.left == pos_node)
					parentNode.left = -1;
				else if (parentNode.right == pos_node)
					parentNode.right = -1;
				else
					cerr << "Error here" << endl;

				write_record(pos_parent, parentNode, file);
				res = true;
				return;
			}
			// Caso 2: hijo en la derecha
			if (node.left == -1)
			{
				long posRight = node.right;

				if (pos_parent == -1)
				{
					Record<TK> rightNode = read_record(posRight, file);
					file.write((char *)&rightNode, sizeof(Record<TK>));
					res = true;
					return;
				}

				Record<TK> parentNode = read_record(pos_parent, file);

				if (parentNode.left == pos_node)
					parentNode.left = posRight;
				else if (parentNode.right == pos_node)
					parentNode.right = posRight;

				write_record(pos_parent, parentNode, file);
				res = true;
				return;
			}

			// Caso 2.1: hijo en la izquierda
			if (node.right == -1)
			{
				long posLeft = node.left;

				if (pos_parent == -1)
				{
					Record<TK> leftNode = read_record(posLeft, file);
					file.write((char *)&leftNode, sizeof(Record<TK>));
					res = true;
					return;
				}

				Record<TK> parentNode = read_record(pos_parent, file);

				if (parentNode.left == pos_node)
					parentNode.left = posLeft;
				else if (parentNode.right == pos_node)
					parentNode.right = posLeft;

				write_record(pos_parent, parentNode, file);
				res = true;
				return;
			}

			// Caso 3: Ambos hijos
			long posSucesor = minValue(node.right, file);

			Record<TK> sucesorNode = read_record(posSucesor, file);

			sucesorNode.left = node.left;
			sucesorNode.right = node.right;

			write_record(pos_node, sucesorNode, file);

			remove(pos_node, node.right, sucesorNode.id, file, res);
		}

		balance(pos_parent, pos_node, file);
	}

	vector<Record<TK>> inorder(long pos_node, vector<Record<TK>> &result, fstream &file)
	{
		if (pos_node == -1)
			return result;

		Record<TK> a;
		file.seekg(sizeof(long) + pos_node * sizeof(Record<TK>), ios::beg);
		file.read((char *)(&a), sizeof(Record<TK>));
		inorder(a.left, result, file);
		result.push_back(a);
		inorder(a.right, result, file);
		return result;
	}

	/* AVL Stuff */

	long minValue(long pos_node, fstream &file)
	{
		if (pos_node == -1)
			return -1;

		Record<TK> node = read_record(pos_node, file);

		if (node.left != -1)
			return minValue(node.left, file);
		else
			return pos_node;
	}

	int height(long pos_node, fstream &file)
	{
		if (pos_node == -1)
			return -1;

		Record<TK> node = read_record(pos_node, file);
		return node.height;
	}

	void displayPretty(long pos_node, int level, fstream &file)
	{
		if (pos_node == -1)
			return;

		int indent = 5;

		Record<TK> node = read_record(pos_node, file);

		displayPretty(node.right, level + 1, file);

		if (level != 0)
		{
			cout << setw(level * indent) << " ";
		}

		cout << node.id << endl;

		displayPretty(node.left, level + 1, file);
	};

	int balancingFactor(long pos_node, fstream &file)
	{
		if (pos_node == -1)
			return 0;

		Record<TK> node = read_record(pos_node, file);
		int h = height(node.left, file) - height(node.right, file);
		return h;
	}

	void updateHeight(long pos_node, fstream &file)
	{
		if (pos_node == -1)
			return;

		Record<TK> node = read_record(pos_node, file);

		node.height = max(height(node.left, file), height(node.right, file)) + 1;

		write_record(pos_node, node, file);
	}

	void left_rotate(long parent, long pos_node, fstream &file)
	{
		Record<TK> node = read_record(pos_node, file);
		int newPos;

		if (balancingFactor(node.left, file) >= 0)
		{
			int posLeft = node.left;

			Record<TK> leftNode = read_record(posLeft, file);

			node.left = leftNode.right;
			leftNode.right = pos_node;

			node.height = max(height(node.left, file), height(node.right, file)) + 1;
			write_record(pos_node, node, file);

			leftNode.height = max(height(leftNode.left, file), height(leftNode.right, file)) + 1;
			write_record(posLeft, leftNode, file);

			newPos = posLeft;
		}
		else
		{
			int posLeft = node.left;

			Record<TK> leftNode = read_record(posLeft, file);

			int posRight = leftNode.right;

			Record<TK> rightNode = read_record(posRight, file);

			leftNode.right = rightNode.left;
			rightNode.left = posLeft;
			node.left = rightNode.right;
			rightNode.right = pos_node;

			node.height = max(height(node.left, file), height(node.right, file)) + 1;
			write_record(pos_node, node, file);

			leftNode.height = max(height(leftNode.left, file), height(leftNode.right, file)) + 1;
			write_record(posLeft, leftNode, file);

			rightNode.height = max(height(rightNode.left, file), height(rightNode.right, file)) + 1;
			write_record(posRight, rightNode, file);

			newPos = posRight;
		}

		if (parent == -1)
		{
			pos_root = newPos;
			file.seekp(0, ios::beg);
			file.write((char *)(&pos_root), sizeof(long));
		}
		else
		{
			Record<TK> newParent = read_record(parent, file);

			if (newParent.left == pos_node)
				newParent.left = newPos;
			else
				newParent.right = newPos;

			write_record(parent, newParent, file);
		}
	}

	void right_rotate(long parent, long pos_node, fstream &file)
	{
		Record<TK> node = read_record(pos_node, file);
		int newPos;

		if (balancingFactor(node.right, file) <= 0)
		{
			int posRight = node.right;

			Record<TK> rightNode = read_record(posRight, file);

			node.right = rightNode.left;
			rightNode.left = pos_node;

			node.height = max(height(node.left, file), height(node.right, file)) + 1;
			write_record(pos_node, node, file);

			rightNode.height = max(height(rightNode.left, file), height(rightNode.right, file)) + 1;
			write_record(posRight, rightNode, file);

			newPos = posRight;
		}
		else
		{
			int posRight = node.right;

			Record<TK> rightNode = read_record(posRight, file);

			int posLeft = rightNode.left;

			Record<TK> leftNode = read_record(posLeft, file);

			rightNode.left = leftNode.right;
			leftNode.right = posRight;
			node.right = leftNode.left;
			leftNode.left = pos_node;

			node.height = max(height(node.left, file), height(node.right, file)) + 1;
			write_record(pos_node, node, file);

			rightNode.height = max(height(rightNode.left, file), height(rightNode.right, file)) + 1;
			write_record(posRight, rightNode, file);

			leftNode.height = max(height(leftNode.left, file), height(leftNode.right, file)) + 1;
			write_record(posLeft, leftNode, file);

			newPos = posLeft;
		}

		if (parent == -1)
		{
			pos_root = newPos;
			file.seekp(0, ios::beg);
			file.write((char *)(&pos_root), sizeof(long));
		}
		else
		{
			Record<TK> newParent = read_record(parent, file);

			if (newParent.left == pos_node)
				newParent.left = newPos;
			else
				newParent.right = newPos;

			write_record(parent, newParent, file);
		}
	}

	void balance(long parent, long pos_node, fstream &file)
	{
		if (pos_node == -1)
			return;

		int h = balancingFactor(pos_node, file);

		if (h > 1)
			left_rotate(parent, pos_node, file);
		else if (h < -1)
			right_rotate(parent, pos_node, file);
		else
			updateHeight(pos_node, file);
	}
};

#endif