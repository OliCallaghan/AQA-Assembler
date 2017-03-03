//
//  main.cpp
//  AQA Assembler
//
//  Created by Oli Callaghan on 02/03/2017.
//  Copyright © 2017 Oli Callaghan. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <deque>
#include <list>
#include <regex>

struct label {
    std::string name;
    unsigned long ln;
};

// Type: 0 - Register
//       1 - Immediate
//       2 - Direct
struct params {
    std::deque<long int> data;
    std::deque<int> pos;
    std::deque<char> type;
};

inline bool isInteger(const std::string &s) {
    if(s.empty() || ((!isdigit(s[0])) && (s[0] != '-') && (s[0] != '+'))) return false;
    
    char* p;
    strtol(s.c_str(), &p, 10);
    
    return (*p == 0);
}

params extractRegisterParameters(std::string line) {
    params p;
    
    try {
        std::regex expr("R([0-9]+)");
        std::sregex_iterator next(line.begin(), line.end(), expr);
        std::sregex_iterator end;
        
        int size = 0;
        
        while (next != end) {
            std::smatch match = *next;
            size++;
            p.data.push_back(atoi(match.str().substr(1,match.str().length() -1).c_str()));
            p.pos.push_back(static_cast<int>(match.position()));
            p.type.push_back(0);
            
            std::cout << "Register: " << p.data[size - 1] << "\n";
            next++;
        }
    } catch (std::regex_error& e) {
        // Syntax Error
    }
    
    return p;
}

params extractImmediateParameters(std::string line) {
    params p;
    
    try {
        std::regex expr("#([0-9]+)");
        std::sregex_iterator next(line.begin(), line.end(), expr);
        std::sregex_iterator end;
        
        int size = 0;
        
        while (next != end) {
            std::smatch match = *next;
            size++;
            p.data.push_back(atoi(match.str().substr(1,match.str().length() -1).c_str()));
            p.pos.push_back(static_cast<int>(match.position()));
            p.type.push_back(1);
            
            std::cout << "Immediate: " << p.data[size - 1] << "\n";
            next++;
        }
    } catch (std::regex_error& e) {
        // Syntax Error
    }
    
    return p;
}

params extractDirectParameters(std::string line) {
    params p;
    
    try {
        std::regex expr("[,\\s]([0-9]+)");
        std::sregex_iterator next(line.begin(), line.end(), expr);
        std::sregex_iterator end;
        
        int size = 0;
        
        while (next != end) {
            std::smatch match = *next;
            size++;
            p.data.push_back(atoi(match.str().substr(1,match.str().length() -1).c_str()));
            p.pos.push_back(static_cast<int>(match.position()));
            p.type.push_back(2);
            
            std::cout << "Direct: " << p.data[size - 1] << "\n";
            next++;
        }
    } catch (std::regex_error& e) {
        // Syntax Error
    }
    
    return p;
}

params combineParamsInOrder(params p_r, params p_i, params p_d) {
    params p;
    while ((p_r.data.size() > 0) || (p_i.data.size() > 0) || (p_d.data.size() > 0)) {
        char p_smallest = 0;
        long int p_smallest_data = 0;
        
        if (p_r.pos.size() > 0) {
            p_smallest_data = p_r.pos[0];
        } else if (p_i.pos.size() > 0) {
            p_smallest_data = p_i.pos[0];
            p_smallest = 1;
        } else {
            p_smallest_data = p_d.pos[0];
            p_smallest = 2;
        }
        
        if (p_i.pos.size() > 0) {
            if (p_i.pos[0] < p_smallest_data) {
                p_smallest_data = p_i.pos[0];
                p_smallest = 1;
            }
        }
        
        if (p_d.pos.size() > 0) {
            if (p_d.pos[0] < p_smallest_data) {
                p_smallest_data = p_d.pos[0];
                p_smallest = 2;
            }
        }
        
        if (p_smallest == 0) {
            p.data.push_back(p_r.data[0]);
            p.pos.push_back(p_r.pos[0]);
            p.type.push_back(p_r.type[0]);
            p_r.pos.pop_front();
            p_r.data.pop_front();
            p_r.type.pop_front();
        } else if (p_smallest == 1) {
            p.data.push_back(p_i.data[0]);
            p.pos.push_back(p_i.pos[0]);
            p.type.push_back(p_i.type[0]);
            p_i.pos.pop_front();
            p_i.data.pop_front();
            p_i.type.pop_front();
        } else {
            p.data.push_back(p_d.data[0]);
            p.pos.push_back(p_d.pos[0]);
            p.type.push_back(p_d.type[0]);
            p_d.pos.pop_front();
            p_d.data.pop_front();
            p_d.type.pop_front();
        }
    }
    return p;
}

class aqa_assembler_vm {
    // Initialise Memory Registers
    unsigned long int r[12] = {0,0,0,0,0,0,0,0,0,0,0,0};
    
    // Initialise Virtual Memory Block
    unsigned long int *memory;
    
    // Initialise Label Storage
    std::vector<label> labels;
    
    // Status Register
    //  0 = Equal
    //  1 = Greater Than
    //  2 = Less Than
    //  3 = Not Equal
    //  4 = Initialised (MUST COMPARE VALUE)
    char sr = 4;
public:
    // Constructor
    aqa_assembler_vm(int memory_size, std::vector<label> labels_data) {
        // Initilaise Memory of Specified Size
        memory = (unsigned long int*) calloc(memory_size, sizeof(unsigned long int));
        labels = labels_data;
    }
    
    // Destructor
    ~aqa_assembler_vm() {
        // Remove Memory
        free(memory);
    }
    
    // Load the value stored at memory location 'm' into register 'r'
    int LDR(unsigned int r, unsigned long int m) {
        this->r[r] = memory[m];
        return 0;
    }
    
    // Store the value stored in register 'r' at memory location 'm'
    int STR(unsigned int r, unsigned long m) {
        memory[m] = this->r[r];
        return 0;
    }
    
    // Add the data stored in register 'r_i0' to the contents of register 'r_i1' and store in 'r_o'
    int ADD(unsigned int r_o, unsigned int r_i0, unsigned long int r_i1, bool direc_addr) {
        if (direc_addr) {
            r[r_o] = r[r_i0] + r[r_i1];
        } else {
            r[r_o] = r[r_i0] + r_i1;
        }
        return 0;
    }
    
    // Subtract the data stored in register 'r_i0' to the contents of register 'r_i1' and store in 'r_o'
    int SUB(unsigned int r_o, unsigned int r_i0, unsigned int r_i1, bool direc_addr) {
        if (direc_addr) {
            r[r_o] = r[r_i0] - r[r_i1];
        } else {
            r[r_o] = r[r_i0] - r_i1;
        }
        return 0;
    }
    
    // Move the data 'd' into register 'r'
    int MOV(unsigned int r_o, unsigned long int r_0, bool direct_addr) {
        if (direct_addr) {
            r[r_o] = r[r_0];
        } else {
            r[r_o] = r_0;
        }
        
        return 0;
    }
    
    // Compare the data stored in register 'r_o' to the data stored in 'r_1' or 'r_1' (dependent on addressing mode)
    int CMP(unsigned int r_0, unsigned long int r_1, bool direct_addr) {
        if (direct_addr) {
            // Direct Addressing Mode
            if (r[r_0] == r[r_1]) {
                // Equal
                sr = 0;
            } else if (r[r_0] > r[r_1]) {
                // Greater Than
                sr = 1;
            } else if (r[r_0] > r[r_1]) {
                // Less Than
                sr = 2;
            } else {
                // Not Equal
                sr = 3;
            }
        } else {
            // Immediate Addressing Mode
            if (r[r_0] == r_1) {
                // Equal
                sr = 0;
            } else if (r[r_0] > r_1) {
                // Greater Than
                sr = 1;
            } else if (r[r_0] > r_1) {
                // Less Than
                sr = 2;
            } else {
                // Not Equal
                sr = 3;
            }

        }
        return 1;
    }
    
    // Branch to instruction at label
    int B(std::string label, unsigned long *pc) {
        // Branch
        for (int l = 0; l < labels.size(); l++) {
            if (labels[l].name == label) {
                *pc = labels[l].ln;
            }
        }
        
        return 0;
    }
    
    // Branch to instruction at label if status register is 0 (equal)
    int BEQ(std::string label, unsigned long *pc) {
        if (sr == 0) {
            // Branch
            this->B(label, pc);
        }
        return 0;
    }
    
    // Branch to instruction at label if status register is 0 (equal)
    int BGT(std::string label, unsigned long *pc) {
        if (sr == 1) {
            // Branch
            this->B(label, pc);
        }
        return 0;
    }
    
    // Branch to instruction at label if status register is 0 (equal)
    int BLT(std::string label, unsigned long *pc) {
        if (sr == 2) {
            // Branch
            this->B(label, pc);
        }
        return 0;
    }
    
    // Branch to instruction at label if status register is 0 (equal)
    int BNE(std::string label, unsigned long *pc) {
        if (sr == 3) {
            // Branch
            this->B(label, pc);
        }
        return 0;
    }
    
    // Perform bitwise AND on the data stored in register 'r_i0' with the contents of register 'r_i1' (or the data 'r_i1' depending on addressing mode) and store in 'r_o'
    int AND(unsigned int r_o, unsigned int r_i0, unsigned long int r_i1, bool direct_addr) {
        if (direct_addr) {
            r[r_o] = r[r_i0] & r[r_i1];
        } else {
            r[r_o] = r[r_i0] & r_i1;
        }
        return 0;
    }
    
    // Perform bitwise OR on the data stored in register 'r_i0' with the contents of register 'r_i1' (or the data 'r_i1' depending on addressing mode) and store in 'r_o'
    int ORR(unsigned int r_o, unsigned int r_i0, unsigned long int r_i1, bool direct_addr) {
        if (direct_addr) {
            r[r_o] = r[r_i0] | r[r_i1];
        } else {
            r[r_o] = r[r_i0] | r_i1;
        }
        return 0;
    }
    
    // Perform bitwise XOR on the data stored in register 'r_i0' with the contents of register 'r_i1' (or the data 'r_i1' depending on addressing mode) and store in 'r_o'
    int EOR(unsigned int r_o, unsigned int r_i0, unsigned long int r_i1, bool direct_addr) {
        if (direct_addr) {
            r[r_o] = r[r_i0] ^ r[r_i1];
        } else {
            r[r_o] = r[r_i0] ^ r_i1;
        }
        return 0;
    }
    
    // Perform bitwise NOT on the data stored in register 'r_i' (or the data 'r_i' depending on addressing mode) and store in 'r_o'
    int MVN(unsigned int r_o, unsigned int r_i, bool direct_addr) {
        if (direct_addr) {
            r[r_o] = ~r[r_i];
        } else {
            r[r_o] = ~r[r_i];
        }
        return 0;
    }
    
    // Logically shift left the value stored in register 'r_i0' by the number of bits specified by the contents of register 'r_i1' (or the data 'r_i' depending on addressing mode) and store in 'r_o'
    int LSL(unsigned int r_o, unsigned int r_i0, unsigned long int r_i1, bool direct_addr) {
        if (direct_addr) {
            r[r_o] = r[r_i0];
            for (int n_shift = 0; n_shift < r_i1; n_shift += 1) {
                r[r_o] = r[r_o] << 1;
            }
        } else {
            r[r_o] = r_i0;
            for (int n_shift = 0; n_shift < r_i1; n_shift += 1) {
                r[r_o] = r[r_o] << 1;
            }
        }
        return 0;
    }
    
    // Logically shift left the value stored in register 'r_i0' by the number of bits specified by the contents of register 'r_i1' (or the data 'r_i' depending on addressing mode) and store in 'r_o'
    int LSR(unsigned int r_o, unsigned int r_i0, unsigned long int r_i1, bool direct_addr) {
        if (direct_addr) {
            r[r_o] = r[r_i0];
            for (int n_shift = 0; n_shift < r_i1; n_shift += 1) {
                r[r_o] = r[r_o] >> 1;
            }
        } else {
            r[r_o] = r_i0;
            for (int n_shift = 0; n_shift < r_i1; n_shift += 1) {
                r[r_o] = r[r_o] >> 1;
            }
        }
        return 0;
    }
    
    void displayRegisters() {
        for (int r_n = 0; r_n < 12; r_n++) {
            std::cout << "R" << r_n << ": " << r[r_n] << "\n";
        }
    }
};

int run(std::string entrypoint) {
    // Initialise file reader
    std::ifstream input(entrypoint);
    
    // Initialise program
    std::vector<std::string> program;
    
    for (std::string line; getline( input, line );) {
        // Increase program length and add line to program
        program.resize(program.size() + 1);
        program[program.size() - 1] = line;
    }
    
    input.close();
    
    // Initialise list of labels
    std::vector<label> labels;
    
    for (unsigned long pc = 0; pc < program.size(); pc++) {
        if (program[pc].back() == ':') {
            // Add label to list
            label l;
            l.name = program[pc].substr(0,program[pc].length() - 1);
            l.ln = pc;
            labels.push_back(l);
        }
    }
    
    // Initialise assembler emulator
    aqa_assembler_vm assembler(1024, labels);
    
    for (unsigned long pc = 0; pc < program.size(); pc++) {
        std::string cmd = program[pc].substr(0,4);
        // Lookup command
        if (cmd == "LDR ") {
            // LDR Command
            std::string params_s = program[pc].substr(4,program[pc].length() - 1);
            params p_r = extractRegisterParameters(params_s);
            params p_d = extractDirectParameters(params_s);
            params p_i = extractImmediateParameters(params_s);
            
            params p = combineParamsInOrder(p_r, p_i, p_d);
            
            std::cout << p.type[0] << p.type[1];
            
            if (p.type.size() == 2) {
                if ((p.type[0] == 0) && (p.type[1] == 2)) {
                    std::cout << "LDR R" << p.data[0] << " " << p.data[1] << "\n";
                    assembler.LDR(static_cast<unsigned int>(p.data[0]), p.data[1]);
                } else {
                    std::cout << "Expected register number and direct memory address\n";
                }
            } else {
                std::cout << "Not enough arguements\n";
            }
        } else if (cmd == "STR ") {
            // STR Command
            std::string params_s = program[pc].substr(4,program[pc].length() - 1);
            params p_r = extractRegisterParameters(params_s);
            params p_d = extractDirectParameters(params_s);
            params p_i = extractImmediateParameters(params_s);
            
            params p = combineParamsInOrder(p_r, p_i, p_d);
            
            std::cout << p.type[0] << p.type[1];
            
            if (p.type.size() == 2) {
                if ((p.type[0] == 0) && (p.type[1] == 2)) {
                    std::cout << "STR R" << p.data[0] << " " << p.data[1] << "\n";
                    assembler.STR(static_cast<unsigned int>(p.data[0]), p.data[1]);
                } else {
                    std::cout << "Expected register number and direct memory address\n";
                }
            } else {
                std::cout << "Not enough arguements\n";
            }
        } else if (cmd == "ADD ") {
            // ADD Command
            std::string params_s = program[pc].substr(4,program[pc].length() - 1);
            params p_r = extractRegisterParameters(params_s);
            params p_d = extractDirectParameters(params_s);
            params p_i = extractImmediateParameters(params_s);
            
            params p = combineParamsInOrder(p_r, p_i, p_d);
            
            std::cout << p.type[0] << p.type[1];
            
            if (p.type.size() == 3) {
                if ((p.type[0] == 0) && (p.type[1] == 0) && (p.type[2] == 0)) {
                    std::cout << "ADD R" << p.data[0] << " R" << p.data[1] << " R" << p.data[2] << "\n";
                    assembler.ADD(static_cast<unsigned int>(p.data[0]), static_cast<unsigned int>(p.data[1]), static_cast<unsigned int>(p.data[2]), true);
                } else if ((p.type[0] == 0) && (p.type[1] == 0) && (p.type[2] == 1)) {
                    std::cout << "ADD R" << p.data[0] << " R" << p.data[1] << " #" << p.data[2] << "\n";
                    assembler.ADD(static_cast<unsigned int>(p.data[0]), static_cast<unsigned int>(p.data[1]), static_cast<unsigned int>(p.data[2]), false);
                } else {
                    std::cout << "Expected desination register number, register number and operand\n";
                }
            } else {
                std::cout << "Not enough arguements\n";
            }
        } else if (cmd == "SUB ") {
            // SUB Command
            std::string params_s = program[pc].substr(4,program[pc].length() - 1);
            params p_r = extractRegisterParameters(params_s);
            params p_d = extractDirectParameters(params_s);
            params p_i = extractImmediateParameters(params_s);
            
            params p = combineParamsInOrder(p_r, p_i, p_d);
            
            std::cout << p.type[0] << p.type[1];
            
            if (p.type.size() == 3) {
                if ((p.type[0] == 0) && (p.type[1] == 0) && (p.type[2] == 0)) {
                    std::cout << "SU  R" << p.data[0] << " R" << p.data[1] << " R" << p.data[2] << "\n";
                    assembler.SUB(static_cast<unsigned int>(p.data[0]), static_cast<unsigned int>(p.data[1]), static_cast<unsigned int>(p.data[2]), true);
                } else if ((p.type[0] == 0) && (p.type[1] == 0) && (p.type[2] == 1)) {
                    std::cout << "SUB R" << p.data[0] << " R" << p.data[1] << " #" << p.data[2] << "\n";
                    assembler.SUB(static_cast<unsigned int>(p.data[0]), static_cast<unsigned int>(p.data[1]), static_cast<unsigned int>(p.data[2]), false);
                } else {
                    std::cout << "Expected desination register number, register number and operand\n";
                }
            } else {
                std::cout << "Not enough arguements\n";
            }
        } else if (cmd == "MOV ") {
            // MOV Command
            std::string params_s = program[pc].substr(4,program[pc].length() - 1);
            params p_r = extractRegisterParameters(params_s);
            params p_d = extractDirectParameters(params_s);
            params p_i = extractImmediateParameters(params_s);
            
            params p = combineParamsInOrder(p_r, p_i, p_d);
            
            std::cout << p.type[0] << p.type[1];
            
            if (p.type.size() == 2) {
                if ((p.type[0] == 0) && (p.type[1] == 0)) {
                    std::cout << "ADD R" << p.data[0] << " R" << p.data[1] << "\n";
                    assembler.MOV(static_cast<unsigned int>(p.data[0]), static_cast<unsigned int>(p.data[1]), true);
                } else if ((p.type[0] == 0) && (p.type[1] == 1)) {
                    std::cout << "ADD R" << p.data[0] << " #" << p.data[1] << "\n";
                    assembler.MOV(static_cast<unsigned int>(p.data[0]), static_cast<unsigned int>(p.data[1]), false);
                } else {
                    std::cout << "Expected desination register number and operand\n";
                }
            } else {
                std::cout << "Not enough arguements\n";
            }
        } else if (cmd == "CMP ") {
            // CMP Command
            std::string params_s = program[pc].substr(4,program[pc].length() - 1);
            params p_r = extractRegisterParameters(params_s);
            params p_d = extractDirectParameters(params_s);
            params p_i = extractImmediateParameters(params_s);
            
            params p = combineParamsInOrder(p_r, p_i, p_d);
            
            std::cout << p.type[0] << p.type[1];
            
            if (p.type.size() == 2) {
                if ((p.type[0] == 0) && (p.type[1] == 0)) {
                    std::cout << "CMP R" << p.data[0] << " R" << p.data[1] << "\n";
                    assembler.CMP(static_cast<unsigned int>(p.data[0]), static_cast<unsigned int>(p.data[1]), true);
                } else if ((p.type[0] == 0) && (p.type[1] == 1)) {
                    std::cout << "CMP R" << p.data[0] << " #" << p.data[1] << "\n";
                    assembler.CMP(static_cast<unsigned int>(p.data[0]), static_cast<unsigned int>(p.data[1]), false);
                } else {
                    std::cout << "Expected desination register number and operand\n";
                }
            } else {
                std::cout << "Not enough arguements\n";
            }
        } else if (program[pc].substr(0,2) == "B ") {
            // Branch
            assembler.B(program[pc].substr(2,program[pc].length()), &pc);
        } else if (cmd == "BEQ ") {
            // BEQ Command
            assembler.BEQ(program[pc].substr(4,program[pc].length()), &pc);
        } else if (cmd == "BNE ") {
            // BNE Command
            assembler.BNE(program[pc].substr(4,program[pc].length()), &pc);
        } else if (cmd == "BGT ") {
            // BGT Command
            assembler.BGT(program[pc].substr(4,program[pc].length()), &pc);
        } else if (cmd == "BLT ") {
            // BLT Command
            assembler.BLT(program[pc].substr(4,program[pc].length()), &pc);
        } else if (cmd == "AND ") {
            // AND Command
            std::string params_s = program[pc].substr(4,program[pc].length() - 1);
            params p_r = extractRegisterParameters(params_s);
            params p_d = extractDirectParameters(params_s);
            params p_i = extractImmediateParameters(params_s);
            
            params p = combineParamsInOrder(p_r, p_i, p_d);
            
            std::cout << p.type[0] << p.type[1];
            
            if (p.type.size() == 3) {
                if ((p.type[0] == 0) && (p.type[1] == 0) && (p.type[2] == 0)) {
                    std::cout << "AND R" << p.data[0] << " R" << p.data[1] << " R" << p.data[2] << "\n";
                    assembler.AND(static_cast<unsigned int>(p.data[0]), static_cast<unsigned int>(p.data[1]), static_cast<unsigned int>(p.data[2]), true);
                } else if ((p.type[0] == 0) && (p.type[1] == 0) && (p.type[2] == 1)) {
                    std::cout << "AND R" << p.data[0] << " R" << p.data[1] << " #" << p.data[2] << "\n";
                    assembler.AND(static_cast<unsigned int>(p.data[0]), static_cast<unsigned int>(p.data[1]), static_cast<unsigned int>(p.data[2]), false);
                } else {
                    std::cout << "Expected desination register number, register number and operand\n";
                }
            } else {
                std::cout << "Not enough arguements\n";
            }
        } else if (cmd == "ORR ") {
            // ORR Command
            std::string params_s = program[pc].substr(4,program[pc].length() - 1);
            params p_r = extractRegisterParameters(params_s);
            params p_d = extractDirectParameters(params_s);
            params p_i = extractImmediateParameters(params_s);
            
            params p = combineParamsInOrder(p_r, p_i, p_d);
            
            std::cout << p.type[0] << p.type[1];
            
            if (p.type.size() == 3) {
                if ((p.type[0] == 0) && (p.type[1] == 0) && (p.type[2] == 0)) {
                    std::cout << "ORR R" << p.data[0] << " R" << p.data[1] << " R" << p.data[2] << "\n";
                    assembler.ORR(static_cast<unsigned int>(p.data[0]), static_cast<unsigned int>(p.data[1]), static_cast<unsigned int>(p.data[2]), true);
                } else if ((p.type[0] == 0) && (p.type[1] == 0) && (p.type[2] == 1)) {
                    std::cout << "ORR R" << p.data[0] << " R" << p.data[1] << " #" << p.data[2] << "\n";
                    assembler.ORR(static_cast<unsigned int>(p.data[0]), static_cast<unsigned int>(p.data[1]), static_cast<unsigned int>(p.data[2]), false);
                } else {
                    std::cout << "Expected desination register number, register number and operand\n";
                }
            } else {
                std::cout << "Not enough arguements\n";
            }
        } else if (cmd == "EOR ") {
            // EOR Command
            std::string params_s = program[pc].substr(4,program[pc].length() - 1);
            params p_r = extractRegisterParameters(params_s);
            params p_d = extractDirectParameters(params_s);
            params p_i = extractImmediateParameters(params_s);
            
            params p = combineParamsInOrder(p_r, p_i, p_d);
            
            std::cout << p.type[0] << p.type[1];
            
            if (p.type.size() == 3) {
                if ((p.type[0] == 0) && (p.type[1] == 0) && (p.type[2] == 0)) {
                    std::cout << "EOR R" << p.data[0] << " R" << p.data[1] << " R" << p.data[2] << "\n";
                    assembler.EOR(static_cast<unsigned int>(p.data[0]), static_cast<unsigned int>(p.data[1]), static_cast<unsigned int>(p.data[2]), true);
                } else if ((p.type[0] == 0) && (p.type[1] == 0) && (p.type[2] == 1)) {
                    std::cout << "EOR R" << p.data[0] << " R" << p.data[1] << " #" << p.data[2] << "\n";
                    assembler.EOR(static_cast<unsigned int>(p.data[0]), static_cast<unsigned int>(p.data[1]), static_cast<unsigned int>(p.data[2]), false);
                } else {
                    std::cout << "Expected desination register number, register number and operand\n";
                }
            } else {
                std::cout << "Not enough arguements\n";
            }
        } else if (cmd == "MVN ") {
            // MVN Command
            std::string params_s = program[pc].substr(4,program[pc].length() - 1);
            params p_r = extractRegisterParameters(params_s);
            params p_d = extractDirectParameters(params_s);
            params p_i = extractImmediateParameters(params_s);
            
            params p = combineParamsInOrder(p_r, p_i, p_d);
            
            std::cout << p.type[0] << p.type[1];
            
            if (p.type.size() == 2) {
                if ((p.type[0] == 0) && (p.type[1] == 0)) {
                    std::cout << "MVN R" << p.data[0] << " R" << p.data[1] << "\n";
                    assembler.MVN(static_cast<unsigned int>(p.data[0]), static_cast<unsigned int>(p.data[1]), true);
                } else if ((p.type[0] == 0) && (p.type[1] == 1)) {
                    std::cout << "MVN R" << p.data[0] << " #" << p.data[1] << "\n";
                    assembler.MVN(static_cast<unsigned int>(p.data[0]), static_cast<unsigned int>(p.data[1]), false);
                } else {
                    std::cout << "Expected desination register number and operand\n";
                }
            } else {
                std::cout << "Not enough arguements\n";
            }
        } else if (cmd == "LSL ") {
            // LSL Command
            std::string params_s = program[pc].substr(4,program[pc].length() - 1);
            params p_r = extractRegisterParameters(params_s);
            params p_d = extractDirectParameters(params_s);
            params p_i = extractImmediateParameters(params_s);
            
            params p = combineParamsInOrder(p_r, p_i, p_d);
            
            std::cout << p.type[0] << p.type[1];
            
            if (p.type.size() == 3) {
                if ((p.type[0] == 0) && (p.type[1] == 0) && (p.type[2] == 0)) {
                    std::cout << "LSL R" << p.data[0] << " R" << p.data[1] << " R" << p.data[2] << "\n";
                    assembler.LSL(static_cast<unsigned int>(p.data[0]), static_cast<unsigned int>(p.data[1]), static_cast<unsigned int>(p.data[2]), true);
                } else if ((p.type[0] == 0) && (p.type[1] == 0) && (p.type[2] == 1)) {
                    std::cout << "LSL R" << p.data[0] << " R" << p.data[1] << " #" << p.data[2] << "\n";
                    assembler.LSL(static_cast<unsigned int>(p.data[0]), static_cast<unsigned int>(p.data[1]), static_cast<unsigned int>(p.data[2]), false);
                } else {
                    std::cout << "Expected desination register number, register number and operand\n";
                }
            } else {
                std::cout << "Not enough arguements\n";
            }
        } else if (cmd == "LSR ") {
            // LSR Command
            std::string params_s = program[pc].substr(4,program[pc].length() - 1);
            params p_r = extractRegisterParameters(params_s);
            params p_d = extractDirectParameters(params_s);
            params p_i = extractImmediateParameters(params_s);
            
            params p = combineParamsInOrder(p_r, p_i, p_d);
            
            std::cout << p.type[0] << p.type[1];
            
            if (p.type.size() == 3) {
                if ((p.type[0] == 0) && (p.type[1] == 0) && (p.type[2] == 0)) {
                    std::cout << "LSR R" << p.data[0] << " R" << p.data[1] << " R" << p.data[2] << "\n";
                    assembler.LSR(static_cast<unsigned int>(p.data[0]), static_cast<unsigned int>(p.data[1]), static_cast<unsigned int>(p.data[2]), true);
                } else if ((p.type[0] == 0) && (p.type[1] == 0) && (p.type[2] == 1)) {
                    std::cout << "LSR R" << p.data[0] << " R" << p.data[1] << " #" << p.data[2] << "\n";
                    assembler.LSR(static_cast<unsigned int>(p.data[0]), static_cast<unsigned int>(p.data[1]), static_cast<unsigned int>(p.data[2]), false);
                } else {
                    std::cout << "Expected desination register number, register number and operand\n";
                }
            } else {
                std::cout << "Not enough arguements\n";
            }
        } else if (program[pc].substr(0,4) == "HALT") {
            // HALT
            pc = program.size();
        } else if (program[pc].back() == ':') {
            // Label
            std::cout << "Reached Label";
        } else {
            throw "ERROR";
        }
    }
    
    assembler.displayRegisters();
    
    return 0;
}

int main(int argc, const char * argv[]) {
    std::cout << "AQA Assembler Runtime\n";
    run("/Users/Oli/Documents/CRGS/Computer Science Society/AQA Assembler/AQA Assembler/test.asm");
    return 0;
}
