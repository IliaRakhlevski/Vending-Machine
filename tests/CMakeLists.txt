# Every test is an ordinary program that returns 0 when all its checks pass.
# One executable per test file, so that a crash in one does not hide the others.
#
# Add new test files (without .cpp) to this list.
set(VENDING_TESTS
    test_vending_machine
    test_coin_system
)

foreach(t ${VENDING_TESTS})
    add_executable(${t} ${t}.cpp)
    target_link_libraries(${t} PRIVATE vending)
    add_test(NAME ${t} COMMAND ${t})
endforeach()
