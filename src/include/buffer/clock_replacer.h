//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// clock_replacer.h
//
// Identification: src/include/buffer/clock_replacer.h
//
// Copyright (c) 2015-2019, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include <list>
#include <mutex>  // NOLINT
#include <vector>

#include "buffer/replacer.h"
#include "common/config.h"

namespace bustub {

/**
 * ClockReplacer implements the clock replacement policy, which approximates the Least Recently Used policy.
 */
class ClockReplacer : public Replacer {
 public:
  /**
   * Create a new ClockReplacer.
   * @param num_pages the maximum number of pages the ClockReplacer will be required to store
   */
  // PINs全为1，Ref随意
  explicit ClockReplacer(size_t num_pages);

  /**
   * Destroys the ClockReplacer.
   */
  ~ClockReplacer() override;

  bool Victim(frame_id_t *frame_id) override;
  void Pin(frame_id_t frame_id) override;
  void Unpin(frame_id_t frame_id) override;
  size_t Size() override;

 private:
  // TODO(student): implement me!

  // 注意我们针对的是，内存中占据了页框的页，我们选择一页，丢掉
  // ref=1 表示该页最近被使用到，pin=1 表示该页正被进程使用， miss=1 表示该页已经丢出去了
  std::vector<bool> Refs;
  std::vector<bool> PINs;
  std::vector<bool> Miss;
  std::list<frame_id_t> Clock_list;
  std::list<frame_id_t>::iterator clock_hand_;
  int num_pages_;
  int num_pages_available_;
};

}  // namespace bustub
