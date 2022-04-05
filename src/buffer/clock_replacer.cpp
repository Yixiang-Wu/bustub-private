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
    this->num_frames_ = num_pages;
    this->num_frames_available_ = 0;
    // Ref随意初始化，Pin初始化为1
    this->Refs = std::vector<bool>(num_pages, true);
    this->PINs = std::vector<bool>(num_pages, true);
    for(frame_id_t i = 0; i < this->num_frames_; i++)
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
    latch.lock();

    if(num_frames_available_ == 0){
        latch.unlock();
        return false;
    }

    while(true){
        *frame_id = *clock_hand_;
        
        if(PINs[*frame_id] == true) ;

        else{
            if(Refs[*frame_id] == false){
                break;
            }
            else{
                Refs[*frame_id] = false;
            }
        }

        clock_hand_++;
        if(clock_hand_ == Clock_list.end()) clock_hand_ = Clock_list.begin();
    }

    /* if I use Pin(*frame_id) here, it will have a deadlock  */
    PINs[*frame_id] = true;
    num_frames_available_ --; 

    clock_hand_++;
    if(clock_hand_ == Clock_list.end()) clock_hand_ = Clock_list.begin();
    latch.unlock();
    return true;
}

void ClockReplacer::Pin(frame_id_t frame_id) {
    latch.lock();

    if(PINs[frame_id] == true) { latch.unlock(); return; }
    PINs[frame_id] = true;
    num_frames_available_ --;

    latch.unlock();
}

void ClockReplacer::Unpin(frame_id_t frame_id) {
    latch.lock();

    if(PINs[frame_id] == false) { latch.unlock(); return; }
    PINs[frame_id] = false;
    Refs[frame_id] = true;
    num_frames_available_ ++;

    latch.unlock();
}

size_t ClockReplacer::Size() { 
    latch.lock();
    latch.unlock();
    return num_frames_available_;
}

}  // namespace bustub
