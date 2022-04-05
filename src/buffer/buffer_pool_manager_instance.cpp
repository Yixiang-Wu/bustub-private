//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// buffer_pool_manager_instance.cpp
//
// Identification: src/buffer/buffer_pool_manager.cpp
//
// Copyright (c) 2015-2021, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "buffer/buffer_pool_manager_instance.h"

#include "common/macros.h"

namespace bustub {

BufferPoolManagerInstance::BufferPoolManagerInstance(size_t pool_size, DiskManager *disk_manager,
                                                     LogManager *log_manager)
    : BufferPoolManagerInstance(pool_size, 1, 0, disk_manager, log_manager) {}

BufferPoolManagerInstance::BufferPoolManagerInstance(size_t pool_size, uint32_t num_instances, uint32_t instance_index,
                                                     DiskManager *disk_manager, LogManager *log_manager)
    : pool_size_(pool_size),
      num_instances_(num_instances),
      instance_index_(instance_index),
      next_page_id_(instance_index),
      disk_manager_(disk_manager),
      log_manager_(log_manager) {
  BUSTUB_ASSERT(num_instances > 0, "If BPI is not part of a pool, then the pool size should just be 1");
  BUSTUB_ASSERT(
      instance_index < num_instances,
      "BPI index cannot be greater than the number of BPIs in the pool. In non-parallel case, index should just be 1.");
  // We allocate a consecutive memory space for the buffer pool.
  pages_ = new Page[pool_size_];
  //  replacer_ = new LRUReplacer(pool_size);
  replacer_ = new ClockReplacer(pool_size);

  // Initially, every page is in the free list.
  for (size_t i = 0; i < pool_size_; ++i) {
    free_list_.emplace_back(static_cast<int>(i));
  }
}

BufferPoolManagerInstance::~BufferPoolManagerInstance() {
  delete[] pages_;
  delete replacer_;
}

bool BufferPoolManagerInstance::FlushPgImp(page_id_t page_id) {
  // Make sure you call DiskManager::WritePage!

  // 我认为flush之后就不是脏页，姑且这样写一下
  latch_.lock();

  // find page in buffer pool
  bool flag = false;
  size_t index;
  for(index = 0; index < pool_size_; index++){
    if(pages_[index].page_id_ == page_id){
      flag = true;
      break;
    }
  }

  // page is not in buffer pool
  if(flag == false){ latch_.unlock(); return false; }

  // flush and set is_dirty to false
  disk_manager_->WritePage(page_id, pages_[index].GetData());
  pages_[index].is_dirty_ = false;
  latch_.unlock();
  return true;
}

void BufferPoolManagerInstance::FlushAllPgsImp() {
  // You can do it!

  // 同样，是设置is_dirty = false的版本
  latch_.lock();

  size_t index;
  for(index = 0; index < pool_size_; index++){
    disk_manager_->WritePage(pages_[index].page_id_, pages_[index].GetData());
    pages_[index].is_dirty_ = false;
  }

  latch_.unlock();
}

Page *BufferPoolManagerInstance::NewPgImp(page_id_t *page_id) {
  // 0.   Make sure you call AllocatePage!
  // 1.   If all the pages in the buffer pool are pinned, return nullptr.
  // 2.   Pick a victim page P from either the free list or the replacer. Always pick from the free list first.
  // 3.   Update P's metadata, zero out memory and add P to the page table.
  // 4.   Set the page ID output parameter. Return a pointer to P.

  latch_.lock();

  // no place
  if(free_list_.size() == 0 && replacer_->Size() == 0){
    latch_.unlock(); return nullptr;
  }

  *page_id = AllocatePage();

  // free list
  if(free_list_.size() != 0){
    frame_id_t frame_id = *free_list_.begin();
    free_list_.erase(free_list_.begin());

    pages_[frame_id].ResetMemory();
    pages_[frame_id].page_id_ = *page_id;
    pages_[frame_id].pin_count_ = 1;
    pages_[frame_id].is_dirty_ = false;

    latch_.unlock();
    return &pages_[frame_id];
  }

  // replacer
  if(replacer_->Size() != 0){
    frame_id_t value = 0;
    frame_id_t * frame_id = &value; 
    replacer_->Victim(frame_id);
    if(pages_[*frame_id].IsDirty()) {
      // remember the lock
      latch_.unlock();
      FlushPgImp(pages_[*frame_id].page_id_);
      latch_.lock();
    }

    pages_[*frame_id].ResetMemory();
    pages_[*frame_id].page_id_ = *page_id;
    pages_[*frame_id].pin_count_ = 1;
    pages_[*frame_id].is_dirty_ = false;    
    
    latch_.unlock();
    return &pages_[*frame_id];
  }

  // default
  latch_.unlock();
  return nullptr;
}

Page *BufferPoolManagerInstance::FetchPgImp(page_id_t page_id) {
  // 1.     Search the page table for the requested page (P).
  // 1.1    If P exists, pin it and return it immediately.
  // 1.2    If P does not exist, find a replacement page (R) from either the free list or the replacer.
  //        Note that pages are always found from the free list first.
  // 2.     If R is dirty, write it back to the disk.
  // 3.     Delete R from the page table and insert P.
  // 4.     Update P's metadata, read in the page content from disk, and then return a pointer to P.
  
  latch_.lock();

  // if page in buffer pool
  for(size_t i = 0; i < pool_size_; i++){
    if(pages_[i].page_id_ == page_id){
      pages_[i].pin_count_++;
      latch_.unlock();
      return &pages_[i];
    }
  }

  // if not have frame to put page in
  if(free_list_.size() == 0 && replacer_->Size() == 0){
      latch_.unlock();
      return nullptr;
  }

  // if have free frame
  if(free_list_.size() != 0){
    frame_id_t frame_id = *free_list_.begin();
    free_list_.erase(free_list_.begin());

    char page_data[PAGE_SIZE];
    disk_manager_->ReadPage(page_id, page_data);
    std::memcpy(pages_[frame_id].GetData(), page_data, PAGE_SIZE);
    pages_[frame_id].page_id_ = page_id;
    pages_[frame_id].pin_count_ = 1;
    pages_[frame_id].is_dirty_ = false;

    latch_.unlock();
    return &pages_[frame_id];
  }

  // if can find a frame in replacer
  if(replacer_->Size() != 0){
    frame_id_t value = 0;
    frame_id_t * frame_id = &value;
    replacer_->Victim(frame_id);
    if(pages_[*frame_id].IsDirty()) {
      latch_.unlock();
      FlushPgImp(pages_[*frame_id].page_id_);
      latch_.lock();
    }
    
    char page_data[PAGE_SIZE];
    disk_manager_->ReadPage(page_id, page_data);
    std::memcpy(pages_[*frame_id].GetData(), page_data, PAGE_SIZE);
    pages_[*frame_id].page_id_ = page_id;
    pages_[*frame_id].pin_count_ = 1;
    pages_[*frame_id].is_dirty_ = false;
    
    latch_.unlock();
    return &pages_[*frame_id];
  }

  // default
  latch_.unlock();
  return nullptr;
}

bool BufferPoolManagerInstance::DeletePgImp(page_id_t page_id) {
  // 0.   Make sure you call DeallocatePage!
  // 1.   Search the page table for the requested page (P).
  // 1.   If P does not exist, return true.
  // 2.   If P exists, but has a non-zero pin-count, return false. Someone is using the page.
  // 3.   Otherwise, P can be deleted. Remove P from the page table, reset its metadata and return it to the free list.
  
  latch_.lock();

  // find page in buffer pool
  bool flag = false;
  size_t index;
  for(index = 0; index < pool_size_; index++){
    if(pages_[index].page_id_ == page_id){
      flag = true;
      break;
    }
  }

  // page is not in buffer pool
  if(flag == false) { latch_.unlock(); return true; } 

  // pin != 0
  if(pages_[index].pin_count_ != 0) { latch_.unlock(); return false; } 

  /*
  can delete, remember 3 things:  
    1. if dirty, we need flush. 
    2. push it into free list. 
    3. reset meta_data
  */
  if(pages_[index].IsDirty()){
    latch_.unlock();
    FlushPgImp(pages_[index].page_id_);
    latch_.lock();
  }
  pages_[index].ResetMemory();
  pages_[index].page_id_ = INVALID_PAGE_ID;
  pages_[index].is_dirty_ = false;
  pages_[index].pin_count_ = 0;
  free_list_.push_back(index);

  // 为了完整性
  DeallocatePage(page_id);

  latch_.unlock();
  return true;
}

bool BufferPoolManagerInstance::UnpinPgImp(page_id_t page_id, bool is_dirty) { 
  
  latch_.lock();

  // find page in buffer pool
  bool flag = false;
  size_t index;
  for(index = 0; index < pool_size_; index++){
    if(pages_[index].page_id_ == page_id){
      flag = true;
      break;
    }
  }

  // page is not in buffer pool
  if(flag == false){ latch_.unlock(); return false; }

  // error
  if(pages_[index].pin_count_ == 0) { latch_.unlock(); return false; }

  // unpin and set is_dirty
  pages_[index].pin_count_--;
  if(pages_[index].pin_count_ == 0){
    replacer_->Unpin(index);
  }

  pages_[index].is_dirty_ = pages_[index].is_dirty_ || is_dirty;
  latch_.unlock();
  return true;
}

page_id_t BufferPoolManagerInstance::AllocatePage() {
  const page_id_t next_page_id = next_page_id_;
  next_page_id_ += num_instances_;
  ValidatePageId(next_page_id);
  return next_page_id;
}

void BufferPoolManagerInstance::ValidatePageId(const page_id_t page_id) const {
  assert(page_id % num_instances_ == instance_index_);  // allocated pages mod back to this BPI
}

}  // namespace bustub
