#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include <string.h>
#include <iostream>
using namespace std;

int32_t data[1000];
int32_t data_count = 0;

struct lab{
    char name[100];
    int32_t address;
};

lab labels[100];
int label_count = 0;

string instruction[1000];
int inst_count = 0;

int32_t assembled_inst[1000];
int32_t assembled_inst_count = 0;
int find_label(char* label){
    int i;
    for(i=0; i<label_count; i++){
        if(strcmp(labels[i].name, label) == 0) return labels[i].address;
    }
    return -1;
}
void data_and_label(char* file){

    //freopen(file, "r", stdin);
    freopen("example2_mod.s", "r", stdin);

    string line;

	char imsi[100];
	int section = -1, cnt;
	// .data : section = 0
	// .text : section = 1

	while (getline(cin, line)) {

	    if (section == -1){
            if (strstr(line.c_str(), ".data"))
                section = 0;
            continue;
	    }
	    if (section == 0){
            if (strstr(line.c_str(), ".text")){
                section = 1;
                continue;
            }
            if (strstr(line.c_str(), ".word")){

                strcpy(imsi, line.c_str());

                char* label;
                char* word;
                char* value;

                if (strstr(imsi,":")){
                    label = strtok(imsi, ":");
                    word = strtok(NULL, " \t");
                    value = strtok(NULL, " \t");

                    lab imsi_label;
                    strcpy(imsi_label.name, label);
                    imsi_label.address = 0x10000000 + data_count*4;
                    labels[label_count++] = imsi_label;

                }else{
                    word = strtok(imsi, " \t");
                    value = strtok(NULL, " \t");
                }

                data[data_count++] = (int32_t)strtol(value, NULL, 0);

            }
	    }
	    if (section == 1){

            strcpy(imsi, line.c_str());

            if (strstr(line.c_str(), ":")){
                lab imsi_label;
                strcpy(imsi_label.name, strtok(imsi, ":"));
                imsi_label.address = 0x400000 + assembled_inst_count * 4;
                labels[label_count++] = imsi_label;
            }else{

                instruction[inst_count++] = line;

                char* imsi1;
                imsi1 = strtok(imsi, " ,\t");

                if(strcmp(imsi1, "la") == 0){
                    char* imsi2;
                    char* imsi3;
                    imsi2 = strtok(NULL, " ,\t");
                    imsi3 = strtok(NULL, " ,\t");
                    cnt = find_label(imsi3);
                    if( (cnt & 65535) == 0 ) assembled_inst_count++;
                    else assembled_inst_count += 2;
                }else assembled_inst_count++;
            }
	    }
	}
	fclose(stdin);

}
void type_I(int32_t op, int32_t rs, int32_t rt, int32_t constant){

    int32_t result = op << 26;
    result |= (rs << 21);
    result |= (rt << 16);
    result |= (constant & 0xffff);

    assembled_inst[assembled_inst_count++] = result;
}
void type_R(int32_t op, int32_t rs, int32_t rt, int32_t rd, int32_t shamt, int32_t funct){

    int32_t result = op << 26;
    result |= (rs << 21);
    result |= (rt << 16);
    result |= (rd << 11);
    result |= (shamt << 6);
    result |= funct;

    assembled_inst[assembled_inst_count++] = result;

}
void type_J(int32_t op, int32_t address){

    int32_t result = (op << 26) | address;

    assembled_inst[assembled_inst_count++] = result;
}
int32_t reg_num(char* reg){

    char* imsi = strtok(reg, "$");
    return (int32_t)atoi(imsi);

}
void assemble(string inst){

    char* imsi1;
    char* imsi2;
    char* imsi3;
    char* imsi4;

    char imsi[100];
    strcpy(imsi, inst.c_str());

    imsi1 = strtok(imsi, " ,\t");

    if (imsi1){
        imsi2 = strtok(NULL, " ,\t");
        if (imsi2){
            imsi3 = strtok(NULL, " ,\t");
            if (imsi3){
                imsi4 = strtok(NULL, " ,\t");
            }
        }
    }

    string func = imsi1;

    if (func == "addiu"){

        type_I(9, reg_num(imsi3), reg_num(imsi2), (int32_t)strtol(imsi4, NULL, 0));

    }else if (func == "addu"){

        type_R(0, reg_num(imsi3), reg_num(imsi4), reg_num(imsi2), 0, 33);

    }else if (func == "and"){

        type_R(0, reg_num(imsi3), reg_num(imsi4), reg_num(imsi2), 0, 36);

    }else if (func == "andi"){

        type_I(12, reg_num(imsi3), reg_num(imsi2), (int32_t)strtol(imsi4, NULL, 0));

    }else if (func == "beq"){

        int32_t diff_addr = find_label(imsi4) - (0x400000 + assembled_inst_count * 4) - 4;
        type_I(4, reg_num(imsi2), reg_num(imsi3), diff_addr >> 2);

    }else if (func == "bne"){

        int32_t diff_addr = find_label(imsi4) - (0x400000 + assembled_inst_count * 4) - 4;
        type_I(5, reg_num(imsi2), reg_num(imsi3), diff_addr >> 2);

    }else if (func == "j"){

        type_J(2, find_label(imsi2) >> 2);

    }else if (func == "jal"){

        type_J(3, find_label(imsi2) >> 2);

    }else if (func == "jr"){

        type_R(0, reg_num(imsi2), 0, 0, 0, 8);

    }else if (func == "lui"){

        type_I(15, 0, reg_num(imsi2), (int32_t)strtol(imsi3, NULL, 0));

    }else if (func == "lw"){

        char* off = strtok(imsi3, "(");
        char* rs = strtok(NULL, ")");
        type_I(35, reg_num(rs), reg_num(imsi2), (int32_t)strtol(off, NULL, 0));

    }else if (func == "nor"){

        type_R(0, reg_num(imsi3), reg_num(imsi4), reg_num(imsi2), 0, 39);

    }else if (func == "or"){

        type_R(0, reg_num(imsi3), reg_num(imsi4), reg_num(imsi2), 0, 37);

    }else if (func == "ori"){

        type_I(13, reg_num(imsi3), reg_num(imsi2), (int32_t)strtol(imsi4, NULL, 0));

    }else if (func == "la"){

        int32_t cnt = find_label(imsi3);
        if( (cnt & 65535) == 0 )
            type_I(15, 0, reg_num(imsi2), cnt >> 16);
        else{
            type_I(15, 0, reg_num(imsi2), cnt >> 16);
            type_I(13, reg_num(imsi2), reg_num(imsi2), cnt & 0xffff);
        }

    }else if (func == "sltiu"){

        type_I(11, reg_num(imsi3), reg_num(imsi2), (int32_t)strtol(imsi4, NULL, 0));

    }else if (func == "sltu"){

        type_R(0, reg_num(imsi3), reg_num(imsi4), reg_num(imsi2), 0, 43);

    }else if (func == "sll"){

        type_R(0, 0, reg_num(imsi3), reg_num(imsi2), (int32_t)strtol(imsi4, NULL, 0), 0);

    }else if (func == "srl"){

        type_R(0, 0, reg_num(imsi3), reg_num(imsi2), (int32_t)strtol(imsi4, NULL, 0), 2);

    }else if (func == "sw"){

        char* off = strtok(imsi3, "(");
        char* rs = strtok(NULL, ")");
        type_I(43, reg_num(rs), reg_num(imsi2), (int32_t)strtol(off, NULL, 0));

    }else if (func == "subu"){

        type_R(0, reg_num(imsi3), reg_num(imsi4), reg_num(imsi2), 0, 35);

    }

}
string get_bin(int32_t cnt){

    int32_t cnt2;
    string sol = "";
    if (cnt < 0) cnt2 = cnt + 0x80000000;
    else cnt2 = cnt;
    for (int i =31; i>0; i--){
        if(cnt2%2 == 0) sol = "0" + sol;
        else sol = "1" + sol;
        cnt2 /= 2;
    }
    if (cnt < 0) return "1" + sol;
    else return "0" + sol;
}
int main(int argc, char* argv[]){

    char file[100];

    bool xxx= 0;

    cout << xxx;
    //strcpy(file, argv[1]);
    data_and_label(file);


    int32_t result;

    int i;

    assembled_inst_count = 0;

    for(i=0; i<inst_count; i++){

        assemble(instruction[i]);

    }

    freopen("output.o", "w", stdout);
    //file[strlen(file)-1] = 'o';
    //freopen(file, "w", stdout);
	cout << get_bin(assembled_inst_count * 4);
	cout << get_bin(data_count * 4);

	for(i=0; i < assembled_inst_count; i++)
        cout << get_bin(assembled_inst[i]);

    for(i=0; i < data_count; i++){
        cout << get_bin(data[i]);
    }


    fclose(stdout);


	return 0;
}


