//
// Created by Benjamin Soto on 17/05/24.
//

#ifndef EXTENDIBLE_H
#define EXTENDIBLE_H
#include <vector>
#include <fstream>
#include <cmath>
#include <valarray>
using namespace  std;

const long block_factor =  5;

struct Record {
    int cod;
    char nombre[12];
    int ciclo;
};


struct Bucket {
    Record records[block_factor]{};
    int sz = 0;
    long  next = -1;
    long local_depth = 0;

    [[nodiscard]] bool is_full() const {
        return sz >= block_factor;
    }

    void add_Record(const Record& record) {
        if( sz == block_factor) return;
        records[sz++] = record;
    }

    void set_ld(long l) {
        this->local_depth = l;
    }
};



/// Vamos a manejar 3 archivos, "metadata.dat" , "directory.dat" y "bucket.dat"


static bool filexist(const string& f){
    ifstream file(f);

    bool ans = file.good();

    file.close();

    return ans;
}

class ExtendibleHashing {
    long global_depth{};
    vector<long> directories; // cada elemento es la posicion fisica de un bucket en el archivo data.dat
    string metadata_file = "metadata.dat";
    string directory_file = "directory.dat";
    string bucket_file = "bucket.dat";
    long last_bucket = -1;

    static void write_bucket(long pos ,Bucket bucket , fstream& file) {
        file.seekg(pos*sizeof(bucket) , ios::beg);
        file.write( reinterpret_cast<char *>(&bucket) , sizeof(Bucket));
    }

    static Bucket read_bucket(long pos ,fstream& file) {
        Bucket bucket;
        file.seekg(pos*sizeof(bucket) , ios::beg);
        file.read(reinterpret_cast<char *>(&bucket) , sizeof(Bucket));
        return bucket;
    }

    static void write_metadata(long global_depth , long last_bucket , fstream& file) {
        file.write(reinterpret_cast<char*>(&global_depth) , sizeof(global_depth));
        file.write(reinterpret_cast<char*>(&last_bucket) , sizeof(last_bucket) );
    }

    static pair<long , long> read_metadata(fstream& file) {
        long glob , last;
        file.seekg(0 , ios::beg);
        file.read(reinterpret_cast<char*>(&glob) , sizeof(glob));
        file.read(reinterpret_cast<char*>(&last) , sizeof(last));
        return {glob , last};
    }

    void write_directories(vector<long>& directories) const {
        ofstream file(this->directory_file , ios::binary | ios::out);
        file.write(reinterpret_cast<char*>(&directories[0]) ,   (directories.size())*sizeof(long) );
        file.close();
    }

    vector<long> read_directories(fstream & file) const {
        int tam = pow(double(2), double(global_depth));
        vector<long> v(tam);
        file.read(reinterpret_cast<char *>(&v[0]) , tam*sizeof(long));
        return v;
    }

    [[nodiscard]] int getDirectoryIndex(int key) const{
        int mask = (1 << this->global_depth) - 1;
        int lsb_k = key & mask;
        return lsb_k;
    }

public:
    explicit ExtendibleHashing(){
        if(!filexist(this->metadata_file)) {
            this->global_depth = 1;
            directories.resize(2 , -1);
            fstream file_meta(metadata_file , ios::in | ios::out | ios::binary);
            write_metadata(global_depth , last_bucket, file_meta);
            file_meta.close();

            fstream file_directory(directory_file , ios::in | ios::out | ios::binary);
            write_directories(directories);
        }
        else {
            fstream file(this->metadata_file , ios::in | ios::binary | ios::out);
            auto meta = read_metadata(file);
            this->global_depth = meta.first;
            this->last_bucket = meta.second;
            file.close();
        }
    }

    void insert(Record& record) {
        int binary_index = getDirectoryIndex(record.cod);
        fstream filedir(this->directory_file , ios::in | ios::out | ios::binary);
        fstream filemeta(this->metadata_file , ios::in | ios::out | ios::binary);
        fstream filebucket(this->bucket_file , ios::in | ios::out | ios::binary);
        auto meta = read_metadata(filemeta);
        this->global_depth = meta.first;
        this->last_bucket = meta.second;
        filemeta.close();
        directories = read_directories(filedir);
        filedir.close();
        if( directories[binary_index] == -1) {
            Bucket new_bucket;
            new_bucket.add_Record(record);
            new_bucket.local_depth = this->global_depth;
            write_bucket(this->last_bucket + 1 ,new_bucket , filebucket);
            this->last_bucket++;
            filebucket.close();
            return;
        }

        Bucket bucket = read_bucket( directories[binary_index] , filebucket);
        if( !bucket.is_full()) {
            bucket.add_Record(record);
            write_bucket(bucket , filebucket);
        }
        else {
            if( bucket.local_depth == this->global_depth) {
                vector<long> new_directories(2*directories.size());

                for(int i = 0 ; i < directories.size() ; i++) {
                    new_directories[i] = directories[i];
                    new_directories[i + directories.size()] = directories[i];
                }
                vector<Record> re_hash(bucket.sz +1);
                for(int i = 0 ; i < bucket.sz ; i++) {
                    re_hash[i] = bucket.records[i];
                }
                re_hash[bucket.sz] = record;

                this->global_depth++;
                bucket.local_depth = global_depth;
                bucket.sz = 0;

                write_bucket(directories[binary_index] , bucket , file_bucket);

                new_directories[binary_index] = last_bucket + 1;
                last_bucket++;
                auto new_bucket = Bucket();

                new_bucket.local_depth = global_depth;

                write_bucket(new_directories[binary_index] , new_bucket , file_bucket);

                write_directories(new_directories);


                this->directories = new_directories;

                for(auto re  : re_hash) {
                    insert(re);
                }

            }
            else {
                split_bucket();
            }
        }

    }

    void split_directory(Bucket& bucket , Record& record , int binary_index , fstream& file_bucket) {

        vector<long> new_directories(2*directories.size());

        for(int i = 0 ; i < directories.size() ; i++) {
            new_directories[i] = directories[i];
            new_directories[i + directories.size()] = directories[i];
        }
        vector<Record> re_hash(bucket.sz +1);
        for(int i = 0 ; i < bucket.sz ; i++) {
            re_hash[i] = bucket.records[i];
        }
        re_hash[bucket.sz] = record;

        this->global_depth++;
        bucket.local_depth = global_depth;
        bucket.sz = 0;

        write_bucket(directories[binary_index] , bucket , file_bucket);

        new_directories[binary_index] = last_bucket + 1;
        last_bucket++;
        auto new_bucket = Bucket();

        new_bucket.local_depth = global_depth;

        write_bucket(new_directories[binary_index] , new_bucket , file_bucket);

        write_directories(new_directories);


        this->directories = new_directories;
    }

    void split_bucket() {

    }



};



#endif //EXTENDIBLE_H
