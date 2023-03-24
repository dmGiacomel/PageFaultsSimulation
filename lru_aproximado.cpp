#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include<ctime>

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

// unsigned int populateFirstIterations(int available_frames, std::vector<unsigned int> &addresses, std::vector<Frame> &frames){
//     unsigned int falhas_iniciais = 0;
    

//     for(int i = 0; i < available_frames; i++){
//         bool is_in_frames = false;
//         for(int j = 0; j < available_frames; j++){
//             if(frames[j].page == addresses[i]){
//                 is_in_frames = true;
//             }
//         }
//         if(!is_in_frames)
//             frames[i] = Frame(addresses[i], i), falhas_iniciais++;
//     }

//     return falhas_iniciais;
// }

int getTotalPageFaults(int available_frames, std::vector<unsigned int> &addresses){
    std::vector<Frame> frames(available_frames, Frame());
    std::map<unsigned int, unsigned char> bits_referencia;
    std::map<unsigned int, bool> foi_chamado;
    unsigned char chegou = 0x80;


    // coloquei o populateFirstIterations aqui porque fica mais fácil
    // de controlar e entender
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

            // printando o map bits_referencia
            std::cout << "map bits_referencia\n";
            for(const auto& elem : bits_referencia){
                std::cout << elem.first << " " << elem.second << "\n";
            }

            foi_chamado.insert({addresses[i], true});

            // printando o map foi_chamado
            std::cout << "map foi_chamado\n";
            for(const auto& elem : foi_chamado){
                std::cout << elem.first << " " << elem.second << "\n";
            }
        }
    }

    int page_faults = falhas_iniciais;

    
    // quando o clock disparar
    // reinicializar o map foi_chamado
    // e deslocar os bits_referencia de acordo com o map foi_chamado
    
    // vamos precisar armazenar o índice em que parou no clock
    // inicia em available_frames
    int ind_atual = available_frames;
    int delay = 1000;
    delay *= CLOCKS_PER_SEC;
    int i;

    reinicia_loop:
    clock_t now = clock();

    while(clock() - now < delay){
        for(i = ind_atual; i < addresses.size(); i++){
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
                    foi_chamado[frame_teste.page] = true; // diz que foi chamada nesse intervalo
                }
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
                // quando for subsituir, escolher a página com menor número do map bits_referencia
                if(!alr_put){
                    std::cout << "entrou na substituição por menor\n";
                    for(int k = 0; k < available_frames; k++){
                        if(menor_index > bits_referencia[frames[k].page]){
                            menor_index = k;
                        }
                    }
                    frames[menor_index].page = frame_teste.page;
                    frames[menor_index].page_index = frame_teste.page_index;
                }
            }
        }
        if(i < addresses.size()){ // ainda não terminou de processar as páginas
            ind_atual = i - 1; // ajusta o índice atual
            std::cout << "valor de i = " << i << std::endl;
            // fazer shift dos bits de referencia adicionais de acordo
            // com o map foi_chamado
            for(auto it: bits_referencia){
                if(foi_chamado[it.first]){
                    it.second >> 1; // faz shift pra direita em um bit
                    it.second | 0x80; // como foi chamado, faz OR com 10000000
                }
                else{
                    it.second >> 1; // só faz shift pra direita em um bit, sem OR
                }
            }

            // reinicializar o map foi_chamado
            for(auto it: foi_chamado){
                it.second = false;
            }

            goto reinicia_loop;   // recomeça o loop
        }
    }
    return page_faults;
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
    std::cout << "entrou na main" << std::endl;

    std::vector<unsigned int> addresses = chargePageAccesses(argv[1]);
    std::cout << "vendo addresses\n";
    // for(auto it: addresses){
    //     std::cout << std::hex << " " << it << "\n";
    // }

    int available_frames = atoi(argv[2]);
    int total_page_faults = getTotalPageFaults(available_frames, addresses);
    std::cout << "page faults no LRU = " << total_page_faults << std::endl;

    return 0;
}