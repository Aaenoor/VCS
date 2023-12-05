#include <iostream>
#include <fstream>
#include <vector>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <openssl/sha.h>
#include <algorithm> // Added for find_if

using namespace std;

// Structure to represent a commit
struct Commit {
    string hash;
    string timestamp;
    string message;
    string filename;
    string content;
};

// Class representing a simple version control system
class VersionControlSystem {
private:
    vector<Commit> commitHistory;

public:
    // Function to initialize a new repository
    void initializeRepository() {
        cout << "Repository initialized." << endl;
    }

    // Function to commit changes to a file
    void commitChanges(const string& filename, const string& message) {
        string fileContent = readFileContent(filename);

        // Create a new commit
        Commit commit;
        commit.timestamp = getTimeStamp();
        commit.message = message;
        commit.filename = filename;
        commit.content = fileContent;
        commit.hash = calculateHash(commit);

        // Add commit to history
        commitHistory.push_back(commit);

        cout << "Changes committed to '" << filename << "'." << endl;
    }

    // Function to view commit log
    vector<Commit> viewCommitLog() {
        return commitHistory;
    }

    // Function to revert to a previous version of a file
    void revertToFile(const string& filename, const string& commitHash) {
        // Find the commit with the specified hash
        auto commitIt = find_if(commitHistory.rbegin(), commitHistory.rend(),
                               [filename, commitHash](const Commit& commit) {
                                   return commit.filename == filename && commit.hash == commitHash;
                               });

        if (commitIt != commitHistory.rend()) {
            // Revert the file to the state at the specified commit
            writeFileContent(filename, commitIt->content);
            cout << "File '" << filename << "' reverted to the state at commit " << commitHash << "." << endl;
        } else {
            cout << "Error: Commit with hash '" << commitHash << "' for file '" << filename << "' not found." << endl;
        }
    }

private:
// Function to calculate SHA-256 hash of a commit
string calculateHash(const Commit& commit) {
    string dataToHash = commit.filename + commit.content + commit.timestamp + commit.message;
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);  // Use SHA256_Init instead of SHA256_Init_ex
    SHA256_Update(&sha256, dataToHash.c_str(), dataToHash.length());  // Use SHA256_Update instead of SHA256_Update_ex
    SHA256_Final(hash, &sha256);  // Use SHA256_Final instead of SHA256_Final_ex
    stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        ss << hex << setw(2) << setfill('0') << static_cast<int>(hash[i]);
    }
    return ss.str();
}


    // Function to get current timestamp
    string getTimeStamp() {
        time_t now = time(0);
        tm* localTime = localtime(&now);
        stringstream ss;
        ss << put_time(localTime, "%Y-%m-%d %X");
        return ss.str();
    }

    // Function to read file content
    string readFileContent(const string& filename) {
        ifstream fileStream(filename);
        stringstream buffer;
        buffer << fileStream.rdbuf();
        return buffer.str();
    }

    // Function to write content to a file
    void writeFileContent(const string& filename, const string& content) {
        ofstream fileStream(filename);
        fileStream << content;
    }
};

int main() {
    VersionControlSystem vcs;

    // Example usage
    vcs.initializeRepository();

    // Make changes to the file
    ofstream fileStream("example.txt");
    fileStream << "This is the initial content.";

    // Commit changes
    vcs.commitChanges("example.txt", "Initial commit");

    // Make more changes to the file
    fileStream << " Additional content.";

    // Commit changes
    vcs.commitChanges("example.txt", "Second commit");

    // View commit log
    vector<Commit> commitLog = vcs.viewCommitLog();
    for (const auto& commit : commitLog) {
        cout << "Hash: " << commit.hash << " | Timestamp: " << commit.timestamp
             << " | Message: " << commit.message << " | File: " << commit.filename << endl;
    }

    // Revert to a previous version
    if (commitLog.size() >= 2) {
        vcs.revertToFile("example.txt", commitLog[1].hash);
    }

    return 0;
}
