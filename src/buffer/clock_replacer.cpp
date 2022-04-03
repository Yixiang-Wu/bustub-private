//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// clock_replacer.cpp
//
// Identification: src/buffer/clock_replacer.cpp
//
// Copyright (c) 2015-2019, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "buffer/clock_replacer.h"
#include "common/logger.h"
namespace bustub {

ClockReplacer::ClockReplacer(size_t num_pages) {
    this->num_pages_ = num_pages;
    this->num_pages_available_ = 0;
    // Ref随意初始化，Pin初始化为1，Miss初始化为0
    this->Refs = std::vector<bool>(num_pages, true);
    this->PINs = std::vector<bool>(num_pages, true);
    this->Miss = std::vector<bool>(num_pages, false);
    for(frame_id_t i = 0; i < this->num_pages_; i++)
        this->Clock_list.push_back(i);
    this->clock_hand_ = Clock_list.begin();
}

ClockReplacer::~ClockReplacer(){
    // delete Refs;
    // delete PINs;
    // delete Miss;
    // delete Clock_list;
    // delete clock_hand_;
}

bool ClockReplacer::Victim(frame_id_t *frame_id) { 
    if(num_pages_available_ == 0) return false;

    frame_id_t page_id;

    while(true){
        page_id = *clock_hand_;
        
        if(PINs[page_id] == true) ;

        else{
            if(Refs[page_id] == false){
                break;
            }
            else{
                Refs[page_id] = false;
            }
        }

        clock_hand_++;
        if(clock_hand_ == Clock_list.end()) clock_hand_ = Clock_list.begin();
    }

    *frame_id = *clock_hand_;
    Miss[*frame_id] = true;
    num_pages_available_--;
    clock_hand_ = Clock_list.erase(clock_hand_);
    if(clock_hand_ == Clock_list.end()) clock_hand_ = Clock_list.begin();
    return true;
}

void ClockReplacer::Pin(frame_id_t frame_id) {
    if(PINs[frame_id] == true) return;
    if(Miss[frame_id] == true) return;
    PINs[frame_id] = true;
    num_pages_available_ --;
}

void ClockReplacer::Unpin(frame_id_t frame_id) {
    if(Miss[frame_id] == true) return;
    if(PINs[frame_id] == false) return;
    PINs[frame_id] = false;
    Refs[frame_id] = true;
    num_pages_available_ ++;
}

size_t ClockReplacer::Size() { 
    return num_pages_available_;
}

}  // namespace bustub
