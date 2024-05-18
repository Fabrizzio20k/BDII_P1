#include <iostream>
#include <vector>
#include <fstream>
#define MAX_GLOBAL_DEPTH 8
#define BUCKET_SIZE 3

using namespace std;

struct Student
{
    long id;
    char name[30];
    char ciclo[10];
};

struct Metadata
{
};

struct Bucket
{
    long currrentSize{};
    long next = -1;
    vector<long> records;
    bool isFull()
    {
        return currrentSize == BUCKET_SIZE;
    }
};
class ExtendibleHashing
{
private:
    long currentGlobalDepth{};
    string file_name;
    vector<Bucket> directory;
    void writeBucket(Bucket bucket, long index)
    {
        fstream file("data.bin", ios::in | ios::out | ios::binary);
        if (!file.is_open())
        {
            cout << "Error al abrir el archivo" << endl;
        }

        long totalSizeToSave = sizeof(long) * 2 + sizeof(long) * BUCKET_SIZE;

        file.seekp(index * totalSizeToSave);
        file.write((char *)&bucket.currrentSize, sizeof(long));
        file.write((char *)&bucket.next, sizeof(long));
        // write the numbers of the vector and complete with -1 if is not full
        for (int i = 0; i < BUCKET_SIZE; i++)
        {
            if (i < bucket.currrentSize)
            {
                file.write((char *)&bucket.records[i], sizeof(long));
            }
            else
            {
                long aux = -1;
                file.write((char *)&aux, sizeof(long));
            }
        }
        file.close();
    }
    void readBucket(Bucket &bucket, long index)
    {
        fstream file("data.bin", ios::in | ios::binary);
        if (!file.is_open())
        {
            cout << "Error al abrir el archivo" << endl;
        }

        long totalSizeToSave = sizeof(long) * 2 + sizeof(long) * BUCKET_SIZE;

        file.seekg(index * totalSizeToSave);
        file.read((char *)&bucket.currrentSize, sizeof(long));
        file.read((char *)&bucket.next, sizeof(long));
        // only add to the vector the numbers that are not -1
        bucket.records.clear();
        for (int i = 0; i < BUCKET_SIZE; i++)
        {
            long aux;
            file.read((char *)&aux, sizeof(long));
            if (aux != -1)
            {
                bucket.records.push_back(aux);
            }
        }
        file.close();
    }
    void resetAll()
    {
        fstream file("data.bin", ios::out | ios::binary);
        if (!file.is_open())
        {
            cout << "Error al abrir el archivo" << endl;
        }
        file.close();
        // write the two first buckets empty
        Bucket bucket;
        writeBucket(bucket, 0);
        writeBucket(bucket, 1);
    }

public:
    ExtendibleHashing()
    {
        fstream file("data.bin", ios::in | ios::binary);
        if (!file.is_open())
        {
            cout << "Error al abrir el archivo" << endl;
        }
        // get all the directory until the final of the file
        while (!file.eof())
        {
            Bucket bucket;
            readBucket(bucket, directory.size());
            directory.push_back(bucket);
        }
        file.close();
    }
    void printBucketsPretty()
    {
        for (int i = 0; i < directory.size(); i++)
        {
            Bucket bucket;
            readBucket(bucket, directory[i]);
            cout << "Bucket " << i << " : ";
            for (int j = 0; j < bucket.records.size(); j++)
            {
                cout << bucket.records[j] << " ";
            }
            cout << endl;
        }
    }
};
