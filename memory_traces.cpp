#include <iostream>
#include <fstream>
#include <string>
#include <vector>

class Frame{
    unsigned int page;
    int time_since_allocated;

public:
    Frame(){
        time_since_allocated = 0;
        page = 0; 
    }

    Frame(unsigned int page){
        this->page = page;
        time_since_allocated = 0;
    }

    void iterateTime(){
        time_since_allocated++;
    }

    void callPage(){
        
    }

    static void iterateTimeForAll(std::vector<Frame>& frames){
        for (auto it : frames) it.iterateTime();
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
        frames[i] = Frame();
        Frame::iterateTimeForAll(frames);
    }
    
}

int getTotalPageFaults(int available_frames, std::vector<unsigned int> &addresses){

    std::vector<Frame> frames(available_frames, Frame());
    int page_faults{0};
    populateFirstIterations(available_frames, addresses, frames);

    return page_faults;
}

int main(int argc, const char **argv){

    std::vector<unsigned int> addresses = chargePageAccesses(argv[1]);
    int available_frames = atoi(argv[2]);
    int total_page_faults = getTotalPageFaults(available_frames, addresses);

    for(auto it: addresses){
        std::cout << std::hex << " " << it << "\n";
    }
    
    return 0;
}