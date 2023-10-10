#
# author: Adam Budziak
#

add_executable(MailCrypt1 mail_crypt1.cpp)
target_link_libraries(MailCrypt1 GraFPE DFARE)

add_executable(MailCrypt2 mail_crypt2.cpp)
target_link_libraries(MailCrypt2 GraFPE DFARE)

add_executable(CCNCrypt ccn_crypt.cpp)
target_link_libraries(CCNCrypt GraFPE DFARE)

set_property(TARGET MailCrypt2 PROPERTY CXX_STANDARD 17)
set_property(TARGET MailCrypt2 PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET MailCrypt1 PROPERTY CXX_STANDARD 17)
set_property(TARGET MailCrypt1 PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET CCNCrypt PROPERTY CXX_STANDARD 17)
set_property(TARGET CCNCrypt PROPERTY CXX_STANDARD_REQUIRED ON)
