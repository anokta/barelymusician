#include "barelymusician/internal/observable.h"

#include <memory>

#include "gtest/gtest.h"

namespace barely::internal {
namespace {

// Tests that an observable value can be observed by a single observer as expected.
TEST(ObservableTest, SingleObserver) {
  // Create an observable value.
  Observable<int> observable{1};
  EXPECT_EQ(*observable, 1);

  // Mutate the observable value.
  *observable = 5;
  EXPECT_EQ(*observable, 5);

  {
    // Create an observer.
    Observer<int> observer = observable.Observe();
    ASSERT_TRUE(observer);
    EXPECT_EQ(*observer, 5);

    // Mutate the observable value with the observer.
    *observer = 10;
    EXPECT_EQ(*observable, 10);
    EXPECT_EQ(*observer, 10);
  }

  EXPECT_EQ(*observable, 10);
}

// Tests that an observable value can be observed by multiple observers as expected.
TEST(ObservableTest, MultipleObservers) {
  static int constructor_count = 0;
  static int destructor_count = 0;
  struct TestData {
    TestData() noexcept { ++constructor_count; }
    ~TestData() noexcept { ++destructor_count; }
    int value = 0;
  };

  // One of the observers will be allocated in the heap.
  std::unique_ptr<Observer<TestData>> observer_in_heap = nullptr;
  EXPECT_EQ(constructor_count, 0);
  EXPECT_EQ(destructor_count, 0);

  {
    // Create an observable value.
    Observable<TestData> observable;
    EXPECT_EQ(observable->value, 0);
    EXPECT_EQ(constructor_count, 1);
    EXPECT_EQ(destructor_count, 0);

    // Mutate the observable value.
    observable->value = 20;
    EXPECT_EQ(observable->value, 20);
    EXPECT_EQ(constructor_count, 1);
    EXPECT_EQ(destructor_count, 0);

    {
      // Create an observer in the stack.
      Observer<TestData> observer_in_stack = observable.Observe();
      ASSERT_TRUE(observer_in_stack);
      EXPECT_EQ(observer_in_stack->value, 20);
      EXPECT_EQ(constructor_count, 1);
      EXPECT_EQ(destructor_count, 0);

      // Mutate the observable value with the observer in the stack.
      observer_in_stack->value = 30;
      EXPECT_EQ(observer_in_stack->value, 30);
      EXPECT_EQ(observable->value, 30);
      EXPECT_EQ(constructor_count, 1);
      EXPECT_EQ(destructor_count, 0);

      // Create an observer in the heap.
      observer_in_heap = std::make_unique<Observer<TestData>>(observable.Observe());
      ASSERT_TRUE(*observer_in_heap);
      EXPECT_TRUE(observer_in_stack);
      EXPECT_EQ((*observer_in_heap)->value, 30);
      EXPECT_EQ(constructor_count, 1);
      EXPECT_EQ(destructor_count, 0);

      // Mutate the observable value with the observer in the stack.
      (*observer_in_heap)->value = 40;
      EXPECT_EQ((*observer_in_heap)->value, 40);
      EXPECT_EQ(observer_in_stack->value, 40);
      EXPECT_EQ(observable->value, 40);
      EXPECT_EQ(constructor_count, 1);
      EXPECT_EQ(destructor_count, 0);
    }

    // Observer in the stack is out of scope.
    EXPECT_TRUE(*observer_in_heap);
    EXPECT_EQ((*observer_in_heap)->value, 40);
    EXPECT_EQ(observable->value, 40);
    EXPECT_EQ(constructor_count, 1);
    EXPECT_EQ(destructor_count, 0);
  }

  // Observable is out of scope.
  EXPECT_FALSE(*observer_in_heap);
  EXPECT_EQ(constructor_count, 1);
  EXPECT_EQ(destructor_count, 1);
}

}  // namespace
}  // namespace barely::internal
