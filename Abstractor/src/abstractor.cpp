/**
 * @file abstractor.cpp
 * @author Umut Deniz Sener
 
 * @brief Code for Abstractor that uses multithreaded system.
 *
 * The Abstractor is a multithread-powered research tool for scientific literature. It focuses on
 * delivering fast and accurate results for given queries. The program takes a query as an input,
 * scans its file collection, finds the most relevant abstracts by measuring the similarities between
 * abstracts and the given query, then retrieves convenient sentences from the selected abstracts.
 *
 * How to compile and run:
    make
    ./abstractor.out
 */
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <set>
#include <vector>
#include <queue>
#include <map>
#include <cstdlib>
#include <pthread.h>
#include <iostream>
#include <iomanip>
#include <algorithm>

using namespace std;
set<string> queries; //initialize queries set as global variable
queue<string> abs_files; //initialize abstract files queue as global variable
multimap<string, vector<string>> result_map; //initiliaze result map by using multimap in order to allow duplicate keys as global variable
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER; //initilization of mutex
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER; //initilization of mutex
pthread_mutex_t mutex3 = PTHREAD_MUTEX_INITIALIZER; //initilization of mutex
pthread_mutex_t mutex4 = PTHREAD_MUTEX_INITIALIZER; //initilization of mutex

ofstream outfile;

void *runner(void *param);

//function that tokenize queries 
void simple_tokenizer(string s)
{
    stringstream ss(s);
    string word;
    while (ss >> word) {
       queries.insert(word);
    }
}
int main(int argc, char *argv[])
{
    //for taking the first line of input file
    ifstream infile(argv[1]);
    outfile.open(argv[2]);
    string snof_threads, sabs_scan, sabs_return;
    infile>>snof_threads;
    infile>>sabs_scan;
    infile>>sabs_return;
    int nof_threads = stoi(snof_threads);
    int abs_scan = stoi(sabs_scan);
    int abs_return = stoi(sabs_return);

    if(nof_threads>abs_scan){
        nof_threads = abs_scan;
    }

    string line;
    vector<string> th_name_vec;
    getline(infile, line);
    getline(infile, line);
    simple_tokenizer(line);
    vector<pthread_t> thread_vec;

    //for taking the names of abstract files
    for(int i = 0 ; i<abs_scan ; i++){
        getline(infile, line);
        abs_files.push(line);
    }
    //by naming the threads
    for(int i =0; i<nof_threads; i++){
     int asciiValue = 65;
     char character = char(asciiValue+i);
     string n(1, character);
     string th_name = "Thread "+n;
     th_name_vec.push_back(th_name);
    
 }
    for(int i = 0; i<nof_threads; i++){

        //creation of threads
        pthread_t t_id;
      
        if(pthread_create(&t_id, NULL, &runner, &th_name_vec[i])!=0){
            fprintf(stderr, "An error occured while creating the thread");
            exit(-1);
        }
        thread_vec.push_back(t_id);
    }

    //main function waiting threads 
   for(auto k : thread_vec){
        pthread_join(k, NULL);
    }
    vector<float> vector_f;
    
    //after threads calculates the values to write them on the output file
    for(auto m : result_map){
        float ratio = stof(m.second[0]);
        vector_f.push_back(ratio);
    }

    sort(vector_f.rbegin(), vector_f.rend());
    outfile<<"###\n";
    for(int z = 0 ; z<abs_return ; z++){
        for(auto m : result_map){
            if(vector_f[z] == stof(m.second[0])){
                outfile<<"Result "<<z+1<<":\n"<<"File: "<<m.first<<"\nScore: "<<m.second[0]<<"\nSummary:";
                if(m.second[1].at(0) != ' '){
                    outfile<<" ";
                }
                outfile<<m.second[1]<<"\n###\n";
                break;
            } 
        }
    }
    outfile.close();

   pthread_exit(NULL);
}

void *runner(void *param){
    string txt_file;
    string thread_id = *(string *)param;
    string abstract_path;


// threads calculates abstract file until it finishes
    while(!abs_files.empty()){

        pthread_mutex_lock(&mutex1); //using mutex lock for critical section
        if(!abs_files.empty()){
        txt_file = abs_files.front();
        outfile<<thread_id<<" is calculating "<<txt_file<<endl;
        abstract_path = "../abstracts/" + txt_file;
        abs_files.pop();}

    
         ifstream infile(abstract_path);
        pthread_mutex_unlock(&mutex1);
        


    string content( (istreambuf_iterator<char>(infile) ),(istreambuf_iterator<char>() ));

vector<string> results;    
set<string> words;   
string segment;
vector<string> seglist;
stringstream str_strm(content);

//to seperate sentences
while(getline(str_strm, segment, '.'))
{
   seglist.push_back(segment);
}
for(auto x: seglist){
    bool contains = true;
    string segm;
    stringstream str_str(x);
    while(getline(str_str, segm, ' ')){     //to tokenize sentences
    segm.erase(remove(segm.begin(), segm.end(), '\n'), segm.end());
    words.insert(segm);
    pthread_mutex_lock(&mutex3);
    for (auto check : queries){
        if(segm == check && contains){
            x.erase(remove(x.begin(), x.end(), '\n'), x.end());
            results.push_back(x);
            contains = false;
        }
    }
    pthread_mutex_unlock(&mutex3);
}
}

int intersection = 0;
int uni = 0;
 pthread_mutex_lock(&mutex3); //using mutex lock for critical section
for(auto element : words){
    for(auto q: queries){
        if(element==q){
            intersection++;
        }
    }
}

uni = words.size()+queries.size()-intersection;
  pthread_mutex_unlock(&mutex3);
float ans = (float)intersection / (float)uni;
string str_res = "";
for(auto m : results){
   str_res = str_res + m + ".";
}

stringstream stream;
stream << fixed << setprecision(4) << ans;
string s_ans = stream.str();
vector<string> res_vec;
res_vec.push_back(s_ans);
res_vec.push_back(str_res);
pthread_mutex_lock(&mutex4); //using mutex lock for critical section
result_map.insert(pair<string, vector<string>>(txt_file, res_vec));
pthread_mutex_unlock(&mutex4);
}
pthread_exit(NULL);
}


