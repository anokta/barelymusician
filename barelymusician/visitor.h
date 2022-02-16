#ifndef BARELYMUSICIAN_VISITOR_H_
#define BARELYMUSICIAN_VISITOR_H_

namespace barelyapi {

/// Visitor template.
template <class... Types>
struct Visitor : Types... {
  using Types::operator()...;
};
template <class... Types>
Visitor(Types...) -> Visitor<Types...>;

}  // namespace barelyapi

#endif  // BARELYMUSICIAN_VISITOR_H_
