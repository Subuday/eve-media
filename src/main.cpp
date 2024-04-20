#include <App.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <thread>
#include <chrono>

using namespace std;

void readFile() {
    this_thread::sleep_for(chrono::seconds(10));

    ifstream file("music_out.wav", ios::binary | ios::in);
    if (!file.is_open()) {
        cerr << "Error opening file" << endl;
        return;
    }

    file.seekg(44, std::ios::beg);

    vector<uint8_t> audio;
    const size_t bufferSize = 4096;
    char buffer[bufferSize];
    const size_t maxReadSize = 10 * 1024 * 1024; 
    while (file.read(buffer, bufferSize) || file.gcount() > 0) {
        audio.insert(audio.end(), buffer, buffer + file.gcount());
        if (audio.size() >= maxReadSize) {
            cout << "Read limit of 10 MB reached, stopping read." << endl;
            break;
       	} 
    }

    file.close();

    const size_t bytesPerSample = 2 * 2;
    const size_t samplesPerChunk = 1024;
    const size_t chunkSize = samplesPerChunk * bytesPerSample;
    const size_t totalChunks = (audio.size() + chunkSize - 1) / chunkSize; 

    cout << "Audio total size in bytes " << audio.size() << endl;
    cout << "Player has total chunks " << totalChunks << endl;

    App& app = App::instance();

    for (size_t i = 0; i < totalChunks; ++i) {
        size_t start = i * chunkSize;
        size_t end = min(start + chunkSize, audio.size());

        vector<uint8_t> chunk(audio.begin() + start, audio.begin() + end);
        
        app.getMediaManager()->write(chunk);
    }

    cout << "Player all chunks are written. " << totalChunks << endl;

    this_thread::sleep_for(chrono::seconds(30));
}

void writeFile() {
    this_thread::sleep_for(chrono::seconds(10));

    ofstream file("music_out_2.pcm", ios::binary);
    if (!file.is_open()) {
        cerr << "Failed to create file." << endl;
        return;
    }

    App& app = App::instance();
    while (true) {
        vector<uint8_t> chunk = app.getMediaManager()->read();
        if (chunk.size() > 0) {
            file.write(reinterpret_cast<const char*>(chunk.data()), chunk.size());
        }
    }

    file.close();
}

int main(int argc, char *argv[]) {
    App& app = App::instance();
    app.start();

    thread pt(readFile);
    thread rt(writeFile);

    rt.join();
    pt.join();
 
    return 0;
}
