#include "TROOT.h"
#include "TChain.h"
#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include "TLeaf.h"
#include <iostream>
#include <fstream>
#include <string>
#include <chrono>

using namespace std;

typedef chrono::time_point<chrono::steady_clock> time_point_t;

void print_measurement(const time_point_t started_at, const Long64_t entries_count, const Long64_t current_entry_number) {
    time_point_t current_time = chrono::steady_clock::now();
    double elapsed      = chrono::duration<double>(current_time - started_at).count();
    double per_entry    = elapsed / current_entry_number;
    int remaining       = int(per_entry * (entries_count - current_entry_number));

    int elapsed_i         = int(elapsed);
    int elapsed_seconds   = elapsed_i % 60;
    int elapsed_minutes   = (elapsed_i / 60) % 60;
    int elapsed_hours     = elapsed_i / 60 / 60;

    int remaining_seconds   = remaining % 60;
    int remaining_minutes   = (remaining / 60) % 60;
    int remaining_hours     = remaining / 60 / 60;

    cout    << current_entry_number << " / " << entries_count << " |   "
            << "Elapsed: "   << elapsed_hours << "h " << elapsed_minutes << "m " << elapsed_seconds << "sec / "
            << "Per entry: " << per_entry << " sec / "
            << "Remaining: " << remaining_hours << "h " << remaining_minutes << "m " << remaining_seconds << "sec "
            << endl;

    cout.flush();
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        cout << "cern_get INPUT_FILE CONFIG_FILE OUTPUT_FILE" << endl;
        return 0;
    }

    cout << "Processing " << argv[1] << "..." << endl;
    ifstream input_file(argv[1]);
    TChain *chain = new TChain("Events");

    string buffer;
    while (getline(input_file, buffer)) {
        cout << "Append file: " << buffer << endl;
        chain->Add(buffer.c_str());
    }

    cout << "Select branches... " << endl;
    ifstream config_file(argv[2]);
    vector<string> branches;
    while (getline(config_file, buffer)) {
        branches.push_back(buffer);
    }

    chain->SetBranchStatus("*", 0);
    for (vector<string>::iterator it = branches.begin(); it != branches.end(); it++) {
        chain->SetBranchStatus(it->c_str(), 1);
        cout << *it << " - ok" << endl;
    }

    chain->SetCacheSize(1024 * 1024 * 50); // IT'S BEAUTIFUL MAGIC OF SPEED UP!

    cout << "done." << endl;

    cout << "Calculating entries count... " << endl;
    cout.flush();

    Long64_t entries_count = chain->GetEntries();

    cout << entries_count << " entries." << endl;

    ofstream csv_out(argv[3]);

    cout << "Retrieving data:"  << endl;
    cout.flush();
    time_point_t started_at = chrono::steady_clock::now();
    for (Long64_t entry_number = 0; entry_number < entries_count; ++entry_number) {
        if (entry_number != 0 && entry_number % 500 == 0) {
            print_measurement(started_at, entries_count, entry_number);
        }

        // working with data
        chain->GetEntry(entry_number);

        for (vector<string>::iterator it = branches.begin(); it != branches.end(); it++) {
            csv_out << chain->GetBranch(it->c_str())->GetLeaf(it->c_str())->GetValue() << " ";
        }
        csv_out << endl;
    }

    print_measurement(started_at, entries_count, entries_count);
    csv_out.close();
    cout << "done" << endl;

    return 0;
}
