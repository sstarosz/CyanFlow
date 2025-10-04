#include "Core/UndoStack.hpp"

#include "gtest/gtest.h"

using namespace cf;
using namespace cf::core;

class UndoStackTest : public ::testing::Test {
protected:
    UndoStack undoStack;
};

class TestCommand : public Command {
public:
    TestCommand(int& valueRef, int newValue)
        : value(valueRef)
        , oldValue(valueRef)
        , newValue(newValue)
    {
    }

    void execute() override
    {
        value = newValue;
    }

    void undo() override
    {
        value = oldValue;
    }

private:
    int& value;
    int oldValue;
    int newValue;
};

TEST_F(UndoStackTest, BasicUsage)
{
    int value = 0;

    EXPECT_FALSE(undoStack.canUndo());
    EXPECT_FALSE(undoStack.canRedo());

    undoStack.push(std::make_unique<TestCommand>(value, 42));
    EXPECT_EQ(value, 42);
    EXPECT_TRUE(undoStack.canUndo());
    EXPECT_FALSE(undoStack.canRedo());

    undoStack.undo();
    EXPECT_EQ(value, 0);
    EXPECT_FALSE(undoStack.canUndo());
    EXPECT_TRUE(undoStack.canRedo());

    undoStack.redo();
    EXPECT_EQ(value, 42);
    EXPECT_TRUE(undoStack.canUndo());
    EXPECT_FALSE(undoStack.canRedo());

    undoStack.push(std::make_unique<TestCommand>(value, 100));
    EXPECT_EQ(value, 100);
    EXPECT_TRUE(undoStack.canUndo());
    EXPECT_FALSE(undoStack.canRedo());

    undoStack.undo();
    EXPECT_EQ(value, 42);
    EXPECT_TRUE(undoStack.canUndo());
    EXPECT_TRUE(undoStack.canRedo());

    undoStack.undo();
    EXPECT_EQ(value, 0);
    EXPECT_FALSE(undoStack.canUndo());
    EXPECT_TRUE(undoStack.canRedo());

    undoStack.redo();
    EXPECT_EQ(value, 42);
    EXPECT_TRUE(undoStack.canUndo());
    EXPECT_TRUE(undoStack.canRedo());

    undoStack.redo();
    EXPECT_EQ(value, 100);
    EXPECT_TRUE(undoStack.canUndo());
    EXPECT_FALSE(undoStack.canRedo());

    undoStack.clear();
    EXPECT_FALSE(undoStack.canUndo());
    EXPECT_FALSE(undoStack.canRedo());
}

TEST_F(UndoStackTest, UndoWithoutCommands)
{
    EXPECT_FALSE(undoStack.canUndo());
    EXPECT_FALSE(undoStack.canRedo());

    // Should not crash or throw
    undoStack.undo();
    undoStack.redo();

    EXPECT_FALSE(undoStack.canUndo());
    EXPECT_FALSE(undoStack.canRedo());
}

TEST_F(UndoStackTest, MultipleUndosRedos)
{
    int value = 0;

    for (int i = 1; i <= 5; ++i) {
        undoStack.push(std::make_unique<TestCommand>(value, i * 10));
        EXPECT_EQ(value, i * 10);
    }

    for (int i = 5; i >= 1; --i) {
        EXPECT_TRUE(undoStack.canUndo());
        undoStack.undo();
        EXPECT_EQ(value, (i - 1) * 10);
    }

    EXPECT_FALSE(undoStack.canUndo());
    EXPECT_TRUE(undoStack.canRedo());

    for (int i = 1; i <= 5; ++i) {
        EXPECT_TRUE(undoStack.canRedo());
        undoStack.redo();
        EXPECT_EQ(value, i * 10);
    }

    EXPECT_TRUE(undoStack.canUndo());
    EXPECT_FALSE(undoStack.canRedo());
}

TEST_F(UndoStackTest, PushClearsRedoStack)
{
    int value = 0;

    undoStack.push(std::make_unique<TestCommand>(value, 10));
    EXPECT_EQ(value, 10);

    undoStack.push(std::make_unique<TestCommand>(value, 20));
    EXPECT_EQ(value, 20);

    undoStack.undo();
    EXPECT_EQ(value, 10);
    EXPECT_TRUE(undoStack.canRedo());

    // Pushing a new command should clear the redo stack
    undoStack.push(std::make_unique<TestCommand>(value, 30));
    EXPECT_EQ(value, 30);
    EXPECT_FALSE(undoStack.canRedo());
}

TEST_F(UndoStackTest, ClearEmptiesStacks)
{
    int value = 0;

    undoStack.push(std::make_unique<TestCommand>(value, 10));
    EXPECT_EQ(value, 10);

    undoStack.push(std::make_unique<TestCommand>(value, 20));
    EXPECT_EQ(value, 20);

    undoStack.undo();
    EXPECT_EQ(value, 10);
    EXPECT_TRUE(undoStack.canRedo());

    undoStack.clear();
    EXPECT_FALSE(undoStack.canUndo());
    EXPECT_FALSE(undoStack.canRedo());

    // After clearing, pushing new commands should work as expected
    undoStack.push(std::make_unique<TestCommand>(value, 30));
    EXPECT_EQ(value, 30);
    EXPECT_TRUE(undoStack.canUndo());
    EXPECT_FALSE(undoStack.canRedo());
}