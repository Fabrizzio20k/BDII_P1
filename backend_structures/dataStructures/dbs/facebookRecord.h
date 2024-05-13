#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

using namespace std;

struct facebookRecord
{
    string genre;
    string artist_name;
    string track_name;
    string track_id;
    int popularity;
    double acousticness;
    double danceability;
    long duration_ms;
    double energy;
    double instrumentalness;
    double key;
    double liveness;
    double loudness;
    string mode;
    double speechiness;
    double tempo;
    string time_signature;
    double valence;
};
