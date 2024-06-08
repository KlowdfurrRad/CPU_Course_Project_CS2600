#include<bits/stdc++.h>
using namespace std;

uint16_t read_reg(int i){
    ifstream ifile;
    ifile.open("Input/rf.txt",ios::binary);
    ifile.seekg(4*i,ios::beg);
    uint16_t val;
    ifile>>std::hex>>val;
    return val;
}

uint16_t read_cache(int i){ //reads 2 bytes of data
    ifstream ifile;
    ifile.open("Input/cache.txt",ios::binary);
    ifile.seekg(4*i,ios::beg);
    uint16_t val;
    ifile>>std::hex>>val;
    return val;
}

uint16_t read_ins(int i){ //reads 2 bytes of data
    ifstream ifile;
    ifile.open("Input/ins.txt",ios::binary);
    ifile.seekg(4*i,ios::beg);
    uint16_t ins1,ins2;
    ifile>>std::hex>>ins1>>ins2;
    return (ins1<<8) + ins2;
}

char int_to_hex(uint16_t val){
    if(val < 10)return ('0'+val);
    else return ('a'+val-10);
}

void write_reg(int i,uint16_t val){
    fstream ofile;
    ofile.open("Input/rf.txt",ios::in|ios::out|ios::binary);
    ofile.seekp(4*i,ios::beg);
    char A,B; //NOTE that should do as 0x04 would print only 4 lel . Anyways ya
    A = int_to_hex(val%16);
    B = int_to_hex(val>>4);
    ofile.put(A);
    ofile.put(B);
}

void write_cache(int i,uint16_t val){
    fstream ofile;
    ofile.open("Input/cache.txt",ios::in|ios::out|ios::binary);
    ofile.seekp(4*i,ios::beg);
    char A,B; //NOTE that should do as 0x04 would print only 4 lel . Anyways ya
    A = int_to_hex(val%16);
    B = int_to_hex(val>>4);
    ofile.put(A);
    ofile.put(B);
}