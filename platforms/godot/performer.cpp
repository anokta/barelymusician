#include "godot/performer.h"

#include <algorithm>

#include "godot/engine.h"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/callable.hpp"

namespace barely::godot {

using ::godot::Callable;
using ::godot::ClassDB;
using ::godot::D_METHOD;
using ::godot::MethodInfo;
using ::godot::PropertyHint;
using ::godot::PropertyInfo;
using ::godot::Ref;
using ::godot::TypedArray;
using ::godot::Variant;

void BarelyTaskResource::set_position(double position) {
  position_ = position;
  emit_changed();
}

void BarelyTaskResource::set_duration(double duration) {
  static constexpr double kMinTaskDuration = 1e-6;
  duration_ = std::max(duration, kMinTaskDuration);
  emit_changed();
}

void BarelyTaskResource::set_priority(int32_t priority) {
  priority_ = priority;
  emit_changed();
}

void BarelyTaskResource::_bind_methods() {
  ClassDB::bind_method(D_METHOD("set_position", "position"), &BarelyTaskResource::set_position);
  ClassDB::bind_method(D_METHOD("get_position"), &BarelyTaskResource::get_position);

  ClassDB::bind_method(D_METHOD("set_duration", "duration"), &BarelyTaskResource::set_duration);
  ClassDB::bind_method(D_METHOD("get_duration"), &BarelyTaskResource::get_duration);

  ClassDB::bind_method(D_METHOD("set_priority", "priority"), &BarelyTaskResource::set_priority);
  ClassDB::bind_method(D_METHOD("get_priority"), &BarelyTaskResource::get_priority);

  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "position"), "set_position", "get_position");
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "duration"), "set_duration", "get_duration");
  ADD_PROPERTY(PropertyInfo(Variant::INT, "priority"), "set_priority", "get_priority");

  ADD_SIGNAL(MethodInfo("task_begin"));
  ADD_SIGNAL(MethodInfo("task_end"));
}

BarelyPerformer::BarelyPerformer() {
  BarelyEngine_CreatePerformer(BarelyEngine::get_singleton()->get(), &performer_id_);
  tasks_ = TypedArray<Ref<BarelyTaskResource>>();
}

BarelyPerformer::~BarelyPerformer() {
  _clear_tasks();
  BarelyEngine_DestroyPerformer(BarelyEngine::get_singleton()->get(), performer_id_);
}

void BarelyPerformer::start() {
  BarelyPerformer_Start(BarelyEngine::get_singleton()->get(), performer_id_);
}

void BarelyPerformer::stop() {
  BarelyPerformer_Stop(BarelyEngine::get_singleton()->get(), performer_id_);
}

void BarelyPerformer::set_position(double position) {
  BarelyPerformer_SetPosition(BarelyEngine::get_singleton()->get(), performer_id_, position);
}

void BarelyPerformer::set_loop_begin_position(double loop_begin_position) {
  loop_begin_position_ = loop_begin_position;

  BarelyPerformer_SetLoopBeginPosition(BarelyEngine::get_singleton()->get(), performer_id_,
                                       loop_begin_position_);
}

void BarelyPerformer::set_loop_length(double loop_length) {
  loop_length_ = loop_length;

  BarelyPerformer_SetLoopLength(BarelyEngine::get_singleton()->get(), performer_id_, loop_length_);
}

void BarelyPerformer::set_looping(bool looping) {
  looping_ = looping;

  BarelyPerformer_SetLooping(BarelyEngine::get_singleton()->get(), performer_id_, looping_);
}

void BarelyPerformer::set_tasks(const TypedArray<Ref<BarelyTaskResource>>& tasks) {
  for (int i = 0; i < tasks_.size(); ++i) {
    Ref<BarelyTaskResource> task = tasks_[i];
    if (task.is_valid()) {
      task->disconnect("changed", Callable(this, "_on_task_changed"));
    }
  }
  tasks_ = tasks;
  for (int i = 0; i < tasks_.size(); ++i) {
    Ref<BarelyTaskResource> task = tasks_[i];
    if (task.is_valid()) {
      task->connect("changed", Callable(this, "_on_task_changed"));
    }
  }
  _on_task_changed();
}

double BarelyPerformer::get_position() const {
  double position = 0.0;
  BarelyPerformer_GetPosition(BarelyEngine::get_singleton()->get(), performer_id_, &position);
  return position;
}

bool BarelyPerformer::is_playing() const {
  bool playing = false;
  BarelyPerformer_IsPlaying(BarelyEngine::get_singleton()->get(), performer_id_, &playing);
  return playing;
}

void BarelyPerformer::_bind_methods() {
  ClassDB::bind_method(D_METHOD("start"), &BarelyPerformer::start);
  ClassDB::bind_method(D_METHOD("stop"), &BarelyPerformer::stop);

  ClassDB::bind_method(D_METHOD("set_position", "position"), &BarelyPerformer::set_position);
  ClassDB::bind_method(D_METHOD("get_position"), &BarelyPerformer::get_position);

  ClassDB::bind_method(D_METHOD("is_playing"), &BarelyPerformer::is_playing);

  ClassDB::bind_method(D_METHOD("set_loop_begin_position", "position"),
                       &BarelyPerformer::set_loop_begin_position);
  ClassDB::bind_method(D_METHOD("get_loop_begin_position"),
                       &BarelyPerformer::get_loop_begin_position);
  ClassDB::bind_method(D_METHOD("set_loop_length", "length"), &BarelyPerformer::set_loop_length);
  ClassDB::bind_method(D_METHOD("get_loop_length"), &BarelyPerformer::get_loop_length);
  ClassDB::bind_method(D_METHOD("set_looping", "looping"), &BarelyPerformer::set_looping);
  ClassDB::bind_method(D_METHOD("is_looping"), &BarelyPerformer::is_looping);

  ClassDB::bind_method(D_METHOD("set_tasks", "tasks"), &BarelyPerformer::set_tasks);
  ClassDB::bind_method(D_METHOD("get_tasks"), &BarelyPerformer::get_tasks);
  ClassDB::bind_method(D_METHOD("_on_task_changed"), &BarelyPerformer::_on_task_changed);

  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "loop_begin_position"), "set_loop_begin_position",
               "get_loop_begin_position");
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "loop_length"), "set_loop_length", "get_loop_length");
  ADD_PROPERTY(PropertyInfo(Variant::BOOL, "looping"), "set_looping", "is_looping");

  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "position"), "set_position", "get_position");
  ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "tasks", PropertyHint::PROPERTY_HINT_ARRAY_TYPE,
                            "BarelyTaskResource"),
               "set_tasks", "get_tasks");
}

void BarelyPerformer::_clear_tasks() {
  for (uint32_t task_id : task_ids_) {
    BarelyEngine_DestroyTask(BarelyEngine::get_singleton()->get(), task_id);
  }
  task_ids_.clear();
}

void BarelyPerformer::_on_task_changed() {
  _clear_tasks();

  for (int i = 0; i < tasks_.size(); ++i) {
    Ref<BarelyTaskResource> task = tasks_[i];
    if (task.is_null()) {
      continue;
    }

    uint32_t task_id = 0;
    BarelyEngine_CreateTask(
        BarelyEngine::get_singleton()->get(), performer_id_, task->get_position(),
        task->get_duration(), task->get_priority(),
        [](BarelyEventType type, void* user_data) {
          BarelyTaskResource* task = static_cast<BarelyTaskResource*>(user_data);

          if (!task) return;

          if (type == BarelyEventType_kBegin) {
            task->emit_signal("task_begin");
          } else if (type == BarelyEventType_kEnd) {
            task->emit_signal("task_end");
          }
        },
        task.ptr(), &task_id);
    task_ids_.push_back(task_id);
  }
}

}  // namespace barely::godot
