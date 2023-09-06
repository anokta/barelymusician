#ifndef BARELYMUSICIAN_COMMON_MACROS_H_
#define BARELYMUSICIAN_COMMON_MACROS_H_

#include "barelymusician/barelymusician.h"

#ifdef __cplusplus
#define BARELY_GENERATE_CUSTOM_EFFECT_DEFINITION(name, ...)       \
  extern "C" {                                                    \
  _BARELY_GENERATE_CUSTOM_EFFECT_DEFINITION(C, name, __VA_ARGS__) \
  }                                                               \
  _BARELY_GENERATE_CUSTOM_EFFECT_DEFINITION(CPP, name, __VA_ARGS__)

#define BARELY_GENERATE_CUSTOM_INSTRUMENT_DEFINITION(name, ...)       \
  extern "C" {                                                        \
  _BARELY_GENERATE_CUSTOM_INSTRUMENT_DEFINITION(C, name, __VA_ARGS__) \
  }                                                                   \
  _BARELY_GENERATE_CUSTOM_INSTRUMENT_DEFINITION(CPP, name, __VA_ARGS__)
#else  // __cplusplus
#define BARELY_GENERATE_CUSTOM_EFFECT_DEFINITION(name, ...) \
  _BARELY_GENERATE_CUSTOM_EFFECT_DEFINITION(C, name, __VA_ARGS__)

#define BARELY_GENERATE_CUSTOM_INSTRUMENT_DEFINITION(name, ...) \
  _BARELY_GENERATE_CUSTOM_INSTRUMENT_DEFINITION(C, name, __VA_ARGS__)
#endif  // __cplusplus

#define _BARELY_NULL(...)

#define _BARELY_ARG1(_0, arg1, ...) arg1

#define _BARELY_ARG2(_0, _1, arg2, ...) arg2

#define _BARELY_ARG_OR_NULL(arg, ...) \
  _BARELY_ARG##arg(arg, __VA_ARGS__, _BARELY_NULL, _BARELY_NULL)

#define _BARELY_ENUM_VALUE_C(name, value, ...) Barely##name##_k##value,

#define _BARELY_GENERATE_ENUM_C(name, values) \
  /* Enum alias. */                           \
  typedef int32_t Barely##name;               \
  /* Enum values. */                          \
  enum Barely##name##_Values{values(name, _BARELY_ENUM_VALUE_C)};

#define _BARELY_GENERATE_CUSTOM_DEFINITION_C(name, type) \
  /* Returns the custom definition. */                   \
  BARELY_EXPORT Barely##type##Definition Barely##name##Definition();

#define _BARELY_GENERATE_CUSTOM_EFFECT_DEFINITION_C(name, ...) \
  _BARELY_GENERATE_CUSTOM_DEFINITION_C(name, Effect)

#define _BARELY_GENERATE_CUSTOM_INSTRUMENT_DEFINITION_C(name, ...) \
  _BARELY_GENERATE_CUSTOM_DEFINITION_C(name, Instrument)

#ifdef __cplusplus
#define _BARELY_ENUM_VALUE_CPP(name, value, ...) \
  k##value = Barely##name##_k##value,

#define _BARELY_GENERATE_ENUM_CPP(name, values) \
  /* Enum. */                                   \
  enum class name : Barely##name{values(name, _BARELY_ENUM_VALUE_CPP)};

#define _BARELY_CONTROL_DEFINITION_VALUE_CPP(name, value, ...) \
  ::barely::ControlDefinition(__VA_ARGS__),

#define _BARELY_GENERATE_CUSTOM_DEFINITION_CPP(name, type)               \
  /* Custom definition. */                                               \
  class name##Definition : public ::barely::type##Definition {           \
   public:                                                               \
    name##Definition() : type##Definition(Barely##name##Definition()) {} \
  };

#define _BARELY_GENERATE_CUSTOM_EFFECT_DEFINITION_CPP(name, controls)        \
  class name;                                                                \
  /* Returns the custom effect definition. */                                \
  inline BarelyEffectDefinition Barely##name##Definition() {                 \
    static const std::vector<::barely::ControlDefinition>                    \
        control_definitions = {                                              \
            controls(name, _BARELY_CONTROL_DEFINITION_VALUE_CPP)};           \
    return ::barely::CustomEffect::GetDefinition<name>(control_definitions); \
  }                                                                          \
  _BARELY_GENERATE_CUSTOM_DEFINITION_CPP(name, Effect)

#define _BARELY_GENERATE_CUSTOM_INSTRUMENT_DEFINITION_CPP(name, controls, \
                                                          note_controls)  \
  class name;                                                             \
  /* Returns the custom instrument definition. */                         \
  inline BarelyInstrumentDefinition Barely##name##Definition() {          \
    static const std::vector<::barely::ControlDefinition>                 \
        control_definitions = {                                           \
            controls(name, _BARELY_CONTROL_DEFINITION_VALUE_CPP)};        \
    static const std::vector<::barely::ControlDefinition>                 \
        note_control_definitions = {                                      \
            note_controls(name, _BARELY_CONTROL_DEFINITION_VALUE_CPP)};   \
    return ::barely::CustomInstrument::GetDefinition<name>(               \
        control_definitions, note_control_definitions);                   \
  }                                                                       \
  _BARELY_GENERATE_CUSTOM_DEFINITION_CPP(name, Instrument)
#endif  // __cplusplus

#define _BARELY_GENERATE_CUSTOM_EFFECT_DEFINITION(target, name, ...)  \
  _BARELY_GENERATE_ENUM_##target(name##Control,                       \
                                 _BARELY_ARG_OR_NULL(1, __VA_ARGS__)) \
      _BARELY_GENERATE_CUSTOM_EFFECT_DEFINITION_##target(             \
          name, _BARELY_ARG_OR_NULL(1, __VA_ARGS__))

#define _BARELY_GENERATE_CUSTOM_INSTRUMENT_DEFINITION(target, name, ...)  \
  _BARELY_GENERATE_ENUM_##target(name##Control,                           \
                                 _BARELY_ARG_OR_NULL(1, __VA_ARGS__))     \
      _BARELY_GENERATE_ENUM_##target(name##NoteControl,                   \
                                     _BARELY_ARG_OR_NULL(2, __VA_ARGS__)) \
          _BARELY_GENERATE_CUSTOM_INSTRUMENT_DEFINITION_##target(         \
              name, _BARELY_ARG_OR_NULL(1, __VA_ARGS__),                  \
              _BARELY_ARG_OR_NULL(2, __VA_ARGS__))

#endif  // BARELYMUSICIAN_COMMON_MACROS_H_
