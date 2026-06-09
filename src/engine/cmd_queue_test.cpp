#include "engine/cmd_queue.h"

#include <cstddef>
#include <cstdint>
#include <memory>

#include "core/arena.h"
#include "engine/cmd.h"
#include "gmock/gmock-matchers.h"
#include "gtest/gtest.h"

using ::testing::AllOf;
using ::testing::Field;
using ::testing::IsNull;
using ::testing::NotNull;
using ::testing::Pair;
using ::testing::Pointee;
using ::testing::VariantWith;

namespace barely {
namespace {

TEST(CmdQueueTest, AddSingleCmd) {
  const auto size = GetAllocSize<CmdQueue>();
  auto data = std::make_unique<std::byte[]>(size);
  Arena arena(data.get(), size);
  CmdQueue cmds(arena);
  EXPECT_THAT(cmds.GetNext(0), IsNull());
  EXPECT_THAT(cmds.GetNext(1), IsNull());
  EXPECT_THAT(cmds.GetNext(10), IsNull());

  cmds.Add(1, InstrumentCreateCmd{5});
  EXPECT_THAT(cmds.GetNext(0), IsNull());
  EXPECT_THAT(cmds.GetNext(1), IsNull());
  EXPECT_THAT(cmds.GetNext(10),
              AllOf(NotNull(), Pointee(Pair(1, VariantWith<InstrumentCreateCmd>(Field(
                                                   &InstrumentCreateCmd::instrument_index, 5))))));

  // Cmd is already returned.
  EXPECT_THAT(cmds.GetNext(10), IsNull());
}

TEST(CmdQueueTest, AddMultipleCmds) {
  const size_t size = GetAllocSize<CmdQueue>();
  auto data = std::make_unique<std::byte[]>(size);
  Arena arena(data.get(), size);
  CmdQueue cmds(arena);
  EXPECT_THAT(cmds.GetNext(10), IsNull());

  for (uint32_t i = 0; i < 10; ++i) {
    cmds.Add(i, InstrumentCreateCmd{i});
  }
  for (uint32_t i = 0; i < 10; ++i) {
    EXPECT_THAT(
        cmds.GetNext(10),
        AllOf(NotNull(), Pointee(Pair(i, VariantWith<InstrumentCreateCmd>(
                                             Field(&InstrumentCreateCmd::instrument_index, i))))));
  }

  // All cmds are already returned.
  EXPECT_THAT(cmds.GetNext(10), IsNull());
}

}  // namespace
}  // namespace barely
