#include <vector>
#include <fstream>
#include <cmath>
#include <valarray>
#include <iostream>
#include <set>
#include <sstream>
#include <cstring>
#define ll long long
using namespace std;

const long block_factor = 3;

const long D = 3;

template <typename TK>
struct Bucket
{
    Record<TK> records[block_factor];
    int sz = 0;
    long next = -1;
    long local_depth = 0;

    [[nodiscard]] bool is_full()
    {
        return sz >= block_factor;
    }

    void add_Record(Record<TK> record)
    {
        if (sz == block_factor)
            return;
        records[sz] = record;
        this->sz += 1;
    }
};

/// Vamos a manejar 3 archivos, "metadata.dat" , "directory.dat" y "bucket.dat"

static bool filexist(const string &f)
{
    ifstream file(f);

    bool ans = file.good();

    file.close();

    return ans;
}

template <typename TK>
class ExtendibleHash
{
private:
    long global_depth{};
    vector<long> directories; // cada elemento es la posicion fisica de un bucket en el archivo data.dat
    string metadata_file = "metadata.dat";
    string directory_file = "directory.dat";
    string bucket_file = "bucket.dat";
    long last_bucket = -1;

    void write_bucket(long pos, Bucket<TK> bucket, fstream &file)
    {
        file.seekg(pos * sizeof(bucket), ios::beg);
        file.write(reinterpret_cast<char *>(&bucket), sizeof(Bucket<TK>));
    }

    Bucket<TK> read_bucket(long pos, fstream &file)
    {
        Bucket<TK> bucket;
        file.seekg(pos * sizeof(bucket), ios::beg);
        file.read(reinterpret_cast<char *>(&bucket), sizeof(Bucket<TK>));
        return bucket;
    }

    void write_metadata(fstream &file)
    {
        file.seekg(0, ios::beg);
        file.write(reinterpret_cast<char *>(&global_depth), sizeof(this->global_depth));
        file.write(reinterpret_cast<char *>(&last_bucket), sizeof(this->last_bucket));
    }

    pair<long, long> read_metadata(fstream &file)
    {
        long glob, last;
        file.seekg(0, ios::beg);
        file.read(reinterpret_cast<char *>(&glob), sizeof(glob));
        file.read(reinterpret_cast<char *>(&last), sizeof(last));
        return {glob, last};
    }

    void write_directories(fstream &file)
    {
        file.seekg(0, ios::beg);
        file.write(reinterpret_cast<char *>(this->directories.data()), (this->directories.size()) * sizeof(long));
    }

    vector<long> read_directories(fstream &file)
    {
        int tam = pow(double(2), double(global_depth));
        vector<long> v(tam);
        file.seekg(0, ios::beg);
        file.read(reinterpret_cast<char *>(v.data()), tam * sizeof(long));
        return v;
    }

    [[nodiscard]] int getDirectoryIndex(int key)
    {
        int mask = (1 << this->global_depth) - 1;
        int lsb_k = key & mask;
        return lsb_k;
    }

    int f(int n, int k)
    {
        int mask = (1 << k) - 1;
        int lsb_k = n & mask;
        return lsb_k;
    }

    void insert(Record<TK> r, fstream &filemeta, fstream &filedir, fstream &filebucket)
    {
        int binary_index = getDirectoryIndex(r.id);
        auto meta = read_metadata(filemeta);
        this->global_depth = meta.first, this->last_bucket = meta.second;

        this->directories = read_directories(filedir);

        if (directories[binary_index] == -1)
        {
            Bucket<TK> new_bucket;
            directories[binary_index] = this->last_bucket + 1;
            this->last_bucket++;
            new_bucket.add_Record(r);
            new_bucket.local_depth = this->global_depth;
            write_bucket(directories[binary_index], new_bucket, filebucket);
            write_directories(filedir);
            write_metadata(filemeta);
            return;
        }

        Bucket<TK> actual_bucket = read_bucket(directories[binary_index], filebucket);

        if (!actual_bucket.is_full())
        {
            actual_bucket.add_Record(r);
            write_bucket(directories[binary_index], actual_bucket, filebucket);
        }
        else
        {
            if (this->global_depth == D)
            {
                if (this->global_depth == actual_bucket.local_depth)
                {
                    Bucket<TK> b = actual_bucket;
                    long pos_b = directories[binary_index];
                    while (b.next != -1)
                    {
                        pos_b = b.next;
                        b = read_bucket(b.next, filebucket);
                    }

                    if (!b.is_full())
                    {
                        b.add_Record(r);
                        write_bucket(pos_b, b, filebucket);
                        return;
                    }

                    b.next = this->last_bucket + 1;

                    Bucket<TK> new_bucket = Bucket<TK>();
                    new_bucket.local_depth = b.local_depth;
                    new_bucket.add_Record(r);

                    write_bucket(b.next, new_bucket, filebucket);
                    write_bucket(pos_b, b, filebucket);
                    this->last_bucket++;

                    write_metadata(filemeta);
                }
                else if (actual_bucket.local_depth < global_depth)
                {
                    // traer los records a rehashear a ram
                    vector<Record<TK>> r_hash(actual_bucket.sz + 1);
                    for (int i = 0; i < actual_bucket.sz; i++)
                    {
                        r_hash[i] = actual_bucket.records[i];
                    }
                    r_hash[actual_bucket.sz] = r;

                    // vaciar el actual_bucket
                    actual_bucket.sz = 0;
                    actual_bucket.local_depth++;
                    write_bucket(directories[binary_index], actual_bucket, filebucket);

                    // crear un nuevo bucket
                    directories[binary_index] = this->last_bucket + 1;
                    directories[binary_index] = this->last_bucket + 1;

                    for (int i = 0; i < directories.size(); i++)
                    {
                        if (f(i, actual_bucket.local_depth) == binary_index)
                        {
                            directories[i] = this->last_bucket + 1;
                        }
                    }
                    Bucket<TK> new_bucket = Bucket<TK>();

                    new_bucket.local_depth = actual_bucket.local_depth;
                    new_bucket.sz = 0;
                    // insertar en la posicion directories[binary_index]

                    write_bucket(directories[binary_index], new_bucket, filebucket);
                    this->last_bucket++;

                    write_metadata(filemeta);
                    write_directories(filedir);

                    for (Record<TK> re : r_hash)
                    {
                        insert(re, filemeta, filedir, filebucket);
                    }
                }
                else
                {
                    cout << "ERROR, ESTO NUNCA DEBERIA PASAR" << endl;
                    return;
                }

                return;
            }

            if (this->global_depth == actual_bucket.local_depth)
            {
                this->directories = read_directories(filedir);
                vector<long> new_directories(2 * directories.size());
                for (int i = 0; i < directories.size(); i++)
                {
                    new_directories[i] = directories[i];
                    new_directories[i + directories.size()] = directories[i];
                }

                // crear un nuevo bucket e inserar en la posicion new_directories[binary_index]
                new_directories[binary_index] = this->last_bucket + 1;
                Bucket<TK> new_bucket = Bucket<TK>();
                new_bucket.local_depth = actual_bucket.local_depth + 1;
                write_bucket(new_directories[binary_index], new_bucket, filebucket);

                // insetar los records de actual bucket y el record a insertar en rehash
                vector<Record<TK>> rehash(actual_bucket.sz + 1);

                for (int i = 0; i < actual_bucket.sz; i++)
                {
                    rehash[i] = actual_bucket.records[i];
                }
                rehash[actual_bucket.sz] = r;

                // for (auto re : rehash)
                // {
                //     re.print();
                // }

                // actualizar el size del bucket actual a 0(vacio) y la profundidad local aumenta en 1
                actual_bucket.sz = 0;
                actual_bucket.local_depth++;
                write_bucket(directories[binary_index], actual_bucket, filebucket);

                this->global_depth++;
                this->last_bucket++;

                write_metadata(filemeta);

                this->directories.clear();
                this->directories = new_directories;

                write_directories(filedir);

                for (auto re : rehash)
                {
                    insert(re, filemeta, filedir, filebucket);
                }
            }
            else if (actual_bucket.local_depth < global_depth)
            {
                // traer los records a rehashear a ram
                vector<Record<TK>> r_hash(actual_bucket.sz + 1);
                for (int i = 0; i < actual_bucket.sz; i++)
                {
                    r_hash[i] = actual_bucket.records[i];
                }
                r_hash[actual_bucket.sz] = r;

                // vaciar el actual_bucket
                actual_bucket.sz = 0;
                actual_bucket.local_depth++;
                write_bucket(directories[binary_index], actual_bucket, filebucket);

                // crear un nuevo bucket
                directories[binary_index] = this->last_bucket + 1;

                for (int i = 0; i < directories.size(); i++)
                {
                    if (f(i, actual_bucket.local_depth) == binary_index)
                    {
                        directories[i] = this->last_bucket + 1;
                    }
                }

                Bucket<TK> new_bucket = Bucket<TK>();

                new_bucket.local_depth = actual_bucket.local_depth;
                new_bucket.sz = 0;
                // insertar en la posicion directories[binary_index]

                write_bucket(directories[binary_index], new_bucket, filebucket);
                this->last_bucket++;

                write_metadata(filemeta);
                write_directories(filedir);

                for (Record<TK> re : r_hash)
                {
                    insert(re, filemeta, filedir, filebucket);
                }
            }
            else
            {
                cout << "ERROR, ESTO NUNCA DEBERIA PASAR" << endl;
                return;
            }
        }
    }

    Record<TK> search(ll key, fstream &filemeta, fstream &filedir, fstream &filebucket)
    {
        int binary_index = getDirectoryIndex(key);
        this->directories = read_directories(filedir);
        Bucket<TK> bucket = read_bucket(directories[binary_index], filebucket);

        Bucket<TK> current = bucket;

        while (current.next != -1)
        {
            for (int i = 0; i < current.sz; i++)
            {
                if (current.records[i].id == key)
                {
                    return current.records[i];
                }
            }

            current = read_bucket(current.next, filebucket);
        }

        for (int i = 0; i < current.sz; i++)
        {
            if (current.records[i].id == key)
            {
                return current.records[i];
            }
        }

        return Record<TK>{};
    }

public:
    void create_file_if_not_exist(const string &filename)
    {
        fstream file(filename, ios::out | ios::binary);
        file.close();
    }

    void clear()
    {
        remove("metadata.dat");
        remove("directory.dat");
        remove("bucket.dat");

        this->global_depth = 1;
        this->directories.resize(2, -1);

        create_file_if_not_exist(metadata_file);
        create_file_if_not_exist(directory_file);
        create_file_if_not_exist(bucket_file);

        fstream file_meta(metadata_file, ios::in | ios::out | ios::binary);
        fstream filedir(directory_file, ios::in | ios::out | ios::binary);

        write_metadata(file_meta);
        write_directories(filedir);

        file_meta.close();
        filedir.close();
    }

    ExtendibleHash()
    {
        if (!filexist(this->metadata_file))
        {

            this->global_depth = 1;
            this->directories.resize(2, -1);

            create_file_if_not_exist(metadata_file);
            create_file_if_not_exist(directory_file);
            create_file_if_not_exist(bucket_file);

            fstream file_meta(metadata_file, ios::in | ios::out | ios::binary);
            fstream filedir(directory_file, ios::in | ios::out | ios::binary);

            write_metadata(file_meta);
            write_directories(filedir);

            file_meta.close();
            filedir.close();
        }
        else
        {
            fstream file(this->metadata_file, ios::in | ios::binary | ios::out);
            auto meta = read_metadata(file);
            this->global_depth = meta.first;
            this->last_bucket = meta.second;
            file.close();
        }
    }

    void insert(Record<TK> record)
    {
        fstream filemeta(this->metadata_file, ios::in | ios::out | ios::binary);
        fstream filedir(this->directory_file, ios::in | ios::out | ios::binary);
        fstream filebucket(this->bucket_file, ios::in | ios::out | ios::binary);

        insert(record, filemeta, filedir, filebucket);

        filemeta.close();
        filedir.close();
        filebucket.close();
    }

    Record<TK> search(ll key)
    {
        fstream filemeta(this->metadata_file, ios::in | ios::out | ios::binary);
        fstream filedir(this->directory_file, ios::in | ios::out | ios::binary);
        fstream filebucket(this->bucket_file, ios::in | ios::out | ios::binary);

        auto r = search(key, filemeta, filedir, filebucket);

        filemeta.close();
        filedir.close();
        filebucket.close();

        return r;
    }

    vector<Record<TK>> get_all()
    {
        fstream filedir(this->directory_file, ios::in | ios::out | ios::binary);
        fstream filebucket(this->bucket_file, ios::in | ios::out | ios::binary);
        vector<Record<TK>> ans;
        set<long> positions;
        this->directories = read_directories(filedir);
        for (auto pos_bucket : directories)
        {
            positions.insert(pos_bucket);
        }

        for (auto pos : positions)
        {
            auto current = read_bucket(pos, filebucket);
            while (current.next != -1)
            {
                for (int i = 0; i < current.sz; i++)
                {
                    ans.push_back(current.records[i]);
                }
                current = read_bucket(current.next, filebucket);
            }

            for (int i = 0; i < current.sz; i++)
            {
                ans.push_back(current.records[i]);
            }
        }

        filedir.close();
        filebucket.close();

        return ans;
    }

    void printAll()
    {
        fstream filebucket(this->bucket_file, ios::in);
        for (int i = 0; i < directories.size(); i++)
        {
            cout << "index: " << i << endl;
            cout << "pos_bucket: " << directories[i] << ": ";

            Bucket<TK> bucket = read_bucket(directories[i], filebucket);
            cout << "bucket size: " << bucket.sz << " local_depth: " << bucket.local_depth << endl;
            for (int j = 0; j < bucket.sz; j++)
            {
                cout << bucket.records[j].id << " | ";
            }
            cout << endl;
        }

        cout << endl;

        filebucket.close();
    }
};