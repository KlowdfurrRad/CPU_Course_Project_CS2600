#include<bits/stdc++.h>
#define debug 0
using namespace std;


// class CPU{

// CPU(FILE*)

// };

class buffer{
public:
    uint32_t ins;
    bool valid;
    buffer(uint32_t ins = 0,bool valid = false){
        this -> ins = ins;
        this -> valid = valid;
    }
};

class parameters{
    public: 
    uint32_t totalins = 0;
    uint32_t arithmetic_ins = 0;
    uint32_t logical_ins = 0;
    uint32_t shift_ins = 0;
    uint32_t memory_ins = 0;
    uint32_t load_imm_ins = 0;
    uint32_t control_ins = 0;
    uint32_t halt_ins = 0;
    double cycles_per_ins = 0;
    uint32_t total_stalls = 0;
    uint32_t data_stalls = 0;
    uint32_t control_stalls = 0;
    uint32_t cycles = 0;
    parameters(){

    }

    void write(ofstream& ofile){
        ofile<<"Total number of instructions executed        : "<<totalins<<"\n";
        ofile<<"Number of instructions in each class"<<"\n";
        ofile<<"Arithmetic instructions                      : "<<arithmetic_ins<<"\n";
        ofile<<"Logical instructions                         : "<<logical_ins<<"\n";
        ofile<<"Shift instructions                           : "<<shift_ins<<"\n";
        ofile<<"Memory instructions                          : "<<memory_ins<<"\n";
        ofile<<"Load immediate instructions                  : "<<load_imm_ins<<"\n";
        ofile<<"Control instructions                         : "<<control_ins<<"\n";
        ofile<<"Halt instructions                            : "<<halt_ins<<"\n";
        ofile<<"Cycles per instruction                       : "<<cycles_per_ins<<"\n";
        ofile<<"Total number of stalls                       : "<<data_stalls+control_stalls<<"\n";
        ofile<<"Data stalls (RAW)                            : "<<data_stalls<<"\n";
        ofile<<"Control stalls                               : "<<control_stalls<<"\n";
    }

};

bool isRAWhazard(uint32_t nextins,uint32_t RAWhazards[]){
    //the rs1 and rs2 of the nextins should not be same as the rd of buffer[3] and buffer[2] -- yet to occur consider ! ! ! !
    uint32_t rs1 = 100;
    uint32_t rs2 = 100;
    uint32_t opcode = nextins>>12;
    if(opcode <= 2){
        rs1 = (nextins>>4)&(0xF);
        rs2 = nextins&(0xF);
    }else if(opcode == 3){
        rs1 = (nextins>>8)&(0xF);
    }else if(opcode <= 6){
        rs1 = (nextins>>4)&(0xF);
        rs2 = nextins&(0xF);
    }else if(opcode <= 9){
        rs1 = (nextins>>4)&(0xF);
    }else if(opcode == 11){
        rs1 = (nextins>>4)&(0xF);
    }else if(opcode == 12){
        rs1 = (nextins>>4)&(0xF);
        rs2 = (nextins>>8)&(0xF);  //This is rd of ST instruction. It is also source register in this instruction.
    }else if(opcode == 14){
        rs1 = (nextins>>8)&(0xF);
        if(debug){
            cout<<RAWhazards[rs1]<<"b.BEQZ\n";
        }
    }

    if(rs1 != 100){
        if(RAWhazards[rs1] > 0)return true;
    }
    if(rs2 != 100){
        if(RAWhazards[rs2] > 0)return true;
    }

    return false;
}

void setRAWhazards(uint32_t nextins,uint32_t RAWhazards[]){
    uint32_t opcode = nextins>>12;
    if(opcode <= 11){
        uint32_t rd = (nextins>>8)&(0xF);
        RAWhazards[rd]++;
    }
}

char int_to_hex(uint32_t val){
    if(val < 10)return ('0'+val);
    else return ('a'+val-10);
}

int main(){
    vector<uint32_t> icache(256);
    vector<uint32_t> dcache(256);
    vector<uint32_t> rf(16);
    ifstream ifile;
    ifstream ifile1;
    ifstream ifile2;
    ifile.open("input/ICache.txt",ios::in);
    ifile1.open("input/DCache.txt",ios::in);
    ifile2.open("input/RF.txt",ios::in);
    for(int i = 0;i<256;i++){
        uint32_t a;
        uint32_t b;
        ifile>>std::hex>>a;
        ifile1>>std::hex>>b;
        icache[i] = a;
        dcache[i] = b;
    }

    if(debug){
        cout<<icache[0]<<" "<<icache[1]<<"\n";
    }

    for(int i = 0;i<16;i++){
        uint32_t c;
        ifile2>>std::hex>>c;
        rf[i] = c;
    }

    uint32_t PC = 0;
    buffer buffers[5];
    uint32_t ALUOutput;
    uint32_t ALUOutput1;
    uint32_t A;
    uint32_t B;
    uint32_t B1;
    uint32_t LMD;
    bool cond;

    bool ControlHazard = false;
    bool halted = false;

    uint32_t RAWhazards[16] = {0};

    parameters para;

    while(true){
        para.cycles++;
        if(debug){
            cout<<"PC:"<<PC<<"\n";
        }
        if(buffers[4].valid){   //Writeback phase
            uint32_t opcode = buffers[4].ins>>12;

            //instruction increments
            para.totalins++;
            if(opcode <= 3)para.arithmetic_ins++;
            else if(opcode <= 7)para.logical_ins++;
            else if(opcode <= 9)para.shift_ins++;
            else if(opcode <= 10)para.load_imm_ins++;
            else if(opcode <= 12)para.memory_ins++;
            else if(opcode <= 14)para.control_ins++;
            else para.halt_ins++;

            if(debug){
                cout<<"4:"<<(buffers[4].ins>>12)<<"\n";
            }
            if(opcode == 15)break; //HLT instruction
            else if(opcode == 11){ //LD instruction
                uint32_t rd = (buffers[4].ins>>8)&(0xF);
                rf[rd] = LMD;
            }else if(opcode == 10){ //LI instruction
                uint32_t imm = (buffers[4].ins)&(0xFF);
                uint32_t rd = (buffers[4].ins>>8)&(0xF);
                //cout<<"LI:"<<rd<<" "<<imm<<"\n";
                rf[rd] = imm;
            }else if(opcode <= 9){ //ALU instructions
                uint32_t rd = (buffers[4].ins>>8)&(0xF);
                rf[rd] = ALUOutput1;
                //if(opcode == 1)cout<<"SUB:"<<ALUOutput1<<"\n";
            }
        }
        if(buffers[3].valid){   //Memory Access phase
            uint32_t opcode = (buffers[3].ins)>>12;
            if(debug){
                cout<<"3:"<<(buffers[3].ins>>12)<<"\n";
            }
            if(opcode == 11){   //LD instruction
                LMD = dcache[ALUOutput];
            }
            else if(opcode == 12){  //ST instruction
                dcache[ALUOutput] = B1;
            }
            if(opcode == 13 || opcode == 14){
                ControlHazard = false;
            }
            //We decrement required RAWhazard in this cycle
            //The we can allow the instructions that read such register to be fetched in this same clock cycle
            //In the next clock cycle the register read for those instructions happens after the write back for the Write instruction happens
            if(opcode <= 11){
                uint32_t rd = ((buffers[3].ins)>>8)&(0xF);
                RAWhazards[rd]--;
            }

            ALUOutput1 = ALUOutput;
        }
        if(buffers[2].valid){   //Execute cycle
            uint32_t opcode = (buffers[2].ins)>>12;
            if(debug){
                cout<<"2:"<<(buffers[2].ins>>12)<<"\n";
            }
            if(opcode == 0){
                ALUOutput = (A+B)%256;
            }else if(opcode == 1){
                ALUOutput = (A-B+256)%256;
            }else if(opcode == 2){
                ALUOutput = (A*B)%256;
            }else if(opcode == 3){
                ALUOutput = (A+1)%256;
            }else if(opcode == 4){
                ALUOutput = A&B;
            }else if(opcode == 5){
                ALUOutput = A|B;
            }else if(opcode == 6){
                ALUOutput = A^B;
            }else if(opcode == 7){
                ALUOutput = ~A;
            }else if(opcode == 8){  //SRLI
                uint32_t imm = (buffers[2].ins)&(0xF);
                ALUOutput = (A<<imm)%256;
            }else if(opcode == 9){  //SLLI
                uint32_t imm = (buffers[2].ins)&(0xF);
                ALUOutput = A>>imm;
            }else if(opcode == 11 || opcode == 12){ 
                uint32_t imm = (buffers[2].ins)&(0xF);
                if(imm >= 8){ //negative so just make it sign extension
                    imm = imm + 0xF0;
                }
                ALUOutput = (A + imm)%256;
            }else if(opcode == 13){ //JMP
                uint32_t imm = ((buffers[2].ins)>>4)&(0xFF);
                imm = (imm*2+256)%256;
                ALUOutput = (PC + imm)%256;
                PC = ALUOutput;
            }else if(opcode == 14){ //BEQZ
                if(A == 0){
                    uint32_t imm = (buffers[2].ins)&(0xFF);
                    imm = (imm*2+256)%256;
                    ALUOutput = (PC + imm + 256)%256;
                    PC = ALUOutput;
                }else {
                    //PC = (PC + 2)%256; //NOTE MAN X , Remove comment if that done
                }
            }
            B1 = B;
        } 
        if(buffers[1].valid){ 
            uint32_t opcode = (buffers[1].ins)>>12;
            if(debug){
                cout<<"1:"<<(buffers[1].ins>>12)<<"\n";
            }
            if(opcode <= 2){    //ARITHMETIC ADD,SUB,MUL
                uint32_t rs1 = ((buffers[1].ins)>>4)&(0xF);
                uint32_t rs2 = (buffers[1].ins)&(0xF);
                A = rf[rs1];
                B = rf[rs2];
                //if(opcode == 1)cout<<"sub in read"<<A<<" "<<B<<"\n";
            }else if(opcode == 3){  //ARITHMETIC INC
                uint32_t r = ((buffers[1].ins)>>8)&(0xF); 
                A = rf[r];
            }else if(opcode <= 6){  //LOGICAL AND,OR,XOR
                uint32_t rs1 = ((buffers[1].ins)>>4)&(0xF);
                uint32_t rs2 = (buffers[1].ins)&(0xF);
                A = rf[rs1];
                B = rf[rs2];
            }else if(opcode <= 9){  //NOT , SLLI , SRLI intructions
                uint32_t rs1 = ((buffers[1].ins)>>4)&(0xF);
                A = rf[rs1];
            }else if(opcode == 11 || opcode == 12){ //ST,LD instructions
                uint32_t rs1 = ((buffers[1].ins)>>4)&(0xF);
                A = rf[rs1];
                if(opcode == 12){   //If ST instruction need to put rd value in B to be stored into memory in MEM phase
                    uint32_t rd = (buffers[1].ins>>8)&(0xF);
                    B = rf[rd];
                }
            }else if(opcode == 14){ //BEQZ instruction
                uint32_t rs = ((buffers[1].ins)>>8)&(0xF);
                A = rf[rs];
                //cout<<"A:"<<A<<"rs:"<<rs<<"\n";
            }
        }

        buffers[4] = buffers[3];
        buffers[3] = buffers[2];
        buffers[2] = buffers[1];

        if(!halted){   //Fetch cycle
            if(!ControlHazard){
                buffers[1] = buffer(icache[PC+1]+256*icache[PC],true);
                uint32_t opcode = (buffers[1].ins)>>12;
                if(opcode == 15){
                    halted = true; //means that we won't fetch anymore
                }
                else if(opcode == 13){
                    ControlHazard = true; //set this to false once the branch instruction is executed
                    PC = (PC + 2)%256; //NOTE
                }else if(opcode == 14){
                    if(!isRAWhazard(buffers[1].ins,RAWhazards)){
                        ControlHazard = true;
                        PC = (PC + 2)%256; //NOTE MAN X
                        //And we dont change the PC value as well
                    }else{
                        para.data_stalls++;
                        buffers[1] = buffer(0,false);
                    }
                }else{
                    if(!isRAWhazard(buffers[1].ins,RAWhazards)){
                        //No raw hazards to be fearing
                        PC = (PC + 2)%256;
                        setRAWhazards(buffers[1].ins,RAWhazards);
                        //if(opcode == 1)cout<<RAWhazards[1]<<"rawhaz_sub\n";
                    }else{
                        para.data_stalls++;
                        buffers[1] = buffer(0,false);
                    }
                }
                
            }else{              //NEED TO DO RAW thing also
                para.control_stalls++;
                buffers[1] = buffer(0,false); //invalid 
            }
        }else buffers[1] = buffer(0,false);
    }
    para.cycles_per_ins = (double)para.cycles/(double)para.totalins;

    ofstream ofile;
    ofstream ofile1;
    ofstream ofile2;
    //ofile.open("Output/ins.txt",ios::out);
    ofile1.open("output/DCache.txt",ios::out);
    //ofile2.open("Output/rf.txt",ios::out);

    ofstream ofile3;
    ofile3.open("output/Output.txt",ios::out);

    for(int i = 0;i<256;i++){
        //ofile<<int_to_hex(icache[i]/16)<<int_to_hex(icache[i]%16)<<"\n";
    }
    for(int i = 0;i<256;i++){
        ofile1<<int_to_hex(dcache[i]/16)<<int_to_hex(dcache[i]%16)<<"\n";
    }
    for(int i = 0;i<16;i++){
        //ofile2<<int_to_hex(rf[i]/16)<<int_to_hex(rf[i]%16)<<"\n";
    }

    para.write(ofile3); //Writes to Output.txt , the parameters of the executions 

}