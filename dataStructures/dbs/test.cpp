#include <iostream>
#include <fstream>
#include "facebookRecord.h"

using namespace std;

int main()
{
    ifstream in("SpotifyFeatures.csv");
    int counter = 0;
    cout << "Reading file..." << endl;

    cout << "first 10 records:" << endl;
    facebookRecord record;
    while (in >> record)
    {
        if (counter < 10)
        {
            cout << record.genre << " " << record.artist_name << " " << record.track_name << " " << record.track_id << " " << record.popularity << " " << record.acousticness << " " << record.danceability << " " << record.duration_ms << " " << record.energy << " " << record.instrumentalness << " " << record.key << " " << record.liveness << " " << record.loudness << " " << record.mode << " " << record.speechiness << " " << record.tempo << " " << record.time_signature << " " << record.valence << endl;
        }
        counter++;
    }
    return 0;
}