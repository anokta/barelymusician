#ifndef BARELYMUSICIAN_GODOT_PERFORMER_H_
#define BARELYMUSICIAN_GODOT_PERFORMER_H_

#include <barelymusician.h>

#include <cstdint>
#include <vector>

#include "godot_cpp/classes/node.hpp"
#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/variant/typed_array.hpp"

namespace barely::godot {

class BarelyTaskResource : public ::godot::Resource {
 public:
  void set_position(double position);
  void set_duration(double duration);
  void set_priority(int32_t priority);

  double get_position() const { return position_; }
  double get_duration() const { return duration_; }
  int32_t get_priority() const { return priority_; }

 private:
  GDCLASS(BarelyTaskResource, ::godot::Resource);
  static void _bind_methods();

  double position_ = 0.0;
  double duration_ = 1.0;
  int32_t priority_ = 0;
};

class BarelyPerformer : public ::godot::Node {
 public:
  BarelyPerformer();
  ~BarelyPerformer();

  void start();
  void stop();

  void set_position(double position);
  void set_loop_begin_position(double position);
  void set_loop_length(double length);
  void set_looping(bool looping);

  void set_tasks(const ::godot::TypedArray<::godot::Ref<BarelyTaskResource>>& tasks);

  double get_position() const;
  bool is_playing() const;

  double get_loop_begin_position() const { return loop_begin_position_; }
  double get_loop_length() const { return loop_length_; }
  bool is_looping() const { return looping_; }

  ::godot::TypedArray<::godot::Ref<BarelyTaskResource>> get_tasks() const { return tasks_; }

 private:
  GDCLASS(BarelyPerformer, ::godot::Node);
  static void _bind_methods();

  void _clear_tasks();
  void _on_task_changed();

  double loop_begin_position_ = 0.0;
  double loop_length_ = 1.0;
  bool looping_ = false;

  ::godot::TypedArray<::godot::Ref<BarelyTaskResource>> tasks_;
  std::vector<uint32_t> task_ids_;

  uint32_t performer_id_ = 0;
};

}  // namespace barely::godot

#endif
