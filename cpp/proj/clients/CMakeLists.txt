#
# author: Adam Budziak
#

add_executable(AsciiCrypt ascii_crypt.cpp)
target_link_libraries(AsciiCrypt GraFPE)

add_executable(LoggerTest logger_test.cpp)
target_link_libraries(LoggerTest GraFPE pur)

add_executable(TestDistribution test_distribution.cpp)
target_link_libraries(TestDistribution GraFPE)

add_executable(DfaFromRegex dfa_from_regex.cpp)
target_link_libraries(DfaFromRegex DFARE)

add_executable(BigNumCrypter bignum_crypter.cpp)
target_link_libraries(BigNumCrypter GraFPE)

add_executable(GenGraph gen_graph.cpp)
target_link_libraries(GenGraph GraFPE)

add_executable(IRS irs.cpp)
target_link_libraries(IRS GraFPE)
