#ifndef BARELYMUSICIAN_COMMON_RESTRICT_H_
#define BARELYMUSICIAN_COMMON_RESTRICT_H_

#if defined(_MSC_VER)
#define BARELY_RESTRICT __restrict
#elif defined(__clang__) || defined(__GNUC__)
#define BARELY_RESTRICT __restrict__
#else  // defined(__clang__) || defined(__GNUC__)
#define BARELY_RESTRICT
#endif  // defined(__clang__) || defined(__GNUC__)

#endif  // BARELYMUSICIAN_COMMON_RESTRICT_H_
