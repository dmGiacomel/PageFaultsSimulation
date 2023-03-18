// ao executar, devem ser passados os seguintes argumentos (respectivamente)
// arquivo de entrada .txt      quantidade de frames livres


#include <iostream>
#include <fstream>
#include <string>
#include <vector>

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

    int menor_index = 1000000, index_frame;
    std::vector<Frame> frames(available_frames, Frame());
    std::cout << "frame vazio = " << frames[0].page << std::endl;
    int page_faults = populateFirstIterations(available_frames, addresses, frames);

    // continua lendo os acessos de endereços
    for(int i = available_frames; i < addresses.size(); i++){
        bool is_in_frames = false;
        Frame frame_teste(addresses[i], i);


        for(int j = 0; j < available_frames; j++){
            if(frame_teste.page == frames[j].page){
                is_in_frames = true;
            }

            // ja pega os índices aqui caso precise pra não ter que
            // percorrer os frames de novo depois
            if(frames[j].page_index < menor_index)
                menor_index = frames[j].page_index, index_frame = j;
        }

        if(!is_in_frames){
            bool alr_put = false;
            page_faults++;

            // primeiro ve se tem algum frame vazio pra colocar
            for(auto it: frames){
                if(it.page == 0){
                    it = frame_teste;
                    alr_put = true;
                    break;
                }
            }

            // se não, pega o menor índice e substitui
            if(!alr_put)
                frames[index_frame] = frame_teste;
        }
    }

    return page_faults;
}

int main(int argc, const char **argv){
    
    std::vector<unsigned int> addresses = chargePageAccesses(argv[1]);
    // std::cout << "vendo addresses\n";
    // for(auto it: addresses){
    //     std::cout << std::hex << " " << it << "\n";
    // }
    int available_frames = atoi(argv[2]);
    int total_page_faults = getTotalPageFaults(available_frames, addresses);
    
    std::cout << "page faults no LRU = " << total_page_faults << std::endl;


    
    return 0;
}