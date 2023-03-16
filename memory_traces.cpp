#include <iostream>
#include <fstream>
#include <string>
#include <vector>

class Frame{
    unsigned int page;
    // int time_since_allocated;
    unsigned int page_index;

public:
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
    std::ifstream fin;
    std::string line;
    std::vector<unsigned int> addresses;
    const unsigned int mask = 0xfffff000;

    fin.open(filename);

    while (std::getline(fin, line))
        addresses.push_back((std::stoul(line, nullptr, 16) & mask) >> 12);

    line.clear();
    fin.close();

    return addresses;
}

//there is no page fault when allocating frames to the first n free frames
void populateFirstIterations(int available_frames, std::vector<unsigned int> &addresses, std::vector<Frame> &frames){
    for(int i = 0; i < available_frames; i++){
        frames[i] = Frame(addresses[i], i);
    }
}

int getTotalPageFaults(int available_frames, std::vector<unsigned int> &addresses){

    std::vector<Frame> frames(available_frames, Frame());
    int page_faults = available_frames;
    populateFirstIterations(available_frames, addresses, frames);

    for(int i = available_frames; i < addresses.size(); i++){
        Frame frame_teste(addresses[i], i);
    }

    return page_faults;
}

int main(int argc, const char **argv){

    std::vector<unsigned int> addresses = chargePageAccesses(argv[1]);
    int available_frames = atoi(argv[2]);
    int total_page_faults = getTotalPageFaults(available_frames, addresses);
    std::cout << "page faults inicial = " << total_page_faults << std::endl;

    // for(auto it: addresses){
    //     std::cout << std::hex << " " << it << "\n";
    // }
    
    return 0;
}