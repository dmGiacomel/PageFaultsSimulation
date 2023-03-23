#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>

class Frame{

public:
    unsigned int page;
    // int time_since_allocated;
    unsigned int page_index;

    Frame(){
        page_index = 0;
        page = 0; 
    }

    Frame(unsigned int page, unsigned int page_index){
        this->page = page;
        this->page_index = page_index;
    }

};

//there is no page fault when allocating frames to the first n free frames
unsigned int populateFirstIterations(int available_frames, std::vector<unsigned int> &addresses, std::vector<Frame> &frames){
    unsigned int falhas_iniciais = 0;
    

    for(int i = 0; i < available_frames; i++){
        bool is_in_frames = false;
        for(int j = 0; j < available_frames; j++){
            if(frames[j].page == addresses[i]){
                is_in_frames = true;
            }
        }
        if(!is_in_frames)
            frames[i] = Frame(addresses[i], i), falhas_iniciais++;
    }

    return falhas_iniciais;
}

int getTotalPageFaults(int available_frames, std::vector<unsigned int> &addresses){
    std::vector<Frame> frames(available_frames, Frame());
    std::map<unsigned int, unsigned char> bits_referencia;
    std::map<unsigned int, bool> foi_chamado;
    unsigned char chegou = 0x80;

    int falhas_iniciais = 0;
    for(int i = 0; i < available_frames; i++){
        bool is_in_frames = false;
        for(int j = 0; j < available_frames; j++){
            if(frames[j].page == addresses[i]){
                is_in_frames = true;
            }
        }
        if(!is_in_frames){
            frames[i] = Frame(addresses[i], i); 
            falhas_iniciais++;
            bits_referencia.insert({addresses[i], chegou});
            foi_chamado.insert({addresses[i], true});
        }
    }
    


}

static std::vector<unsigned int> chargePageAccesses(const char *filename){
    
    int aux = 0;
    std::ifstream fin;
    std::string line;
    std::vector<unsigned int> addresses;
    const unsigned int mask = 0xfffff000;

    fin.open(filename);

    while (std::getline(fin, line)){
        addresses.push_back((std::stoul(line, nullptr, 16) & mask) >> 12);
    }

    line.clear();
    fin.close();
    return addresses;
}

int main(int argc, const char **argv){
    
    std::vector<unsigned int> addresses = chargePageAccesses(argv[1]);
    // std::cout << "vendo addresses\n";
    // for(auto it: addresses){
    //     std::cout << std::hex << " " << it << "\n";
    // }
    int available_frames = atoi(argv[2]);


    return 0;
}