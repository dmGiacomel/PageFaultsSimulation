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
    
    std::vector<Frame> frames(available_frames, Frame());
    std::cout << "frame vazio = " << frames[0].page << std::endl;

    // coloca as páginas nos frames vazios iniciais
    // e contabiliza quantas falhas ocorreram
    int page_faults = populateFirstIterations(available_frames, addresses, frames);
    std::cout << "falhas de páginas inciais = " << page_faults << std::endl;

    // continua lendo os acessos de endereços
    for(int i = available_frames; i < addresses.size(); i++){
        int menor_index = 1000000, index_frame;
        bool is_in_frames = false;
        Frame frame_teste(addresses[i], i);

        std::cout << "mostrando frames\n";
        for(auto it: frames){
            std::cout << std::hex << " " << it.page << " - " << it.page_index << std::endl;
        }

        // varre o vetor de frames para ver se
        // a página já está lá
        for(int j = 0; j < available_frames; j++){
            if(frame_teste.page == frames[j].page){
                is_in_frames = true;

                // atualiza o index (para constar que a paǵina foi usada novamente)
                frames[j].page_index = frame_teste.page_index;
            }

            // ja pega o menor índice aqui caso precise pra não ter que
            // percorrer os frames de novo depois
            if(frames[j].page_index < menor_index)
                menor_index = frames[j].page_index, index_frame = j;
        }
        // std::cout << "posição da pagina com menor index == " << index_frame << std::endl;

        // se não estiver nos frames, substitui por aquele
        // com menor frame ou coloca em um frame vazio
        if(!is_in_frames){
            bool alr_put = false; // já colocou no frame?
            page_faults++; // de qualquer forma, ocorreu uma falha de página

            // primeiro ve se tem algum frame vazio pra colocar
            for(int k = 0; k < available_frames; k++){
                if(frames[k].page == 0){
                    std::cout << "entrou no frame vazio, com k = " << k << std::endl;
                    frames[k].page = frame_teste.page;
                    frames[k].page_index = frame_teste.page_index;
                    alr_put = true;
                    break;
                }
            }
            
            // se não, pega o menor índice e substitui
            if(!alr_put){
                std::cout << "entrou na substituição por menor\n";
                frames[index_frame].page = frame_teste.page;
                frames[index_frame].page_index = frame_teste.page_index;
            }
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