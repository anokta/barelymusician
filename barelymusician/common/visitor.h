#ifndef BARELYMUSICIAN_COMMON_VISITOR_H_
#define BARELYMUSICIAN_COMMON_VISITOR_H_

namespace barely {

/// Visitor template.
template <class... Types>
struct Visitor : Types... {
  using Types::operator()...;
};
template <class... Types>
Visitor(Types...) -> Visitor<Types...>;

}  // namespace barely

#endif  // BARELYMUSICIAN_COMMON_VISITOR_H_
